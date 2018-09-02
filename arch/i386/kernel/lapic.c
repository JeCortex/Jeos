/*
 lapic.c for the kernel software

 Contact: JeCortex@yahoo.com

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
*/

/* 
 * The local APIC manages internal (non-I/O) interrupts.
 * see Chapter 8 & Appendix C of Intel processor manual volume 3.
 */

#include <linux/types.h>
#include <linux/mm.h>
#include <asm/x86.h>
#include <asm/traps.h>
#include <asm/string.h>
#include <asm/mp.h>

/* local APIC registers, divided by 4 for use as uint32[] indices. */
#define ID      (0x0020/4)   /* ID */
#define VER     (0x0030/4)   /* version */
#define TPR     (0x0080/4)   /* task Priority */
#define EOI     (0x00B0/4)   /* EOI */
#define SVR     (0x00F0/4)   /* spurious Interrupt Vector */
#define ENABLE  0x00000100   /* unit Enable */
#define ESR     (0x0280/4)   /* error Status */
#define ICRLO   (0x0300/4)   /* interrupt Command */
#define INIT       0x00000500   /* INIT/RESET */
#define STARTUP    0x00000600   /* startup IPI */
#define DELIVS     0x00001000   /* delivery status */
#define ASSERT     0x00004000   /* assert interrupt (vs deassert) */
#define DEASSERT   0x00000000
#define LEVEL      0x00008000   /* level triggered */
#define BCAST      0x00080000   /* send to all APICs, including self. */
#define BUSY       0x00001000
#define FIXED      0x00000000
#define ICRHI   (0x0310/4)      /* interrupt Command [63:32] */
#define TIMER   (0x0320/4)      /* local Vector Table 0 (TIMER) */
#define X1         0x0000000B   /* divide counts by 1 */
#define PERIODIC   0x00020000   /* periodic */
#define PCINT   (0x0340/4)      /* performance Counter LVT */
#define LINT0   (0x0350/4)      /* local Vector Table 1 (LINT0) */
#define LINT1   (0x0360/4)      /* local Vector Table 2 (LINT1) */
#define ERROR   (0x0370/4)      /* local Vector Table 3 (ERROR) */
#define MASKED  0x00010000      /* interrupt masked */
#define TICR    (0x0380/4)      /* timer Initial Count */
#define TCCR    (0x0390/4)      /* timer Current Count */
#define TDCR    (0x03E0/4)      /* timer Divide Configuration */

struct rtcdate {
        uint32 second;
        uint32 minute;
        uint32 hour;
        uint32 day;
        uint32 month;
        uint32 year;
};

static void lapicw(volatile uint32 *lapic, int index, int value)
{
        lapic[index] = value;
        lapic[ID];  /* wait for write to finish, by reading */
}

void lapic_init(struct apic *apic)
{
        if (!apic->lapic)
                return;

        /* enable local APIC; set spurious interrupt vector. */
        lapicw(apic->lapic, SVR, ENABLE | (T_IRQ0 + IRQ_SPURIOUS));

        /* 
         * The timer repeatedly counts down at bus frequency
         * from lapic[TICR] and then issues an interrupt.
         * if xv6 cared more about precise timekeeping,
         * TICR would be calibrated using an external time source.
         */
        lapicw(apic->lapic, TDCR, X1);
        lapicw(apic->lapic, TIMER, PERIODIC | (T_IRQ0 + IRQ_TIMER));
        lapicw(apic->lapic, TICR, 10000000);

        /* disable logical interrupt lines. */
        lapicw(apic->lapic, LINT0, MASKED);
        lapicw(apic->lapic, LINT1, MASKED);

        /* 
         * Disable performance counter overflow interrupts
         * on machines that provide that interrupt entry.
         */
        if (((apic->lapic[VER]>>16) & 0xFF) >= 4)
                lapicw(apic->lapic, PCINT, MASKED);

        /* map error interrupt to IRQ_ERROR. */
        lapicw(apic->lapic, ERROR, T_IRQ0 + IRQ_ERROR);

        /* clear error status register (requires back-to-back writes). */
        lapicw(apic->lapic, ESR, 0);
        lapicw(apic->lapic, ESR, 0);

        /* ack any outstanding interrupts. */
        lapicw(apic->lapic, EOI, 0);

        /* send an Init Level De-Assert to synchronise arbitration ID's. */
        lapicw(apic->lapic, ICRHI, 0);
        lapicw(apic->lapic, ICRLO, BCAST | INIT | LEVEL);
        while (apic->lapic[ICRLO] & DELIVS)
                ;

        /* enable interrupts on the APIC (but not on the processor). */
        lapicw(apic->lapic, TPR, 0);
}

