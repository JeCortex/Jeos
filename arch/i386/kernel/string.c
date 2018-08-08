/*
 string.c for the kernel software

 Contact: JeCortex@yahoo.com

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
*/

#include <linux/types.h>
#include <asm/string.h>
#include <linux/console.h>

void* memmov(void *dst, const void *src, uint32 n)
{
        const char *s = (const char *) src;
        char *d = (char *) dst;

        if (s < d && s + n > d) {
                s += n, d += n;
                while (n--) {
                        *--d = *--s;
                }
        }
        else {
                while (n--) {
                        *d++ = *s++;
                }
        }

        return dst;
}

void* memcpy(void *dst, const void *src, uint32 n)
{
        return memmov(dst, src, n);
}

void* memset(void *dst, uint32 c, uint32 n)
{
        uint32 i;
        char* d = (char *) dst;
        for (i = 0; i < n; i++) {
                *d++ = (c & 0xff);
        }

        return dst;
}

int memcmp(const void* b1, const void* b2, uint32 n)
{
        if (n == 0) {
                return 0;
        }

        while (--n && *((char *)b1) == *((char *)b2)) {
                b1 = (char *) b1 + 1;
                b2 = (char *) b2 + 1;
        }

        return (*((char *)b1) - *((char *)b2));
}

char* strcpy(char* dst, const char* src)
{
        char* d = dst;
        while (*src) {
                *d++ = *src++;
        }
        *d++ = '\0';

        return dst;
}

char* strncpy(char* dst, const char* src, int n)
{
        char* d = dst;
        while (--n >= 0 && *src) {
                *d++ = *src++;
        }
        *d++ = '\0';

        return dst;
}

int strcmp(const char* s1, const char *s2)
{
        while (*s1 && *s2 && *s1 == *s2) {
                s1++;
                s2++;
        }

        return *s1 - *s2;
}

int strlen(const char* s)
{
        int len = 0;
        while (*s++) {
                len++;
        }
        return len;
}

int strncmp(const char* s1, const char *s2, int n)
{
        while (*s1 && *s2 && *s1 == *s2 && --n >= 0) {
                s1++;
                s2++;
        }

        return n == 0 ? 0 : *s1 - *s2;
}

char* strcat(char* dst, const char* src)
{
        char* ret = dst;
        while (*dst) {
                dst++;
        }
        while (*src) {
                *dst++ = *src++;
        }
        return ret;
}

int is_digit(char c)
{
        return (c >= '0' && c <= '9');
}

int sprint_str(char* buffer, char* s, int width)
{
        int i, len;
    if (s == NULL) {
        return 0;
    }

    len = strlen(s);
    if (width < len) {
        width = len;
    }

    strcat(buffer, s);
    buffer += len;
    for (i = 0; i < width - len; i++) {
        *buffer++ = ' ';
    }
    return width;
}

int sprint_int(char* buffer, int n, int width, int base, int sign)
{
        int i=0, j;
        if (base <= 0) {
                return -1;
        }

        const static char digits[] = "0123456789abcdef";
        char buf[16] = {0};

        uint32 num = (uint32)n;
        if (sign && (sign = (n < 0))) {
                num = -n;
        }

        do {
                buf[i++] = digits[num % base];
                num /= base;
        } while (num != 0);

        if (sign) {
                buf[i++] = '-';
        }

        if (width < i) {
                width = i;
        }

        for (j = 0; j < width - i; j++) {
                *buffer++ = base == 16 ? '0' : ' ';
        }
        while (--i >= 0) {
                *buffer++ = buf[i];
        }

        return width;
}

int vsprintf(char *buffer, const char *fmt, va_list ap)
{
        int i;
        //static char str_null[] = "NULL";

        buffer[0] = '\0';
        if (fmt == NULL) {
                return 0;
        }

        int total = 0;
        char c;
        int width = 0;
        //char* s = NULL;
        for (i = 0; (c = CHARACTER(fmt[i])) != 0; i++) {
                if (c != '%') {
                        buffer[total++] = c;
                        continue;
                }

                c = CHARACTER(fmt[++i]);
                if (c == '\0') {
                        break;
                }

                width = 0;
                while (c != '\0' && is_digit(c)) {
                        width = width * 10 + c - '0';
                        c = CHARACTER(fmt[++i]);
                }

                if (c == '\0') {
                        break;
                }

                switch (c) {
                        case 'd':
                        total += sprint_int(buffer + total, va_arg(ap, int32), width, 10, 1);
                        break;
                        case 'u':
                        total += sprint_int(buffer + total, va_arg(ap, int32), width, 10, 0);
                        break;
                        case 'x':
                        case 'p':
                        total += sprint_int(buffer + total, va_arg(ap, int32), width, 16, 0);
                        break;
                        case 'c':
                        buffer[total++] = (char) CHARACTER(va_arg(ap, int32));
                        break;
                        case 's':
                        total += sprint_str(buffer + total, va_arg(ap, char *), width);
                        break;
                        case '%':
                        buffer[total++] = '%';
                        break;
                        default:
                        buffer[total++] = '%';
                        buffer[total++] = c;
                        break;
                }
        }
return total;
}

int sprintf(char* buffer, const char *fmt, ...)
{
        va_list ap;
        va_start(ap, fmt);
        int total = vsprintf(buffer, fmt, ap);
        va_end(ap);

        return total;
}
