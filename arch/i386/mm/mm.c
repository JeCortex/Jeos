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
#include <asm/string.h>
#include <asm/atomic.h>

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

void *boot_mem_alloc(struct mm *mm, uint32 size, uint32 page_align)
{
        if (page_align) {
                mm->m_mem_start = (uint8 *)PAGE_ALIGN(mm->m_mem_start);
        }

        void *p = (void *) mm->m_mem_start;
        mm->m_mem_start += size;

        return p;
}

void boot_map_pages(struct mm *mm, void *va, uint32 pa, uint32 size, uint32 perm)
{
        uint32 i;
        uint8 *v = (uint8 *) (((uint32)va) & PAGE_MASK);
        uint8 *e = (uint8 *) (((uint32)va + size) & PAGE_MASK);
        pa = (pa & PAGE_MASK);

        pde_t *pde = &mm->m_kernel_pg_dir[PD_INDEX(va)];
        pte_t *pg_table;
        while (v < e) {
                if ((*pde) & PTE_P) {
                        pg_table = (pte_t *)(PA2VA((*pde) & PAGE_MASK));
                }
                else {
                        pg_table = (pte_t *)boot_mem_alloc(mm, PAGE_SIZE, 1);
                        memset(pg_table, 0, PAGE_SIZE);
                        *pde = (VA2PA(pg_table) | PTE_P | PTE_W | 0x04);
                }

                pde++;
                for (i = PT_INDEX(v); i < NR_PTE_PER_PAGE && v < e; 
                                i++, v += PAGE_SIZE, pa += PAGE_SIZE) {
                        pte_t *pte = &pg_table[i];
                        if (v < e) {
                                *pte = pa | PTE_P | perm;
                        }
                }
        }
}

void test_page_mapping(struct mm *mm)
{
        uint32 total = 0;
        uint8 *v;
        for (v = (uint8 *)KERNEL_BASE; v < mm->m_mem_end; v += 1*KB) {
                pde_t *pde = &mm->m_kernel_pg_dir[PD_INDEX(v)];

                if ((*pde) & PTE_P) {
                        pte_t *pg_table = (pte_t *)(PA2VA(((*pde) & PAGE_MASK)));
                        pte_t *pte = &pg_table[PT_INDEX(v)];
                        if (!((*pte) & PTE_P)) {
                                kprintf(WHITE, "page fault: v: 0x%p, *pde: \
                                                   0x%p, *pte: 0x%p\n", v, *pde, *pte);
                                break;
                        }
                }
                else {
                        kprintf(WHITE, "page fault2: v: 0x%p, *pde: 0x%p\n", v, *pde);
                        break;
                }
                uint8 x = *v;
                total += x;
        }
}

void init_paging(struct mm *mm)
{
        // mem for m_kernel_pg_dir
        mm->m_kernel_pg_dir = (pde_t *) boot_mem_alloc(mm, PAGE_SIZE, 1);
        memset(mm->m_kernel_pg_dir, 0, PAGE_SIZE);
        kprintf(WHITE, "In init_paging ...\n");

        // first 1MB: KERNEL_BASE ~ KERNEL_LOAD -> 0~1M
        boot_map_pages(mm, (uint8 *)KERNEL_BASE, 0, EXTENED_MEM, PTE_W);

        // kernel text + rodata: KERNEL_LOAD ~ data -> 1M ~ VA2PA(data)
        boot_map_pages(mm, (uint8 *)KERNEL_LOAD, VA2PA(KERNEL_LOAD), VA2PA(data) - VA2PA(KERNEL_LOAD), 0);

        // kernel data + memory: data ~ KERNEL_BASE+MAX_PHY_MEM -> VA2PA(data) ~ MAX_PHY_MEM
        boot_map_pages(mm, data, VA2PA(data), VA2PA(mm->m_mem_end) - VA2PA(data), PTE_W);

        // map the video vram mem
        uint32 screen_vram = (uint32)get_vram(&os.sc);
        mm->m_kernel_pg_dir[((uint32)screen_vram)>>22] = ((uint32)(VA2PA(entry_pg_table_vram)) | (PTE_P | PTE_W));

        // map apic base
        //pte_t* pg_table_apic = (pte_t *) boot_mem_alloc(mm, PAGE_SIZE, 1);
        //pg_table_apic[PT_INDEX(APIC_BASE)] = APIC_BASE | (PTE_P | PTE_W);
        //pg_table_apic[PT_INDEX(IO_APIC_BASE)] = IO_APIC_BASE | (PTE_P | PTE_W);
        //mm->m_kernel_pg_dir[PD_INDEX(APIC_BASE)] = ((uint32)(VA2PA(pg_table_apic)) | (PTE_P | PTE_W));

        set_cr3(VA2PA(mm->m_kernel_pg_dir));
        kprintf(WHITE, "Out ...\n");

        // FIXME: debug
        test_page_mapping(mm);
}

