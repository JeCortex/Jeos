/*
 screen.c for the kernel software

 Contact: JeCortex@yahoo.com

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
*/

#include <linux/types.h>
#include <linux/mm.h>
#include <asm/kernel.h>
#include <linux/os.h>

uint8 *get_vram(struct screen *sc)
{
        return sc->m_base;
}

void set_pixel(uint32 x, uint32 y, color_ref_t color)
{
        if (x < os.sc.m_width && y < os.sc.m_height) {
                uint8* pvram = os.sc.m_base + os.sc.m_bytes_pp*y*os.sc.m_width + os.sc.m_bytes_pp*x;
                pvram[0] = RGB_GET_B(color);
                pvram[1] = RGB_GET_G(color);
                pvram[2] = RGB_GET_R(color);
        }
}

void draw_asc16(char ch, uint32 left, uint32 top, color_ref_t color)
{
        int32 x, y;
        uint8 test_bit;
        uint8* p_asc = os.sc.m_asc16_addr + ch * ASC16_SIZE;

        for (y = 0; y < ASC16_HEIGHT; y++) {
                test_bit = 1 << 7;
                for (x = 0; x < ASC16_WIDTH; x++) {
                        if (*p_asc & test_bit) {
                                set_pixel(left+x, top+y, color);
                        }
                        test_bit >>= 1;
                }
                p_asc++;
        }
}

void fill_rectangle(rect_t rect, color_ref_t color)
{
        uint32 x, y;
        for (x = 0; x < rect.height; ++x)
        {
                for (y = 0; y < rect.width; ++y) {
                        set_pixel(rect.left + y, rect.top + x, color);
                }
        }
}

int screen_init(struct screen *sc)
{
        video_info_t *info = (video_info_t *) PA2VA(VIDEO_INFO_ADDR);
        sc->m_width    = info->width; 
        sc->m_height   = info->height; 
        sc->m_bytes_pp = info->bits_per_pixel / 8;
        sc->m_base     = (info->vram_base_addr);
        sc->m_asc16_addr = (uint8 *) PA2VA(FONT_ASC16_ADDR);

        return 0;
}
