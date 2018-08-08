/*
 kernel.h for the kernel software

 Contact: JeCortex@yahoo.com

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
*/

#ifndef _KERNEL_H_
#define _KENERL_H_

/* segs */
#define SEG_KCODE           (1)
#define SEG_KDATA           (2)

#define	SECT_SIZE			(512)
#define KSTACK_SIZE         (8192)

#define STACK_BOOT			(0x1000)
#define STACK_PM_BOTTOM     (0x10000)

/* boot information */
#define BOOT_INFO_ADDR		(0x8000)    /* 0x8000, 32k */
#define BOOT_INFO_SEG		(BOOT_INFO_ADDR >> 4)

/* video info */
#define VIDEO_INFO_ADDR	    (BOOT_INFO_ADDR)
#define VIDEO_INFO_SIZE	    (12)

/* memory info */
#define MEMORY_INFO_ADDR	(VIDEO_INFO_ADDR + VIDEO_INFO_SIZE)
#define MEMORY_INFO_OFFSET  (MEMORY_INFO_ADDR - BOOT_INFO_ADDR)
#define MEMORY_INFO_SIZE	(4+256)

/* font */
#define FONT_ASC16_ADDR     (0x10000)   /* 64k */

/* struct of boot disk */
#define FONT_ASC16_SECT_NUM (8)
#define KERNEL_ELF_LBA      (3)
#define FONT_ASC16_LBA      (2040)

/* memory */
#define KERNEL_BASE		    0xc0000000  // 3GB

/* page table, page directory entry flag */
#define PTE_P               0x001		// present
#define PTE_W               0x002		// writeable
#define PTE_U               0x004		// user

/* for cr0 */
#define CR0_PE              0x00000001
#define CR0_PG              0x80000000

#endif

