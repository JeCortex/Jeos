/*
 mm.c for the kernel software

 Contact: JeCortex@yahoo.com

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
*/

#include <asm/kernel.h>
#include <linux/mm.h>
#include <asm/x86.h>
#include <linux/os.h>

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

extern uint8 data[];    // defined by kernel.ld
extern uint8 end[];     // defined by kernel.ld

void init_mem_range()
{
        uint32 i;
        memory_layout_t *info = (memory_layout_t *) PA2VA(MEMORY_INFO_ADDR);
        kprintf(WHITE, "the memory info from int 0x15, eax=0xe820:\n");
        kprintf(WHITE, "type\t\taddress\t\tlength\n");
        for (i = 0; i < info->num_of_range; i++) {
                kprintf(RED, "0x%8x\t0x%8x\t0x%8x\n", 
                        info->ranges[i].type,
                        info->ranges[i].base_addr_low,
                        info->ranges[i].base_addr_low + info->ranges[i].length_low);

                /* I am not sure how to get the total memory, and I don't know why when I set the machine with 128MB
                 * memory, this method get a range 1MB ~ 127MB with type 1 (Usable (normal) RAM, 
                 * and 127MB~128MB with type 2(Reserved - unusable)
                 * for now, get the range of address >= 1M and type == 1 (Usable (normal) RAM */
                if (info->ranges[i].type == 1 && info->ranges[i].base_addr_low >= 1*MB) {
                    os.mm.m_usable_phy_mem_start = info->ranges[i].base_addr_low;
                    os.mm.m_usable_phy_mem_end = info->ranges[i].base_addr_low + info->ranges[i].length_low;
                }
        }

        kprintf(WHITE, "usable memory above 1MB: from %uMB, to %dMB\n", 
                os.mm.m_usable_phy_mem_start / (1*MB), os.mm.m_usable_phy_mem_end / (1*MB));

        os.mm.m_mem_start = end;
        os.mm.m_mem_end = (uint8 *)PA2VA(os.mm.m_usable_phy_mem_end);
        kprintf(WHITE, "mem_start: 0x%8x, mem_end: 0x%8x\n", os.mm.m_mem_start, os.mm.m_mem_end);
}

int mm_init(void)
{
        init_mem_range();

        return 0;
}
