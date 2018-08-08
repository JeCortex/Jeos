
d=../
D=$(wordlist 1, $(MAKELEVEL), $(MAXMAKELEVEL))
R := $(foreach i,$(D),$(d))
R1 := ./$(R)
TOPDIR=$(shell echo $(R1) | sed s/[[:space:]]//g)

exclude_dirs = include

include $(TOPDIR)scripts/Makefile.build
