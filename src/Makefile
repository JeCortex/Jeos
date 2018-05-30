# Makefile for the kernel software
#
# Contact: JeCortex@yahoo.com
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.

AS = as --32
CC = gcc -m32
LD = ld

CFLAGS = -fno-pic -static -fno-builtin -fno-strict-aliasing -Wall -ggdb -m32 -Werror -fno-omit-frame-pointer -nostdinc -fno-stack-protector
LDFLAGS = -m elf_i386 -static

CFLAGS = -g -w -trigraphs -fno-builtin -fno-exceptions -fno-stack-protector -m32 -fno-rtti -nostdlib -nodefaultlibs -Werror -fno-omit-frame-pointer 

all: Jeos

Jeos: boot loader kernel
	../tool/checksize boot 1
	../tool/checksize loader 2
	../tool/checksize kernel 2037
	dd if=boot              of=Jeos_hd.img ibs=512 seek=0   count=1 conv=notrunc
	dd if=loader            of=Jeos_hd.img ibs=512 seek=1   count=2 conv=notrunc
	dd if=kernel            of=Jeos_hd.img ibs=512 seek=3           conv=notrunc

boot: boot.S
	${CC} ${CFLAGS} -c boot.S 
	${LD} ${LDFLAGS} --oformat binary -N -Ttext 0x7c00 -o boot boot.o

loader: load.S loadmain.c
	${CC} ${CFLAGS} -c load.S 
	${CC} ${CFLAGS} -O -c loadmain.c
	${LD} ${LDFLAGS} --oformat binary -N -Ttext 0x0000 -o loader load.o loadmain.o

kernel: entry.o loadmain.o mm.o kernel.ld                                      
	${LD} ${LDFLAGS} -T kernel.ld -o kernel entry.o loadmain.o mm.o

%.o: %.c
	$(CC) $(CPFLAGS) -c $< -o  $@

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o  $@

clean:
	rm -f boot loader kernel *.o
