#----------------------------------------------------------------------
#  @file   Rules.make
#  @brief  common rules makefile include for tboot build
#  $Id: Rules.make,v 1.35 2015/01/13 09:46:29 cnyu Exp $
#  $Author: cnyu $
#  $Revision: 1.35 $
#
#  Copyright (c) 2006 Terawins Inc. All rights reserved.
# 
#  @date   2007/02/26   jedy  New file.
#----------------------------------------------------------------------
NEZHA_VER=$$(cat $(TOPDIR)/.version)

DID=0x0311
PID=0xDF11
VID=0x0483

PLATFORM=T300

PWD=$(shell pwd)
SUBDIR=$(patsubst $(TOPDIR)%,%, $(PWD))
DIRNAME=$(shell basename $(PWD))

ALL_C=$(shell ls *.c)

SHELLTINYLIB=$(TOPDIR)/shell/shell_tiny.lo
SHELLALLLIB=$(TOPDIR)/shell/shell.lo

ifndef SHELLLIB
SHELLLIB=$(SHELLTINYLIB)
endif
# SHELLLIB=$(SHELLALLLIB)
SYSLIB=$(SHELLLIB)
ifeq ($(TOPDIR)/net, $(wildcard $(TOPDIR)/net))
SYSLIB+=$(TOPDIR)/net/net.la
endif
SYSLIB+=$(TOPDIR)/sys/sys.la $(TOPDIR)/drivers/drivers.la $(TOPDIR)/codec_eng/codec_eng.la $(TOPDIR)/fs/fs.la $(TOPDIR)/libs/libs.la

export PLATFORM

# Makefile variables

ifeq ($(CONFIG_DEBUG),y)
DEBUG_OPT=-O
else
DEBUG_OPT=-O
#CFLAGS += -DNDEBUG
endif

ifeq ($(CONFIG_RINGTONG_NUM),)
CONFIG_RINGTONG_NUM=3
endif

ifeq ($(CONFIG_RINGTONG_UNITSIZE),)
CONFIG_RINGTONG_UNITSIZE=65536
endif

CPU     =4kec
ISA     =-mips16
FPU     =
ENDIAN  =-EL
OPTIONS =
#OPTIONS =-finstrument-functions

CROSS   = sde-
#CROSS   = mips-sde-elf-
OFORMAT = --oformat=elf32-tradlittlemips
CC      = $(CROSS)gcc
LD      = $(CROSS)ld
NM      = $(CROSS)nm
OBJCOPY = $(CROSS)objcopy
OBJDUMP = $(CROSS)objdump
AR      = $(CROSS)ar
STRIP   = $(CROSS)strip
#MAKE    = $(CROSS)make SDB=MSIM16L
ACFLAGS  += -I. -I$(TOPDIR) -I$(TOPDIR)/include -I$(TOPDIR)/sys/include -I$(TOPDIR)/drivers/jcodec -I$(TOPDIR)/res -I$(TOPDIR)/fis -I$(TOPDIR)/codec_eng -I$(TOPDIR)/net/include \
            $(_CFLAGS) \
            -Wall $(DEBUG_OPT) -mmemcpy \
	        -fno-common -fno-strength-reduce -G 0 -pipe $(ENDIAN) \
            -mno-abicalls -fno-pic 

CFLAGS += $(ACFLAGS) $(ISA) $(FPU) $(OPTIONS)
#CFLAGS += $(ACFLAGS)

