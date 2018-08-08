/*
 fs.h for the kernel software

 Contact: JeCortex@yahoo.com

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
*/

#ifndef __FS_H__
#define __FS_H__

#define ROOT_DEV 1
#define ROOT_INUM 1 

#define BSIZE   SECT_SIZE

#define MAX_PATH 14
#define MAX_INODE_CACHE 64

struct file_system {
        int      *do_read(int fd, void* buffer, uint32 count);
        int      *do_write(int fd, void* buffer, uint32 count);
}

#endif
