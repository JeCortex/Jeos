/*
 picirq.c for the kernel software

 Contact: JeCortex@yahoo.com

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
*/

#include <linux/types.h>
#include <asm/x86.h>
#include <asm/traps.h>

/* I/O Addresses of the two programmable interrupt controllers */
#define IO_PIC1         0x20    /* Master (IRQs 0-7) */
#define IO_PIC2         0xA0    /* Slave (IRQs 8-15) */

/* don't use the 8259A interrupt controllers.  Xv6 assumes SMP hardware. */
void pic_init(void)
{
        /* mask all interrupts */
        outb(IO_PIC1+1, 0xFF);
        outb(IO_PIC2+1, 0xFF);
}