BUILD_NUMBER_FILE=.build_number
BUILD_NUMBER_LDFLAGS = --defsym __BUILD_DATE=$$(date +'%Y%m%d')
BUILD_NUMBER_LDFLAGS += --defsym __BUILD_NUMBER=$$(cat $(BUILD_NUMBER_FILE))
BUILD_NUMBER_LDFLAGS += --defsym __NEZHA_VER=$(NEZHA_VER)
BUILD_NUMBER_LDFLAGS += --defsym __BUILD_DATE_X=0x$$(date +'%Y%m%d')
BUILD_NUMBER_LDFLAGS += --defsym __BUILD_NUMBER_X=0x$$(cat $(BUILD_NUMBER_FILE))
BUILD_NUMBER_LDFLAGS += --defsym __NEZHA_VER_X=0x$(NEZHA_VER)
BUILD_NUMBER_LDFLAGS += --defsym __RINGTONG_NUM=$(CONFIG_RINGTONG_NUM)
BUILD_NUMBER_LDFLAGS += --defsym __RINGTONG_UNITSIZE=$(CONFIG_RINGTONG_UNITSIZE)


LDFLAGS := --nmagic $(ENDIAN) $(OFORMAT)
ASFLAGS := -D__ASSEMBLY__ $(ACFLAGS) -mips32r2
ARFLAGS = cr

LIBGCC = $(shell $(CC) $(ISA) $(FPU) $(ENDIAN) --print-libgcc-file-name)

export-objs += $(patsubst %, $(shell pwd)/%, $(OBJECTS))



ifeq ($(CONFIG_MAKE_SILENT),n)
# nothing for silent make
else
.SILENT:
endif

#
# implict compile and asmbly
#
%.o: %.c
	@echo -- gcc $< --
	$(CC) $(CFLAGS) -c -o $@ $<

%.s: %.c    
	@echo -- gcc -S -o $@ $< --
	$(CC) $(CFLAGS) -S -o $@ $< 

%.o: %.S
	@echo -- gas $< --
	$(CC) $(ASFLAGS) -c -o $@ $<

%.ro: %.res
	@echo -e "-- Generate res $@  ----"
	$(OBJCOPY) -I binary -O elf32-tradlittlemips -B mips --rename-section .data=.resource $^ $@

%.ring.rng: %.ring
	@echo -e "-- Generate ring1 $@ ----"
	@if [ $(words $(RINGS)) -ne $(CONFIG_RINGTONG_NUM) ]; then echo -e "[.config/CONFIG_RINGTONG_NUM=$(CONFIG_RINGTONG_NUM)] but\n[Makefile/RINGS=$(RINGS)] is not match"; exit -1; fi 
	$(TOPDIR)/mktools/mkringtong.sh $^ $(CONFIG_RINGTONG_UNITSIZE)
	$(OBJCOPY) -I binary -O elf32-tradlittlemips -B mips --rename-section .data=.rng $^.64k $@
	@rm $^.64k

#
# default OBJECTS and TARGET
# 
ifndef OBJECTS
S_OBJECTS=$(SOURCES:.S=.o)
OBJECTS=$(S_OBJECTS:.c=.o)
endif

ifndef TARGET
TARGET=$(DIRNAME).la
endif

ifdef S_SUBDIRS
ifeq ($(realpath .),)
SUBDIRS := $(foreach sdir, $(S_SUBDIRS), $(sdir))
else
SUBDIRS := $(foreach sdir, $(S_SUBDIRS), $(realpath $(sdir)))
endif
OBJECTS := $(foreach sdir, $(patsubst %/,%,$(S_SUBDIRS)), $(sdir)/$(sdir).lo)

ifneq ($(MAKELEVEL),)
SUPPRESS_SUBDIR_PRINT=1
endif

endif


ifeq ($(realpath .),)
SUBDIRS+= $(dir $(ALL_SUBOBJS))
else
SUBDIRS+=$(realpath $(dir $(ALL_SUBOBJS)))
endif

#
# add RING to OBJECTS
#

OBJECTS += $(patsubst %,%.rng,$(RINGS))


# Rule to build subdir TARGET
#

ifdef LO_TARGET
$(LO_TARGET): $(OBJECTS) | JUNITY
	$(LD) $(ENDIAN) -r -o $@ $^
endif

$(TARGET): $(OBJECTS) $(LIB_OBJECTS) | JUNITY
	$(AR) cr $@ $^

