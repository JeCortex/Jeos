/*
 os.c for the kernel software

 Contact: JeCortex@yahoo.com

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
*/

#include <asm/x86.h>
#include <linux/os.h>
#include <linux/console.h>
#include <linux/types.h>

void panic(const char* s)
{
        cli();
        kprintf(RED, "[BABYOS PANICED], %s\n", s);
        while (1) {
                halt();
        }
}
