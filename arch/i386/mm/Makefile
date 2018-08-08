
d=../
D=$(wordlist 1, $(MAKELEVEL), $(MAXMAKELEVEL))
R := $(foreach i,$(D),$(d))
R1 := ./$(R)
TOPDIR=$(shell echo $(R1) | sed s/[[:space:]]//g)

include $(TOPDIR)Makefile.include
TARGET=Mm
EXEPATH=$(TOPDIR)bin

include $(TOPDIR)scripts/Makefile.build
