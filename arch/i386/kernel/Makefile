# Makefile for the kernel software
#
# Contact: JeCortex@yahoo.com
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.

d=../
D=$(wordlist 1, $(MAKELEVEL), $(MAXMAKELEVEL))
R := $(foreach i,$(D),$(d))
R1 := ./$(R)
TOPDIR=$(shell echo $(R1) | sed s/[[:space:]]//g)

include $(TOPDIR)Makefile.include
TARGET=Kernel
EXEPATH=$(TOPDIR)bin

include $(TOPDIR)scripts/Makefile.build
