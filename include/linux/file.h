/*
 file.h for the kernel software

 Contact: JeCortex@yahoo.com

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
*/

#ifndef __FILE_H__
#define __FILE_H__

#include <linux/fs.h>

struct file {
        enum { FD_NONE, FD_PIPE, FD_INODE } type;
        int ref; /* reference count */
        char readable;
        char writable;
        struct pipe *pipe;
        struct inode *ip;
        uint32 off;
};


/* in-memory copy of an inode */
struct inode {
        uint32 dev;           /* device number */
        uint32 inum;          /* inode number */
        int ref;            /* reference count */
        //struct sleeplock lock; /* protects everything below here */
        int valid;          /* inode has been read from disk? */

        short type;         /* copy of disk inode */
        short major;
        short minor;
        short nlink;
        uint32 size;
        uint32 addrs[NDIRECT+1];
};

/* 
* Table mapping major device number to
* device functions 
*/
struct devsw {
        int (*read)(struct inode*, char*, int);
        int (*write)(struct inode*, char*, int);
};

extern struct devsw devsw[];

#define CONSOLE 1

#endif
