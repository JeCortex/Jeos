/*
 loadmain.c for the kernel software

 Copyright(C)  2018 
 Contact: JeCortex@yahoo.com

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
*/
#include "kernel.h"

#define NULL (0)

typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned int        uint32;
typedef int                 int32;

typedef struct vidoe_info_s {
        uint16 video_mode;
        uint16 cx_screen;
        uint16 cy_screen;
        uint8  n_bits_per_pixel;
        uint8  n_memory_model;
        uint8* p_vram_base_addr;

} video_info_t;

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
        for (i = 100; i < 1024-100; i++) {
                set_pixel(i, 200, 0xff, 34, 89);

        }

}

void loadmain(void)
{
        p_vram_base_addr = p_video_info->p_vram_base_addr;
        test();

}