int lapicid(volatile uint32 *lapic)
{
        if (!lapic)
                return 0;
        return lapic[ID] >> 24;
}

/* acknowledge interrupt. */
void lapiceoi(volatile uint32 *lapic)
{
        if (lapic)
                lapicw(lapic, EOI, 0);
}

/* 
 * Spin for a given number of microseconds.
 * on real hardware would want to tune this dynamically.
 */
void microdelay(int us)
{
        ;
}

#define CMOS_PORT    0x70
#define CMOS_RETURN  0x71

/* 
 * Start additional processor running entry code at addr.
 * see Appendix B of MultiProcessor Specification.
 */
void lapicstartap(volatile uint32 *lapic, uint8 apicid, uint32 addr)
{
        int i;
        uint16 *wrv;

        /* 
         * "The BSP must initialize CMOS shutdown code to 0AH
         * and the warm reset vector (DWORD based at 40:67) to point at
         * the AP startup code prior to the [universal startup algorithm]."
         */
        outb(CMOS_PORT, 0xF);  /* offset 0xF is shutdown code */
        outb(CMOS_PORT+1, 0x0A);
        wrv = (uint16*)PA2VA((0x40<<4 | 0x67));  /* warm reset vector */
        wrv[0] = 0;
        wrv[1] = addr >> 4;

        /* 
         * "Universal startup algorithm."
         * send INIT (level-triggered) interrupt to reset other CPU.
         */
        lapicw(lapic, ICRHI, apicid<<24);
        lapicw(lapic, ICRLO, INIT | LEVEL | ASSERT);
        microdelay(200);
        lapicw(lapic, ICRLO, INIT | LEVEL);
        microdelay(100);    /* should be 10ms, but too slow in Bochs! */

        /* 
         * Send startup IPI (twice!) to enter code.
         * regular hardware is supposed to only accept a STARTUP
         * when it is in the halted state due to an INIT.  So the second
         * should be ignored, but it is part of the official Intel algorithm.
         * Bochs complains about the second one.  Too bad for Bochs.
         */
        for (i = 0; i < 2; i++){
                lapicw(lapic, ICRHI, apicid<<24);
                lapicw(lapic, ICRLO, STARTUP | (addr>>12));
                microdelay(200);
        }
}

#define CMOS_STATA   0x0a
#define CMOS_STATB   0x0b
#define CMOS_UIP    (1 << 7)        /* RTC update in progress */

#define SECS    0x00
#define MINS    0x02
#define HOURS   0x04
#define DAY     0x07
#define MONTH   0x08
#define YEAR    0x09

static uint32 _cmos_read(uint32 reg)
{
        outb(CMOS_PORT,  reg);
        microdelay(200);

        return inb(CMOS_RETURN);
}

static void fill_rtcdate(struct rtcdate *r)
{
        r->second = _cmos_read(SECS);
        r->minute = _cmos_read(MINS);
        r->hour   = _cmos_read(HOURS);
        r->day    = _cmos_read(DAY);
        r->month  = _cmos_read(MONTH);
        r->year   = _cmos_read(YEAR);
}

/* qemu seems to use 24-hour GWT and the values are BCD encoded */
void cmostime(struct rtcdate *r)
{
        struct rtcdate t1, t2;
        int sb, bcd;

        sb = _cmos_read(CMOS_STATB);

        bcd = (sb & (1 << 2)) == 0;

        /* make sure CMOS doesn't modify time while we read it */
        for (;;) {
                fill_rtcdate(&t1);
                if (_cmos_read(CMOS_STATA) & CMOS_UIP)
                        continue;
                fill_rtcdate(&t2);
                if (memcmp(&t1, &t2, sizeof(t1)) == 0)
                        break;
        }

        /* convert */
        if (bcd) {
                #define    CONV(x)     (t1.x = ((t1.x >> 4) * 10) + (t1.x & 0xf))
                CONV(second);
                CONV(minute);
                CONV(hour  );
                CONV(day   );
                CONV(month );
                CONV(year  );
                #undef     CONV
        }

        *r = t1;
        r->year += 2000;
}
