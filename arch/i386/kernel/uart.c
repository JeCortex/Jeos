/*
 uart.c for the kernel software

 Contact: JeCortex@yahoo.com

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
*/

/* 
 * Intel 8250 serial port (UART). 
 */

#include <linux/types.h>
#include <linux/mm.h>
#include <asm/x86.h>

#define COM1    0x3f8

static int uart;

void uartputc(int c)
{
        int i;

        if (!uart)
                return;
        for (i = 0; i < 128 && !(inb(COM1+5) & 0x20); i++)
                ; /*microdelay(10);*/
        outb(COM1+0, c);
}

void uart_init(void)
{
        char *p;

        /* turn off the FIFO */
        outb(COM1+2, 0);

        /* 
         * Uart init
         * 9600 baud, 8 data bits, 1 stop bit, parity off.
         */
        outb(COM1+3, 0x80);    /* unlock divisor */
        outb(COM1+0, 115200/9600);
        outb(COM1+1, 0);
        outb(COM1+3, 0x03);    /* lock divisor, 8 data bits. */
        outb(COM1+4, 0);
        outb(COM1+1, 0x01);    /* enable receive interrupts. */

        /* if status is 0xFF, no serial port. */
        if (inb(COM1+5) == 0xFF)
                return;
        uart = 1;

        /* simple test. */
        for (p="Jeos ...\n"; *p; p++)
                uartputc(*p);
}
