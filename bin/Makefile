# Makefile for the kernel software
#
# Contact: JeCortex@yahoo.com
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.

include ../Makefile.include

OBJS += mm.o \
	main.o \
	screen.o \
	console.o \
	string.o \
	spinlock.o

all: Jeos
Jeos: boot loader kernel
	#../tools/checksize boot 1
	#../tools/checksize loader 2
	#../tools/checksize kernel 2037
	dd if=boot              of=Jeos_hd.img ibs=512 seek=0   count=1 conv=notrunc
	dd if=loader            of=Jeos_hd.img ibs=512 seek=1   count=2 conv=notrunc
	dd if=kernel            of=Jeos_hd.img ibs=512 seek=3           conv=notrunc

kernel: $(OBJS) entry.o kernel.ld                                      
	${LD} ${LDFLAGS} -T kernel.ld -o kernel entry.o $(OBJS)

#TORM = $(shell find $(srctree) -name *.o) 
clean:
	rm -rf Jeos_hd.img boot loader kernel *.o