#
# Rules for JUNITY
# 
.PHONY: JUNITY
ifeq (junity, $(wildcard junity))
JUNITY:
	if [ -d junity ]; then make -C junity "juv=0"; fi
endif

.PHONY:diag
diag:
	@echo Make ver $(MAKE_VERSION)
	@echo TOPDIR=$(TOPDIR)
	@echo CURDIR=$(CURDIR)
	@echo SUBDIRS=$(SUBDIRS)
	@echo ALL_SUBOBJS=$(ALL_SUBOBJS)
	@echo OBJECTS=$(OBJECTS)
	@echo PROJDIRS=$(PROJDIRS)
	@echo realpath=$(realpath .)
	@echo LIBGCC=$(LIBGCC)
	@echo TARGET=$(TARGET)
	@echo LO_TARGET=$(LO_TARGET)
	@echo .DEFAULT_GOAL=$(.DEFAULT_GOAL)

##########################
# make_sub
##########################
_SUBDIR_MAKE=$(patsubst %, _subdir_%_make, $(SUBDIRS))
_MAKE_SUBDIR=$(patsubst _subdir_%_make, %, $@)
_SHORT_DIRNAME=$(patsubst $(TOPDIR)/%,%, $(_MAKE_SUBDIR))
.PHONY:make_sub
make_sub: $(_SUBDIR_MAKE)
.PHONY:$(_SUBDIR_MAKE)
$(_SUBDIR_MAKE):
ifndef SUPPRESS_SUBDIR_PRINT
	@echo -e ">> <top>/"'\E[34m'"$(_SHORT_DIRNAME)"'\E[0m\E[35G'" making <<<"
endif
	$(MAKE) --no-print-directory -C $(_MAKE_SUBDIR)


##########################
# dep_sub
##########################
_SUBDIR_DEP=$(patsubst %, _subdir_%_dep, $(SUBDIRS))
_DEP_SUBDIR=$(patsubst _subdir_%_dep, %, $@)
.PHONY: dep
dep: $(_SUBDIR_DEP) dep_sub

.PHONY:$(_SUBDIR_DEP)
$(_SUBDIR_DEP): 
	$(MAKE) --no-print-directory -C $(_DEP_SUBDIR) dep_sub

dep: dep_sub
.PHONY: dep_sub
dep_sub:
ifeq ($(wildcard .depend),)
ifneq "$(origin SOURCES)" "undefined"
	$(CC) -M $(CFLAGS) $(SOURCES) > .depend
else
	touch .depend
endif
endif

ifneq ($(wildcard .depend),)
include .depend
endif

# distclean
.PHONY: distclean
distclean: clean
	@echo ==== remove all .depend files ====
	find $(TOPDIR) -name .depend -exec rm -f {} \;
	@make --no-print-directory -C $(TOPDIR)/mktools clean


########################
# clean_sub
########################
_CLEANDIRNAME=$(patsubst $(TOPDIR)/%,%, $(realpath $(CURDIR)))
_SUBDIR_CLEAN=$(patsubst %,_subdir_%_clean, $(SUBDIRS))
_CLEAN_SUBDIR=$(patsubst _subdir_%_clean,%, $@)
_PROJDIR_CLEAN=$(patsubst %,_subdir_%_clean, $(PROJDIRS))
_CLEAN_PROJDIR=$(patsubst _subdir_%_clean,%, $@)

ifndef SUPPRESS_CLEAN
.PHONY: clean
clean: $(_SUBDIR_CLEAN) clean_sub $(_PROJDIR_CLEAN)
	@rm -f .depend
endif

.PHONY:$(_SUBDIR_CLEAN)
$(_SUBDIR_CLEAN): 
	@$(MAKE) --no-print-directory -C $(_CLEAN_SUBDIR) clean
	@rm -f $(DFU) _config.c _config.o