void init_mem_range(struct mm *mm)
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

                /* I am not sure how to get the total memory, and I don't know why 
                 * when I set the machine with 128MB memory, this method get a range 
                 * 1MB ~ 127MB with type 1 (Usable (normal) RAM, and 127MB~128MB with 
                 * type 2(Reserved - unusable) for now, get the range of address >= 1M 
                 * and type == 1 (Usable (normal) RAM 
                 */
                if (info->ranges[i].type == 1 && info->ranges[i].base_addr_low >= 1*MB) {
                        mm->m_usable_phy_mem_start = info->ranges[i].base_addr_low;
                        mm->m_usable_phy_mem_end = info->ranges[i].base_addr_low + \
                                                info->ranges[i].length_low;
                }
        }

        kprintf(WHITE, "usable memory above 1MB: from %uMB, to %dMB\n", 
                mm->m_usable_phy_mem_start / (1*MB), mm->m_usable_phy_mem_end / (1*MB));

        mm->m_mem_start = end;
        mm->m_mem_end = (uint8 *)PA2VA(mm->m_usable_phy_mem_end);
        kprintf(WHITE, "mem_start: 0x%8x, mem_end: 0x%8x\n", mm->m_mem_start, mm->m_mem_end);
}

void init_pages(struct mm *mm)
{
        uint32 i;
        uint32 page_num = (uint32) (mm->m_mem_end - KERNEL_BASE) / PAGE_SIZE;
        uint32 size = page_num * sizeof(page_t);
        mm->m_pages = (page_t *) boot_mem_alloc(mm, size, 0);

        for (i = 0; i < page_num; i++) {
                atomic_set(&mm->m_pages[i].ref, 1);
        }
}

uint32 dec_page_ref(struct mm *mm, uint32 phy_addr)
{
        page_t* page = &mm->m_pages[phy_addr >> PAGE_SHIFT];
        if (page->ref.counter <= 0) {
                panic("ref count <= 0 when dec ref");

        }
        return atomic_dec_and_test(&page->ref);

}

uint32 pow(int32 x, int32 p)
{
        uint32 i;
        uint32 ret = 1;
        for (i = 0; i < p; i++) {
                ret *= x;
        }
        return ret;

}

uint32 get_buddy(struct mm *mm, uint32 addr, uint32 mask)
{
        uint32 buddy = ((addr - (uint32)mm->m_free_area.base) ^ (-mask)) + (uint32)mm->m_free_area.base;
        return buddy;

}

static inline void add_to_head(free_list_t* head, free_list_t * entry)
{
        entry->prev = head;
        entry->next = head->next;
        head->next->prev = entry;
        head->next = entry;
}

static inline void remove_head(free_list_t* head, free_list_t * entry)
{
        entry->next->prev = entry->prev;
        entry->prev->next = entry->next;
}

void free_pages(struct mm *mm, void* addr, uint32 order)
{
        //dec the ref count, if it's not 0, don't free the pages
        if (!dec_page_ref(mm, VA2PA(addr))) {
                return;
        }
        atomic_add(pow(2, order), &mm->m_free_page_num);

        uint32 address = (uint32) addr;
        uint32 index = MAP_NR(address - (uint32)mm->m_free_area.base) >> (1 + order);
        uint32 mask = PAGE_MASK << order;

        address &= mask;
        while (order < MAX_ORDER) {
                if (!change_bit(index, mm->m_free_area.free_list[order].map)) {
                        break;
                }

                uint32 buddy = get_buddy(mm, address, mask);
                remove_head(mm->m_free_area.free_list+order, (free_list_t *)buddy);
                order++;
                index >>= 1;
                mask <<= 1;
                address &= mask;
        }
        add_to_head(mm->m_free_area.free_list+order, (free_list_t *) address);
}

void free_boot_mem(struct mm *mm)
{
        uint8 *p;
        atomic_set(&mm->m_free_page_num, 0);
        for (p = mm->m_free_area.base; p < mm->m_mem_end; p += PAGE_SIZE) {
                free_pages(mm, p, 0);
        }
}

void init_free_area(struct mm *mm)
{
        int i;
        uint32 mask = PAGE_MASK;
        uint32 bitmap_size;
        for (i = 0; i <= MAX_ORDER; i++) {
                mm->m_free_area.free_list[i].prev = &mm->m_free_area.free_list[i];
                mm->m_free_area.free_list[i].next = &mm->m_free_area.free_list[i];
                mask += mask;
                mm->m_mem_end = (uint8 *)(((uint32)(mm->m_mem_end)) & mask);
                bitmap_size = ((uint32)(mm->m_mem_end - mm->m_mem_start)) >> (PAGE_SHIFT + i);
                bitmap_size = (bitmap_size + 7) >> 3;
                bitmap_size = (bitmap_size + sizeof(uint32) - 1) & ~(sizeof(uint32)-1);
                mm->m_free_area.free_list[i].map = (uint32 *) mm->m_mem_start;
                memset((void *) mm->m_mem_start, 0, bitmap_size);
                mm->m_mem_start += bitmap_size;
        }

        init_pages(mm);

        mm->m_free_area.base = (uint8*)(((uint32)mm->m_mem_start + ~mask) & mask);
        free_boot_mem(mm);
}

int mm_init(struct mm *mm)
{
        init_mem_range(mm);
        init_paging(mm);
        init_free_area(mm);
        return 0;
}
