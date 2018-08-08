/*
 checksize.c 

 Copyright(C)  2018 
 Contact: JeCortex@yahoo.com

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SECT_SIZE 512

void hung()
{
    while (1) {
        ;
    }
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        printf("Usage: checksize name max_sector_num\n");
        hung();
    }
    FILE *fp = fopen(argv[1], "r");

    fseek(fp, 0L, SEEK_END);
    long size = ftell(fp);
    long max_size = SECT_SIZE * atol(argv[2]);
    if (size > max_size) {
        printf("%s size %ld is larger than %ld\n", argv[1], size, max_size);
        hung();
    }

    return 0;
}
