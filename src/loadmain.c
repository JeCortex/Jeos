/*
 loadmain.c for the kernel software

 Contact: JeCortex@yahoo.com

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
*/

#include "types.h"
#include "kernel.h"
#include "elf.h"
#include "x86.h"

#define HD_STATE_READY  0x40
#define HD_STATE_BUSY   0x80
#define IO_CMD_READ     0x20


typedef void (*kernel_entry_t)(void);

video_info_t* p_video_info = (video_info_t *)VIDEO_INFO_ADDR;

uint8* p_vram_base_addr = (uint8 *)0xe0000000;
uint32 cx_screen = 1024;
uint32 cy_screen = 768;
uint32 n_bytes_per_pixel = 3;

static int is_pixel_valid(int32 x, int32 y)
{
        if (x < 0 || y < 0 || (uint32)x >= cx_screen || (uint32)y >= cy_screen) {
                return 0;

        }
        return 1;
}

int set_pixel(int32 x, int32 y, uint8 r, uint8 g, uint8 b)
{
        uint8* pvram = NULL;
        if (!is_pixel_valid(x, y)) {
                return 0;
        }

        pvram = p_vram_base_addr + n_bytes_per_pixel*y*cx_screen + n_bytes_per_pixel*x;
        pvram[0] = b;
        pvram[1] = g;
        pvram[2] = r;

        return 1;
}

void test()
{
        int i;
        p_vram_base_addr = p_video_info->p_vram_base_addr;
        for (i = 100; i < 1024-100; i++) {
                set_pixel(i, 200, 0xff, 34, 89);
        }
}

void wait_disk()
{
        while ((inb(0x1f7) & (HD_STATE_BUSY | HD_STATE_READY)) != HD_STATE_READY) {
                ;
        }
}

void read_sector(void* buf, uint32 lba)
{
        wait_disk();

        outb(0x1f2, 1);                     /* sector num */
        outb(0x1f3, lba & 0xff);
        outb(0x1f4, (lba >> 8)  & 0xff);
        outb(0x1f5, (lba >> 16) & 0xff);
        outb(0x1f6, 0xe0 | ((lba >> 24) & 0xff));
        outb(0x1f7, IO_CMD_READ);

        wait_disk();
        insl(0x1f0, buf, SECT_SIZE / 4);
}

/* pa: the buffer to read data, will be aligned by SECT_SIZE
* offset: where to read from disk (byte)
* size: how many byte to read
*/
void read_segment(void* pa, uint32 offset, uint32 size)
{
        uint8* p = (uint8 *) pa - (offset % SECT_SIZE);;
        uint32 lba = offset / SECT_SIZE;
        uint8* end = p + size;

        for (; p < end; p += SECT_SIZE, lba++) {
                read_sector(p, lba);
        }
}

void loadmain()
{
        int i;
        char buf[512] = {0};
        //p_vram_base_addr = p_video_info->p_vram_base_addr;

        /*for test*/
        //test();

        elf_hdr_t* elf = (elf_hdr_t *) buf;
        read_sector(elf, KERNEL_ELF_LBA);
        if (elf->magic != ELF_MAGIC) {
                //test();
                return;
        }
        //test();

        /* read segments */
        uint32 elf_offset = SECT_SIZE * KERNEL_ELF_LBA;
        prog_hdr_t* ph = (prog_hdr_t *) ((uint8 *)elf + elf->phoff);
        for (i = 0; i < elf->phnum; i++, ph++) {
                read_segment((void *) ph->paddr, elf_offset + ph->off, ph->filesz);
                if (ph->memsz > ph->filesz) {
                        stosb((void *) ph->paddr + ph->filesz, 0, ph->memsz - ph->filesz);
                }
        }

        /* load font */
        uint8* font_addr = (uint8 *) FONT_ASC16_ADDR;
        uint32 font_lba = FONT_ASC16_LBA;
        for (i = 0; i < FONT_ASC16_SECT_NUM; i++, font_addr += SECT_SIZE, font_lba++) {
                read_sector(font_addr, font_lba);
        }

        /* find entry from elf, and call */
        kernel_entry_t entry = (kernel_entry_t) elf->entry;


        //test();
        entry();
        //test();
}
