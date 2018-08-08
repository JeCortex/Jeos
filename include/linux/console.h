/*
 console.h for the kernel software

 Contact: JeCortex@yahoo.com

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
*/

#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include <linux/types.h>
#include <linux/screen.h>
#include <asm/spinlock.h>

#define BUFFER_SIZE     1024

#define CHARACTER(ch)       (ch & 0xff)
#define MAX_ROW             48
#define MAX_COL             128
#define BACKGROUND_COLOR    RGB(0x40, 0, 0x30)
#define CURSOR_COLOR        RGB(0xff, 0xff, 0x00)

struct color_text {
        char ch;
        color_ref_t color;
};

struct input_buffer {
    unsigned    m_read_index;
    unsigned    m_write_index;
    unsigned    m_edit_index;
    char        m_buffer[BUFFER_SIZE];
};

struct console {
        uint32          m_row_num;
        uint32          m_col_num;
        uint32          m_row;
        uint32          m_col;

        struct color_text m_text[MAX_ROW][MAX_COL];
        uint32          m_tick_to_update;
        uint32          m_show_cursor;
        spinlock_t      m_lock;
        struct input_buffer   m_input_buffer;
        //wait_queue_t    m_wait_queue;

};

extern int console_init(struct console *cs, struct screen *sc);
void kprintf(color_ref_t color, const char *fmt, ...);

#endif
