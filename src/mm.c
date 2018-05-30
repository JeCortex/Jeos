/*
 mm.c for the kernel software

 Contact: JeCortex@yahoo.com

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
*/

#include "kernel.h"
#include "mm.h"
#include "x86.h"

__attribute__ ((__aligned__(KSTACK_SIZE)))
uint8 kernel_stack[8*KSTACK_SIZE] = {
    0xff,
};

/* pg_dir and pte for entry */
__attribute__ ((__aligned__(PAGE_SIZE)))
pte_t entry_pg_table0[NR_PTE_PER_PAGE] = { 
    [0] = (0) | PTE_P | PTE_W,
};
__attribute__ ((__aligned__(PAGE_SIZE)))
pte_t entry_pg_table_vram[NR_PTE_PER_PAGE] = {
    [0] = (0) | PTE_P | PTE_W,
};

__attribute__ ((__aligned__(PAGE_SIZE)))
pde_t entry_pg_dir[NR_PDE_PER_PAGE] = { 
    [0] = (0) | PTE_P | PTE_W,
};
