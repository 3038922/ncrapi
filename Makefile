# @Author: 陈昱安
# @Date:   2018-09-16T00:20:57+08:00
# @Email:  31612534@qq.com
# @Last modified by:   陈昱安
# @Last modified time: 2018-09-16T20:56:22+08:00



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

WARNFLAGS+=-Wno-psabi
EXTRA_CFLAGS=
EXTRA_CXXFLAGS=

# Set this to 1 to add additional rules to compile your project as a PROS library template
IS_LIBRARY:=0
# TODO: CHANGE THIS!
LIBNAME:=libbest
VERSION:=1.0.0
# EXCLUDE_SRC_FROM_LIB= $(SRCDIR)/unpublishedfile.c
# this line excludes opcontrol.c and similar files
EXCLUDE_SRC_FROM_LIB+=$(foreach file, $(SRCDIR)/opcontrol $(SRCDIR)/initialize $(SRCDIR)/autonomous,$(foreach cext,$(CEXTS),$(file).$(cext)) $(foreach cxxext,$(CXXEXTS),$(file).$(cxxext)))

# files that get distributed to every user (beyond your source archive) - add
# whatever files you want here. This line is configured to add all header files
# that are in the the include directory get exported
TEMPLATE_FILES=$(INCDIR)/**/*.h $(INCDIR)/**/*.hpp

.DEFAULT_GOAL=quick

################################################################################
################################################################################
########## Nothing below this line should be edited by typical users ###########
-include ./common.mk

INCLUDE=-iquote$(INCDIR)

ASMSRC=$(foreach asmext,$(ASMEXTS),$(call rwildcard, $(SRCDIR),*.$(asmext), $1))
ASMOBJ=$(addprefix $(BINDIR)/,$(patsubst $(SRCDIR)/%,%.o,$(call ASMSRC,$1)))
CSRC=$(foreach cext,$(CEXTS),$(call rwildcard, $(SRCDIR),*.$(cext), $1))
COBJ=$(addprefix $(BINDIR)/,$(patsubst $(SRCDIR)/%,%.o,$(call CSRC, $1)))
CXXSRC=$(foreach cxxext,$(CXXEXTS),$(call rwildcard, $(SRCDIR),*.$(cxxext), $1))
CXXOBJ=$(addprefix $(BINDIR)/,$(patsubst $(SRCDIR)/%,%.o,$(call CXXSRC,$1)))

GETALLOBJ=$(sort $(call ASMOBJ,$1) $(call COBJ,$1) $(call CXXOBJ,$1))

LIBRARIES=-L$(FWDIR) -Wl,--start-group $(wildcard $(FWDIR)/*.a) -lc -lm -lgcc -lstdc++ -lsupc++ -Wl,--end-group
ARCHIVE_TEXT_LIST:=$(subst $(SPACE),$(COMMA),$(notdir $(basename $(wildcard $(FWDIR)/*.a))))

ifndef OUTBIN
OUTNAME:=output
endif
OUTBIN:=$(BINDIR)/$(OUTNAME).bin
OUTELF:=$(BINDIR)/$(OUTNAME).elf
LDTIMEOBJ:=$(BINDIR)/_pros_ld_timestamp.o

.PHONY: all clean quick

quick: $(OUTBIN)

all: clean $(OUTBIN)

clean:
	@echo Cleaning project
	-$Drm -rf $(BINDIR)

$(OUTBIN): $(OUTELF) $(BINDIR)
	@echo -n "Creating $@ for $(DEVICE) "
	$(call test_output,$D$(OBJCOPY) $< -O binary $@,$(DONE_STRING))

$(OUTELF): $(call GETALLOBJ,$(EXCLUDE_SRCDIRS))
	$(call _pros_ld_timestamp)
	@echo -n "Linking project with $(ARCHIVE_TEXT_LIST) "
	$(call test_output,$D$(LD) $(LDFLAGS) $^ $(LDTIMEOBJ) $(LIBRARIES) -o $@,$(OK_STRING))
	@echo Section sizes:
	-$(VV)$(SIZETOOL) $(SIZEFLAGS) $@ $(SIZES_SED) $(SIZES_NUMFMT)

define asm_rule
$(BINDIR)/%.$1.o: $(SRCDIR)/%.$1
	$(VV)mkdir -p $$(dir $$@)
	@echo -n "Compiling $$< "
	$$(call test_output,$D$(AS) -c $(ASMFLAGS) -o $$@ $$<,$(OK_STRING))
endef
$(foreach asmext,$(ASMEXTS),$(eval $(call asm_rule,$(asmext))))

define c_rule
$(BINDIR)/%.$1.o: $(SRCDIR)/%.$1
	$(VV)mkdir -p $$(dir $$@)
	@echo -n "Compiling $$< "
	$$(call test_output,$D$(CC) -c $(INCLUDE) -iquote$(INCDIR)/$$(dir $$*) $(CFLAGS) $(EXTRA_CFLAGS) -o $$@ $$<,$(OK_STRING))
endef
$(foreach cext,$(CEXTS),$(eval $(call c_rule,$(cext))))

define cxx_rule
$(BINDIR)/%.$1.o: $(SRCDIR)/%.$1
	$(VV)mkdir -p $$(dir $$@)
	@echo -n "Compiling $$< "
	$$(call test_output,$D$(CXX) -c $(INCLUDE) -iquote$(INCDIR)/$$(dir $$*) $(CXXFLAGS) $(EXTRA_CXXFLAGS) -o $$@ $$<,$(OK_STRING))
endef
$(foreach cxxext,$(CXXEXTS),$(eval $(call cxx_rule,$(cxxext))))

define _pros_ld_timestamp
@echo -n "Adding timestamp "
@# Pipe a line of code defining _PROS_COMPILE_TOOLSTAMP and _PROS_COMPILE_DIRECTORY into GCC,
@# which allows compilation from stdin. We define _PROS_COMPILE_DIRECTORY using a command line-defined macro
@# which is the pwd | tail bit, which will truncate the path to the last 23 characters
$(call test_output, $(VV)echo 'char const * const _PROS_COMPILE_TIMESTAMP = __DATE__ " " __TIME__; char const * const _PROS_COMPILE_DIRECTORY = PCD;' | $(CC) -c -x c $(CFLAGS) $(EXTRA_CFLAGS) -DPCD="\"`pwd | tail -c 23`\"" -o $(LDTIMEOBJ) -,$(OK_STRING))
endef


ifeq ($(IS_LIBRARY),1)
ifeq ($(LIBNAME),libbest)
$(errror "You should rename your library! libbest is the default library name and should be changed")
endif

LIBAR=$(BINDIR)/$(LIBNAME).a
TEMPLATE_DIR=$(ROOT)/template

clean-template:
	@echo Cleaning $(TEMPLATE_DIR)
	-$Drm -rf $(TEMPLATE_DIR)

$(LIBAR): $(call GETALLOBJ,$(EXCLUDE_SRC_FROM_LIB))
	@echo -n "Creating $@ "
	$(call test_output,$D$(AR) rcs $@ $^, $(DONE_STRING))

.PHONY: library
library: $(LIBAR)

.PHONY: template
template: clean-template $(LIBAR)
	$Dprosv5 c create-template . $(LIBNAME) $(VERSION) $(foreach file,$(TEMPLATE_FILES) $(LIBAR),--system "$(file)") --target v5
endif
