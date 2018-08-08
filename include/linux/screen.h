/*
 screen.h for the kernel software

 Contact: JeCortex@yahoo.com

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
*/

#ifndef _SCREEN_H_
#define _SCREEN_H_

#include <linux/types.h>
#include <linux/color.h>

#define ASC16_SIZE      16
#define ASC16_WIDTH     8
#define ASC16_HEIGHT    16

struct screen {
	uint8*	m_base;        // base address
	uint8*	m_asc16_addr;
	uint16	m_width;
	uint16	m_height;
	uint8	m_bytes_pp;    // bytes per pixel
};

extern int screen_init(struct screen *sc);
extern void draw_asc16(char ch, uint32 left, uint32 top, color_ref_t color);
extern void fill_rectangle(rect_t rect, color_ref_t color);

#endif
