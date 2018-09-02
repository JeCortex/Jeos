/*
 mp.c for the kernel software

 Contact: JeCortex@yahoo.com

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
*/

/* 
 * Multiprocessor support
 * search memory for MP description structures.
 */

#include <linux/types.h>
#include <linux/mm.h>
#include <linux/os.h>
#include <asm/mp.h>
#include <asm/x86.h>
#include <asm/string.h>

struct cpu {
        uint8 apicid;
};

struct cpu cpus[NCPU];
int ncpu;

static uint8 sum(uint8 *addr, int len)
{
        int i, sum;

        sum = 0;
        for (i=0; i<len; i++)
                sum += addr[i];
        return sum;
}

/* Look for an MP structure in the len bytes at addr. */
static struct mp* mpsearch1(uint32 a, int len)
{
        uint8 *e, *p, *addr;
        addr = PA2VA(a);
        e = addr+len;

        for (p = addr; p < e; p += sizeof(struct mp))
                if (memcmp(p, "_MP_", 4) == 0 && sum(p, sizeof(struct mp)) == 0)
                        return (struct mp*)p;
        return 0;
}

/* 
 * Search for the MP Floating Pointer Structure, which according to the
 * spec is in one of the following three locations:
 * 1) in the first KB of the EBDA;
 * 2) in the last KB of system base memory;
 * 3) in the BIOS ROM between 0xE0000 and 0xFFFFF.
 */
static struct mp* mpsearch(void)
{
        uint8 *bda;
        uint32 p;
        struct mp *mp;

        bda = (uint8 *) PA2VA(0x400);
        if ((p = ((bda[0x0F]<<8)| bda[0x0E]) << 4)){
                if ((mp = mpsearch1(p, 1024)))
                        return mp;
        } else {
                p = ((bda[0x14]<<8)|bda[0x13])*1024;
                if ((mp = mpsearch1(p-1024, 1024)))
                        return mp;
        }
        return mpsearch1(0xF0000, 0x10000);
}

/* 
 * Search for an MP configuration table.  For now,
 * don't accept the default configurations (physaddr == 0).
 * check for correct signature, calculate the checksum and,
 * if correct, check the version.
 * TODO: check extended table checksum.
 */
static struct mpconf* mpconfig(struct mp **pmp)
{
        struct mpconf *conf;
        struct mp *mp;

        if ((mp = mpsearch()) == 0 || mp->physaddr == 0)
                return 0;
        conf = (struct mpconf*) PA2VA((uint32) mp->physaddr);
        if (memcmp(conf, "PCMP", 4) != 0)
                return 0;
        if (conf->version != 1 && conf->version != 4)
                return 0;
        if (sum((uint8*)conf, conf->length) != 0)
                return 0;
        *pmp = mp;

        return conf;
}

void mp_init(struct mparch *mparch)
{
        uint8 *p, *e;
        int ismp;

        if ((mparch->mpc = mpconfig(&mparch->mp)) == 0)
                panic("Expect to run on an SMP");
        ismp = 1;

        for (p = (uint8*)(mparch->mpc + 1), e = (uint8*)mparch->mpc +
                        mparch->mpc->length; p < e;){
                switch (*p) {
                case MPPROC:
                        mparch->mpp = (struct mpproc*)p;
                        if (ncpu < NCPU) {
                                cpus[ncpu].apicid = mparch->mpp->apicid;  /* apicid may differ from ncpu */
                                ncpu++;
                        }
                        p += sizeof(struct mpproc);
                        kprintf(WHITE, "In MPPROC apicid=%d ...\n", mparch->mpp->apicid);
                        continue;
                case MPIOAPIC:
                        mparch->mpi = (struct mpioapic*)p;
                        mparch->apic.ioapicid = mparch->mpi->apicno;
                        p += sizeof(struct mpioapic);
                        kprintf(WHITE, "In MPIOAPIC ioapicid=%d ...\n", mparch->mpi->apicno);
                        continue;
                case MPBUS:
                case MPIOINTR:
                case MPLINTR:
                        kprintf(WHITE, "In OTHERS...\n");
                        p += 8;
                        continue;
                default:
                        kprintf(WHITE, "In DEFAULT...\n");
                        ismp = 0;
                        break;
                }
        }
        if (!ismp)
                panic("Didn't find a suitable machine");

        if (mparch->mp->imcrp) {
                /* 
                 * Bochs doesn't support IMCR, so this doesn't run on Bochs.
                 * but it would on real hardware.
                 */
                outb(0x22, 0x70);   /* Select IMCR */
                outb(0x23, inb(0x23) | 1);  /* Mask external interrupts. */
        }
}
