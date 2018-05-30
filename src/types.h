/*
 types.h for the kernel software

 Contact: JeCortex@yahoo.com

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
*/

#ifndef _TYPES_H_
#define _TYPES_H_

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

typedef uint32  pde_t;
typedef uint32  pte_t;

#endif

