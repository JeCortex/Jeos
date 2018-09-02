/*
 ioapic.c for the kernel software

 Contact: JeCortex@yahoo.com

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
*/

/* 
 * The I/O APIC manages hardware interrupts for an SMP system.
 * http://www.intel.com/design/chipsets/datashts/29056601.pdf
 * see also picirq.c.
 */

#include <linux/types.h>
#include <asm/mp.h>
#include <asm/traps.h>

#define IOAPIC  0xFEC00000   /* default physical address of IO APIC */

#define REG_ID     0x00  /* register index: ID */
#define REG_VER    0x01  /* register index: version */
#define REG_TABLE  0x10  /* redirection table base */

/* 
 * The redirection table starts at REG_TABLE and uses
 * two registers to configure each interrupt.
 * The first (low) register in a pair contains configuration bits.
 * The second (high) register contains a bitmask telling which
 * CPUs can serve that interrupt.
 */
#define INT_DISABLED   0x00010000  /* interrupt disabled */
#define INT_LEVEL      0x00008000  /* level-triggered (vs edge-) */
#define INT_ACTIVELOW  0x00002000  /* active low (vs high) */
#define INT_LOGICAL    0x00000800  /* destination is CPU id (vs APIC ID) */

static uint32 ioapicread(volatile struct ioapic *ioapic, int reg)
{
        ioapic->reg = reg;
        return ioapic->data;
}

static void ioapicwrite(volatile struct ioapic *ioapic, int reg, uint32 data)
{
        ioapic->reg = reg;
        ioapic->data = data;
}

void ioapic_init(struct apic *apic)
{
        int i, id, maxintr;

        apic->ioapic = (volatile struct ioapic*)IOAPIC;
        maxintr = (ioapicread(apic->ioapic, REG_VER) >> 16) & 0xFF;
        id = ioapicread(apic->ioapic, REG_ID) >> 24;
        if (id != apic->ioapicid)
                //cprintf("ioapicinit: id isn't equal to ioapicid; not a MP\n");

        /* 
         * Mark all interrupts edge-triggered, active high, disabled,
         * and not routed to any CPUs.
         */
        for (i = 0; i <= maxintr; i++) {
                ioapicwrite(apic->ioapic, REG_TABLE+2*i, INT_DISABLED | (T_IRQ0 + i));
                ioapicwrite(apic->ioapic, REG_TABLE+2*i+1, 0);
        }
}

void ioapicenable(struct apic *apic, int irq, int cpunum)
{
        /* 
         * Mark interrupt edge-triggered, active high,
         * enabled, and routed to the given cpunum,
         * which happens to be that cpu's APIC ID.
         */
        ioapicwrite(apic->ioapic, REG_TABLE+2*irq, T_IRQ0 + irq);
        ioapicwrite(apic->ioapic, REG_TABLE+2*irq+1, cpunum << 24);
}