.PHONY:$(_PROJDIR_CLEAN)
$(_PROJDIR_CLEAN):
	@echo ""
	@echo -e "============================================================="
	@echo -e ">> $(_CLEAN_PROJDIR)/ clean "'\E[60G'"=="
	@echo -e "============================================================="
	@$(MAKE) --no-print-directory -C $(_CLEAN_PROJDIR) clean

.PHONY: clean_junity
clean_junity:
	if [ -d junity ]; then $(MAKE) --no-print-directory -C junity clean; fi

ifndef SUPPRESS_CLEAN
.PHONY: clean_sub
clean_sub: clean_junity
	@echo -e ">> <top>/"'\E[34m'"$(_CLEANDIRNAME)"'\E[0m\E[35G'" cleaning <<<"
	@rm -f $(OBJECTS) *.lo *.la *.dis *.map $(TARGET).bin $(TARGET).bin.o out.[1-9] core.* .depend .#* *.linfo *.lnor *.lres *.lsram *.ldfu *.lupg *.dfu.o *.dfu *.dfu.layout *.spi *.rp *.rp.o *.pad *.64k *.hh *.rng* *.rng *.jo *.ja *.ju *.pass
endif

##################################
# mkproj
##################################
_SUBDIR_PROJ=$(patsubst %, _subdir_%_proj, $(PROJDIRS))
_PROJ_SUBDIR=$(patsubst _subdir_%_proj,%, $@)
.PHONY:proj
proj: $(_SUBDIR_PROJ) 

.PHONY:$(_SUBDIR_PROJ)
$(_SUBDIR_PROJ):
	@echo ""
	@echo -e "============================================================="
	@echo -e "== Make Project $(_PROJ_SUBDIR)"'\E[60G'"=="
	@echo -e "============================================================="
	@$(MAKE) --no-print-directory -C $(_PROJ_SUBDIR)
	

# install_sub
.PHONY: install_sub
install_sub:
ifneq ($(EXPORT_HEADERS),)
	install -d $(INSTALLDIR)/include 
	install -m 644 $(EXPORT_HEADERS) $(INSTALLDIR)/include
endif


###########################################
# Rules for Project 
###########################################

#crt0.S: $(TOPDIR)/sys/crt0.S
#	@echo -e "-- copy crt0.S ---"
#	cp $^ $@

$(BUILD_NUMBER_FILE): $(OBJECTS) $(ALL_SUBOBJS)
	@if ! test -f $(BUILD_NUMBER_FILE); then echo 0 > $(BUILD_NUMBER_FILE); fi
	@echo $$(($$(cat $(BUILD_NUMBER_FILE)) + 1)) > $(BUILD_NUMBER_FILE)    
	
RSP_OFFSET = 0x`grep res_rp_start $@.map | awk -F " " '{print $$1}'`

ifdef EBIN_FILE
EBIN_OBJ=$(EBIN_FILE).o
endif

$(DFU): _config.o $(OBJECTS) make_sub $(BUILD_NUMBER_FILE) 
	@echo -e "== Generate $@ ver=$(NEZHA_VER) build=$$(cat $(BUILD_NUMBER_FILE)) ===="
ifdef EBIN_FILE
	$(OBJCOPY) -I binary -O elf32-tradlittlemips -B mips --rename-section .data=.ebin $(EBIN_FILE) $(EBIN_FILE).o  
endif
	$(LD) $(LDFLAGS) $(BUILD_NUMBER_LDFLAGS) -n -o $@.o  -T $(TOPDIR)/nezha.lds $(OBJECTS) _config.o $(ALL_SUBOBJS) $(EBIN_OBJ) $(LIBGCC) $(RESOURCE_RP)
	$(NM) $@.o | sort > $@.map
ifdef RESOURCE
	@echo -e "== Make Resource repackage  ==="
ifeq ($(CONFIG_OSD2_ONLY),y)
	@echo -e "-- resrp osd2_only $(RSP_OFFSET) $(RESOURCE) $(RESOURCE).rp $(RESOURCE).spi"
	$(TOPDIR)/mktools/resrp_osd/resrp $(RSP_OFFSET) $(RESOURCE) $(RESOURCE).rp $(RESOURCE).spi
