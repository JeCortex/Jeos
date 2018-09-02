/*
 mp.h for the kernel software

 Contact: JeCortex@yahoo.com

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
*/

#ifndef __MP_H__
#define __MP_H__

#define NCPU          8  /* maximum number of CPUs */

/* table entry types */
#define MPPROC    0x00  /* one per processor */
#define MPBUS     0x01  /* one per bus */
#define MPIOAPIC  0x02  /* one per I/O APIC */
#define MPIOINTR  0x03  /* one per bus interrupt source */
#define MPLINTR   0x04  /* one per system interrupt source */

/* IO APIC MMIO structure: write reg, then read or write data. */
struct ioapic {
        uint32 reg;
        uint32 pad[3];
        uint32 data;
};

struct apic {
        uint8 ioapicid;
        volatile uint32 *lapic;
        volatile struct ioapic *ioapic;
};

/* see MultiProcessor Specification Version 1.[14] */
struct mparch {
        struct mp *mp;
        struct mpconf *mpc;
        struct mpproc *mpp;
        struct mpioapic *mpi;
        struct apic apic;
};

struct mp {             /* floating pointer */
        uint8 signature[4];           /* "_MP_" */
        void *physaddr;               /* phys addr of MP config table */
        uint8 length;                 /* 1 */
        uint8 specrev;                /* [14] */
        uint8 checksum;               /* all bytes must add up to 0 */
        uint8 type;                   /* MP system config type */
        uint8 imcrp;
        uint8 reserved[3];
};

struct mpconf {         /* configuration table header */
        uint8 signature[4];           /* "PCMP" */
        uint16 length;                /* total table length */
        uint8 version;                /* [14] */
        uint8 checksum;               /* all bytes must add up to 0 */
        uint8 product[20];            /* product id */
        uint32 *oemtable;               /* OEM table pointer */
        uint16 oemlength;             /* OEM table length */
        uint16 entry;                 /* entry count */
        uint32 *lapicaddr;              /* address of local APIC */
        uint16 xlength;               /* extended table length */
        uint8 xchecksum;              /* extended table checksum */
        uint8 reserved;
};

struct mpproc {         /* processor table entry */
        uint8 type;                   /* entry type (0) */
        uint8 apicid;                 /* local APIC id */
        uint8 version;                /* local APIC verison */
        uint8 flags;                  /* CPU flags */
        #define MPBOOT 0x02           /* This proc is the bootstrap processor. */
        uint8 signature[4];           /* CPU signature */
        uint32 feature;                 /* feature flags from CPUID instruction */
        uint8 reserved[8];
};

struct mpioapic {       /* I/O APIC table entry */
        uint8 type;                   /* entry type (2) */
        uint8 apicno;                 /* I/O APIC id */
        uint8 version;                /* I/O APIC version */
        uint8 flags;                  /* I/O APIC flags */
        uint32 *addr;                   /* I/O APIC address */
};

extern void mp_init(struct mparch *mparch);
extern void lapic_init(struct apic *apic);
extern void ioapic_init(struct apic *apic);
extern void ioapicenable(struct apic *apic, int irq, int cpunum);

#endif
