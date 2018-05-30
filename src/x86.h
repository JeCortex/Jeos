/*
 x86.h for the kernel software

 Contact: JeCortex@yahoo.com

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
*/

#ifndef _X86_H_
#define _X86_H_

static inline uint8 inb(uint16 port)
{
    uint16 data;
    __asm__ volatile("inb %1,%0" : "=a" (data) : "d" (port));
    return data;
}

static inline uint8 inw(uint16 port)
{
    uint16 data;
    __asm__ volatile("inw %1,%0" : "=a" (data) : "d" (port));
    return data;
}

static inline void insl(int port, void *addr, int cnt)
{
    __asm__ volatile("cld; rep insl" :
            "=D" (addr), "=c" (cnt) :
            "d" (port), "0" (addr), "1" (cnt) :
            "memory", "cc");
}

static inline void outsl(int port, const void *addr, int cnt)
{
    __asm__ volatile("cld; rep outsl" :
            "=S" (addr), "=c" (cnt) :
            "d" (port), "0" (addr), "1" (cnt) :
            "cc");
}

static inline void outb(uint16 port, uint8 data)
{
    __asm__ volatile("outb %0,%1" : : "a" (data), "d" (port));
}

static inline void outw(uint16 port, uint16 data)
{
    __asm__ volatile("outw %0,%1" : : "a" (data), "d" (port));
}

static inline void stosb(void *addr, int32 data, int32 cnt)
{
    __asm__ volatile("cld; rep stosb" :
            "=D" (addr), "=c" (cnt) :
            "0" (addr), "1" (cnt), "a" (data) :    /* di=addr, cx=cnt, ax=data */
            "memory", "cc");
}

static inline void movsb(void *dst, void *src, int32 cnt)
{
    __asm__ volatile("cld; rep movsb" :
            :
            "D" (dst), "S" (src), "c" (cnt) :      /* di=dst, si=src, cx=cnt */
            "memory", "cc");
}

#endif
