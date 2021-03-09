################################################################################
######################### User configurable parameters #########################
# filename extensions
CEXTS:=c
ASMEXTS:=s S
CXXEXTS:=cpp c++ cc

# probably shouldn't modify these, but you may need them below
ROOT=.
FWDIR:=$(ROOT)/firmware
BINDIR=$(ROOT)/bin
SRCDIR=$(ROOT)/src
INCDIR=$(ROOT)/include

WARNFLAGS+=-Wno-format# 
EXTRA_CFLAGS=-Wall -Wextra
EXTRA_CXXFLAGS=-Wall -Wextra  -Wno-unused-function -Wno-unused-parameter -Wno-unused-but-set-variable -Wno-unused-variable  -Wno-implicit-fallthrough  -Werror=return-type -Wno-reorder -Wno-comment #-Wmissing-include-dirs -Wconversion -pedantic
# Set to 1 to enable hot/cold linking
USE_PACKAGE:=0

# Set this to 1 to add additional rules to compile your project as a PROS library template
IS_LIBRARY:=1
LIBNAME:=ncrapi
VERSION:=10.6.7
EXCLUDE_SRC_FROM_LIB=$(call rwildcard,$(SRCDIR)/skillAuto,$(SRCDIR)/userAuto,*.*)
# this line excludes opcontrol.c and similar files
EXCLUDE_SRC_FROM_LIB+= $(foreach file, $(SRCDIR)/customTest $(SRCDIR)/userauto $(SRCDIR)/initialize $(SRCDIR)/autonomous $(SRCDIR)/main,$(foreach cext,$(CEXTS),$(file).$(cext)) $(foreach cxxext,$(CXXEXTS),$(file).$(cxxext)))

# files that get distributed to every user (beyond your source archive) - add
# whatever files you want here. This line is configured to add all header files
# that are in the the include directory get exported
TEMPLATE_FILES=$(INCDIR)/ncrapi/**/*.h $(INCDIR)/ncrapi/**/*.hpp

.DEFAULT_GOAL=quick

################################################################################
################################################################################
########## Nothing below this line should be edited by typical users ###########
-include ./common.mk
