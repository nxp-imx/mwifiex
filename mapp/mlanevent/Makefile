# File : mlanevent/Makefile
#
# Copyright 2008-2022 NXP

# Path to the top directory of the wlan distribution
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


#CFLAGS += -DAP22 -fshort-enums
CFLAGS += -Wall
#ECHO = @
ifeq (,$(findstring ANDROID_KERNEL, $(CFLAGS)))
LIBS=-lrt
endif



.PHONY: default tags all

OBJECTS = mlanevent.o
HEADERS = mlanevent.h

TARGET = mlanevent.exe

build appsbuild default: $(TARGET)
	@cp -f $(TARGET) $(INSTALLPATH)

all : tags default

$(TARGET): $(OBJECTS) $(HEADERS)
	$(ECHO)$(CC) $(LIBS) -o $@ $(OBJECTS)

%.o: %.c $(HEADERS)
	$(ECHO)$(CC) $(CFLAGS) -c -o $@ $<

tags:
	ctags -R -f tags.txt

distclean clean:
	$(ECHO)$(RM) $(OBJECTS) $(TARGET)
	$(ECHO)$(RM) tags.txt

