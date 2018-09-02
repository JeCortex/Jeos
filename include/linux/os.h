/*
 os.h for the kernel software

 Contact: JeCortex@yahoo.com

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
*/

#ifndef __OS_H__
#define __OS_H__

#include <linux/screen.h>
#include <linux/console.h>
#include <linux/mm.h>
#include <asm/mp.h>


enum pool_type_e {
        VMA_POOL = 0,
        PIPE_POOL,
        TIMER_POOL,
        MAX_POOL,
};

enum device_type_e {
        DEV_CONSOLE = 0,
        MAX_DEV,
};

struct os {
        struct screen  sc;
        struct console cs;
        struct mm mm;
        struct mparch mparch;

        //struct file_system fs;
};

extern struct os os;
extern void panic(const char* s);

#endif
