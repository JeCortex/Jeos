/*
 console.c for the kernel software

 Contact: JeCortex@yahoo.com

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
*/

#include <linux/console.h>
#include <linux/os.h>
#include <asm/string.h>
#include <asm/spinlock.h>

#include <asm/arg.h>
#define HZ              (100)

void input_buffer_init(struct input_buffer *ib)
{
        ib->m_read_index = 0;
        ib->m_write_index = 0;
        ib->m_edit_index = 0;
        memset(ib->m_buffer, 0, BUFFER_SIZE);
}

void draw_background(struct screen *sc)
{
        rect_t rc = { 0, 0, sc->m_width, sc->m_height};
        fill_rectangle(rc, BACKGROUND_COLOR);
}

void _draw_cursor(struct console *cs)
{
        rect_t rc = { cs->m_col*ASC16_WIDTH, cs->m_row*ASC16_HEIGHT, ASC16_WIDTH, ASC16_HEIGHT };
        fill_rectangle(rc, cs->m_show_cursor ? CURSOR_COLOR : BACKGROUND_COLOR);
}

void draw_cursor()
{
        rect_t rc = { os.cs.m_col*ASC16_WIDTH, os.cs.m_row*ASC16_HEIGHT, ASC16_WIDTH, ASC16_HEIGHT };
        fill_rectangle(rc, os.cs.m_show_cursor ? CURSOR_COLOR : BACKGROUND_COLOR);
}

void scroll()
{
    if (os.cs.m_row < os.cs.m_row_num) {
        return;
    }

    // scroll screen
    scroll();

    // clear last line
    rect_t rc = { 0, (os.cs.m_row-1)*ASC16_HEIGHT, ASC16_WIDTH*os.cs.m_col_num, ASC16_HEIGHT };
    fill_rectangle(rc, BACKGROUND_COLOR);

    os.cs.m_row--;
    os.cs.m_col = 0;
    draw_cursor();
}

void unput_char()
{
    rect_t rc = { os.cs.m_col*ASC16_WIDTH, os.cs.m_row*ASC16_HEIGHT, ASC16_WIDTH, ASC16_HEIGHT };
    fill_rectangle(rc, BACKGROUND_COLOR);
}

void backspace()
{
    unput_char();
    if (os.cs.m_col == 0) {
        if (os.cs.m_row == 0) {
            return;
        }
        else {
            os.cs.m_row--;
            os.cs.m_col = os.cs.m_col_num-1;
        }
    }
    else {
        os.cs.m_col--;
    }
    unput_char();
}

void put_char(char c, color_ref_t color)
{
    rect_t rc = { os.cs.m_col*ASC16_WIDTH, os.cs.m_row*ASC16_HEIGHT, ASC16_WIDTH, ASC16_HEIGHT };
    fill_rectangle(rc, BACKGROUND_COLOR);
    draw_asc16((char) c, os.cs.m_col*ASC16_WIDTH, os.cs.m_row*ASC16_HEIGHT, color);
    os.cs.m_col++;
    if (os.cs.m_col == os.cs.m_col_num) {
        os.cs.m_row++;
        os.cs.m_col = 0;
        scroll();
    }
    draw_cursor();
}

void _putc(int c, color_ref_t color){
        uint32 num;
        rect_t rc;

        switch (c) {
                case '\n':
                        rc.left = os.cs.m_col*ASC16_WIDTH;
                        rc.top = os.cs.m_row*ASC16_HEIGHT;
                        rc.width = ASC16_WIDTH;
                        rc.height = ASC16_HEIGHT;
                        fill_rectangle(rc, BACKGROUND_COLOR);
                        os.cs.m_row++;
                        os.cs.m_col = 0;
                        scroll();
                        break;
                case '\t':
                        num = (4 - os.cs.m_col % 4);
                        while (num--) {
                                put_char(' ', color);
                        }
                        break;
                case '\b':
                        backspace();
                        break;
                default:
                        put_char((char) c, color);
                        break;
        }
        draw_cursor();
}

void kprintf(color_ref_t color, const char *fmt, ...)
{
        int i;
        uint32 flags = 0;
        static char buffer[BUFFER_SIZE] = {0};
        if (fmt == NULL) {
                return;
        }
        spin_lock_irqsave(&os.cs.m_lock, flags);

        memset(buffer, 0, BUFFER_SIZE);
        va_list ap;
        va_start(ap, fmt);
        int total = vsprintf(buffer, fmt, ap);
        va_end(ap);

        for (i = 0; i < total; i++) {
                _putc(buffer[i], color);
        }
        spin_unlock_irqrestore(&os.cs.m_lock, flags);
}

int console_init(struct console *cs, struct screen *sc)
{
        cs->m_row_num = sc->m_height / ASC16_HEIGHT;
        cs->m_col_num = sc->m_width / ASC16_WIDTH;
        cs->m_row = 0;
        cs->m_col = 0;

        cs->m_tick_to_update = HZ;
        cs->m_show_cursor = 1;
        spinlock_init(&cs->m_lock);

        input_buffer_init(&cs->m_input_buffer);

        draw_background(sc);
        //_draw_cursor(cs);
        draw_cursor();

        return 0;
}
