
include Makefile.include

MAXMAKELEVEL= 0 1 2 3 4 5 6
export MAXMAKELEVEL

exclude_dirs= include tool scripts bin

include $(srctree)/scripts/Makefile.build
