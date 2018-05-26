# load.S for the kernel software
#
# Copyright(C)  2018 
# Contact: JeCortex@yahoo.com
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.

#include "kernel.h"

.section .text
.global _start

.org 0

_start:
	jmp		main


main:
	movl	$(SEG_KDATA<<3),		%eax
	movw	%ax,					%ds
	movw	%ax,					%es
	movw	%ax,					%fs
	movw	%ax,					%gs
	movw	%ax,					%ss
	movl	$STACK_PM_BOTTOM,		%esp

	call	loadmain

1:
	jmp		1b
