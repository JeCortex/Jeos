/*
 main.c for the kernel software

 Contact: JeCortex@yahoo.com

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
*/

#include <linux/mm.h>
#include <linux/screen.h>
#include <linux/console.h>
#include <linux/os.h>
#include <linux/uart.h>
#include <asm/mp.h>

struct os os;

int main(void)
{
        screen_init(&os.sc);
        console_init(&os.cs, &os.sc);
        kprintf(YELLOW, "Welcome to Jeos \n");
        mm_init(&os.mm);
        uart_init();
        mp_init(&os.mparch);
        lapic_init(&os.mparch.apic);
        ioapic_init(&os.mparch.apic);

        while (1);
        return 0;
}
