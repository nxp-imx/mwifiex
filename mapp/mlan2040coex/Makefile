#
# File : mlan2040coex/Makefile
#
# Copyright 2009-2022 NXP

# Path to the top directory of the mlandriver distribution
PATH_TO_TOP = ../..

ifeq ($(findstring IMX_ANDROID,$(ccflags-y)),IMX_ANDROID)
CC=/usr/local/arm/android-ndk-r23-beta5/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android21-clang
endif

# Determine how we should copy things to the install directory
ABSPATH := $(filter /%, $(INSTALLDIR))
RELPATH := $(filter-out /%, $(INSTALLDIR))
INSTALLPATH := $(ABSPATH)
ifeq ($(strip $(INSTALLPATH)),)
INSTALLPATH := $(PATH_TO_TOP)/$(RELPATH)
endif

# Override CFLAGS for application sources, remove __ kernel namespace defines
CFLAGS := $(filter-out -D__%, $(ccflags-y))
# remove KERNEL include dir
CFLAGS := $(filter-out -I$(KERNELDIR)%, $(CFLAGS))

#
# List of application executables to create
#
libobjs:= mlan2040coex.o mlan2040misc.o
exectarget=mlan2040coex
TARGETS := $(exectarget)

#
# Make target rules
#

# All rule compiles list of TARGETS using builtin program target from src rule
all :
$(exectarget): $(libobjs)
	$(CC) $(CFLAGS) $(libobjs) -o $(exectarget)

# Update any needed TARGETS and then copy to the install path
build appsbuild install: $(TARGETS)
	@cp -f $(exectarget) $(INSTALLPATH)

clean:
	@rm -f $(exectarget)
	@rm -f *.o

distclean: clean
	@rm -f *~ core
	@rm -f tags
