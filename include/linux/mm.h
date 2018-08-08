/*
 mm.h for the kernel software

 Contact: JeCortex@yahoo.com

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
*/

#ifndef _mm_t_H_
#define _mm_t_H_

#include <linux/types.h>

#define KB                  (1024)
#define MB                  (1024*KB)
#define GB                  (1024*GB)

#define PAGE_SHIFT          (12)
#define PAGE_SIZE           (1U << PAGE_SHIFT)
#define PAGE_MASK           (~(PAGE_SIZE-1))
#define PAGE_ALIGN(addr)    (((uint32)(addr)+PAGE_SIZE-1) & PAGE_MASK)

#define PGDIR_SHIFT	        (22)
#define PGDIR_SIZE	        (1UL << PGDIR_SHIFT)

#define VA2PA(x)	        (((uint32)(x)) - KERNEL_BASE)
#define PA2VA(x)	        ((void *)((x) + KERNEL_BASE))
#define NR_PDE_PER_PAGE     (PAGE_SIZE / sizeof(pde_t))
#define NR_PTE_PER_PAGE     (PAGE_SIZE / sizeof(pte_t))

#define MAX_ORDER           6
#define MAP_NR(addr)		(((unsigned long)(addr)) >> PAGE_SHIFT)

typedef struct page_s {
        //atomic_t	ref;
        uint32 ref;
} page_t;

typedef struct free_list_s {
        struct free_list_s*	next;
        struct free_list_s*	prev;
        uint32*				map;
} free_list_t;

typedef struct free_area_s {
        free_list_t free_list[MAX_ORDER+1];
        uint8*		base;
} free_area_t;

struct mm {
        pde_t *m_kernel_pg_dir;
        page_t *m_pages;

        uint8 *m_mem_start;
        uint8 *m_mem_end;

        uint32 m_usable_phy_mem_start;
        uint32 m_usable_phy_mem_end;

        free_area_t m_free_area;
};

extern int mm_init(void);
#endif