else	
	@echo -e "-- resrp $(RSP_OFFSET) $(RESOURCE) $(RESOURCE).rp $(RESOURCE).spi"
	$(TOPDIR)/mktools/resrp/resrp $(RSP_OFFSET) $(RESOURCE) $(RESOURCE).rp $(RESOURCE).spi
endif	
	$(OBJCOPY) -I binary -O elf32-tradlittlemips -B mips --rename-section .data=.rsp $(RESOURCE).rp $(RESOURCE).rp.o  
	$(LD) $(LDFLAGS) $(BUILD_NUMBER_LDFLAGS) -n -o $@.o -T $(TOPDIR)/nezha.lds $(OBJECTS) _config.o $(ALL_SUBOBJS) $(EBIN_OBJ) $(LIBGCC) $(RESOURCE).rp.o
	$(NM) $@.o | sort > $@.map
endif
	@echo -e "== Make dfu $@.o ==="
	$(STRIP) -s -S $@.o
	$(OBJCOPY) -O binary -j .text -j .rodata $@.o  $@.lnor
	$(OBJCOPY) -O binary -j .data $@.o $@.lsram
	$(OBJCOPY) -O binary -j .upgrade $@.o $@.lupg
	$(OBJCOPY) -O binary -j .dfuins  $@.o $@.ldfu
ifeq ($(CONFIG_RINGTONG_NUM)$(EBIN_FILE),0)
	cat $@.lnor $@.lsram $@.lupg $@.ldfu > $@
else    
	$(OBJCOPY) -O binary -j .resource -j .res_rp -j .ebin -j .info -j .ringtong $@.o $@.lres
	cat $@.lnor $@.lsram $@.lupg $@.ldfu $@.lres > $@
endif
	chmod ogu+x $@
	$(OBJDUMP) -xhSldr $@.o > $@.dis
	#$(TOPDIR)/mktools/dfu/dfu $@ -did $(DID) -pid $(PID) -vid $(VID) > /dev/null 2>&1
	rm -f $@.ltext $@.lsram $@.lupg $@.ldfu $@.lres $@.linfo $@.lnor
	@echo -e "== Summary ROM/RAM layout ===="
	$(TOPDIR)/mktools/norsummary.sh $@.map > $@.layout


_config.c: .config
	@echo -e "-- generate _config.c ---"
	$(TOPDIR)/mktools/autoconf.sh __CONFIG_C < $^ >$@

.PHONY: DEFCONFIG
DEFCONFIG: _config.c
	@if [ ! -f .config ]; then  \
		echo "Please setup your '.config' file." \
		/bin/false; \
	fi;

#$(TOPDIR)/include/autoconf.h: $(TOPDIR)/sysconfig
#	@echo "-- generate autoconf.h ----"
#	$(TOPDIR)/mktools/autoconf.sh __AUTOCONF_H < $^ > $@


#$(TOPDIR)/include/defconf.h:$(TOPDIR)/defconf
#	@echo "-- generate defconf.h ----"
#	$(TOPDIR)/mktools/autoconf.sh __DEFCONF_H < $^ > $@

#.PHONY:SYSCONFIG
#SYSCONFIG: $(TOPDIR)/include/autoconf.h $(TOPDIR)/include/defconf.h


.PHONY: appclean
appclean:
	@rm -f _config.c _config.o 

# A rule to do nothing

dummy_sub:


# A rule to make all
.PHONY: mktools
mktools:
	@make --no-print-directory -C $(TOPDIR)/mktools

mkjunity.ja: 
ifeq ($(TOPDIR)/junity, $(wildcard $(TOPDIR)/junity))
	@echo "== make junity host library =="
	@make --no-print-directory -C $(TOPDIR)/junity
endif

mkall: $(TARGET)

mkapp: mktools mkjunity.ja DEFCONFIG dep $(DFU)

mkproj: proj
	
