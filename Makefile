#  File: Makefile
#
#  Copyright 2008-2024 NXP
#
#  This software file (the File) is distributed by NXP
#  under the terms of the GNU General Public License Version 2, June 1991
#  (the License).  You may use, redistribute and/or modify the File in
#  accordance with the terms and conditions of the License, a copy of which
#  is available by writing to the Free Software Foundation, Inc.,
#  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA or on the
#  worldwide web at http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt.
#
#  THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE
#  ARE EXPRESSLY DISCLAIMED.  The License provides additional details about
#  this warranty disclaimer.
#

CONFIG_COMPATDIR=n
ifeq ($(CONFIG_COMPATDIR), y)
COMPATDIR=/lib/modules/$(KERNELVERSION_X86)/build/compat-wireless-3.2-rc1-1/include
CC ?=		$(CROSS_COMPILE)gcc -I$(COMPATDIR)
endif

LD ?=		$(CROSS_COMPILE)ld
BACKUP=		/root/backup
YMD=		`date +%Y%m%d%H%M`

#############################################################################
# Configuration Options
#############################################################################
# Multi-chipsets
CONFIG_SD8997=y


# Debug Option
# DEBUG LEVEL n/1/2:
# n: NO DEBUG
# 1: Only PRINTM(MMSG,...), PRINTM(MFATAL,...), ...
# 2: All PRINTM()
CONFIG_DEBUG=1

# Enable STA mode support
CONFIG_STA_SUPPORT=y

# Enable uAP mode support
CONFIG_UAP_SUPPORT=y

# Enable WIFIDIRECT support
CONFIG_WIFI_DIRECT_SUPPORT=y


# Re-association in driver
CONFIG_REASSOCIATION=y


# Manufacturing firmware support
CONFIG_MFG_CMD_SUPPORT=y

# OpenWrt support
CONFIG_OPENWRT_SUPPORT=n

# Big-endian platform
CONFIG_BIG_ENDIAN=n





# SDIO suspend/resume
CONFIG_SDIO_SUSPEND_RESUME=y

# DFS testing support
CONFIG_DFS_TESTING_SUPPORT=y


# Enable Android kernel
CONFIG_ANDROID_KERNEL=n

# Enable driver/FW dump to proc
CONFIG_DUMP_TO_PROC=y




#32bit app over 64bit kernel support
CONFIG_USERSPACE_32BIT_OVER_KERNEL_64BIT=n

ifeq ($(ANDROID),)
GCC_VERSION := $(shell echo `gcc -dumpversion | cut -f1-2 -d.` \>= 4.4 | sed -e 's/\./*100+/g' | bc )
ifeq ($(GCC_VERSION),1)
	ccflags-y += -Wno-packed-bitfield-compat
endif
WimpGCC_VERSION := $(shell echo `gcc -dumpversion | cut -f1 -d.`| bc )
ifeq ($(shell test $(WimpGCC_VERSION) -ge 7; echo $$?),0)
ccflags-y += -Wimplicit-fallthrough=3
endif
#ccflags-y += -Wunused-but-set-variable
#ccflags-y += -Wmissing-prototypes
#ccflags-y += -Wold-style-definition
#ccflags-y += -Wtype-limits
#ccflags-y += -Wsuggest-attribute=format
#ccflags-y += -Wmissing-include-dirs
#ccflags-y += -Wshadow
#ccflags-y += -Wsign-compare
#ccflags-y += -Wunused-macros
#ccflags-y += -Wmissing-field-initializers
#ccflags-y += -Wstringop-truncation
#ccflags-y += -Wmisleading-indentation
#ccflags-y += -Wunused-const-variable
endif

#############################################################################
# Enable Feature Config
#############################################################################
ifneq ($(KERNELRELEASE),)
ifeq ($(CONFIG_WIRELESS_EXT),y)
ifeq ($(CONFIG_WEXT_PRIV),y)
	# Enable WEXT for STA
	CONFIG_STA_WEXT=y
	# Enable WEXT for uAP
	CONFIG_UAP_WEXT=y
else
# Disable WEXT for STA
	CONFIG_STA_WEXT=n
# Disable WEXT for uAP
	CONFIG_UAP_WEXT=n
endif
endif
# Enable CFG80211 for STA
ifeq ($(CONFIG_CFG80211),y)
	CONFIG_STA_CFG80211=y
else ifeq ($(CONFIG_CFG80211),m)
	CONFIG_STA_CFG80211=y
else
	CONFIG_STA_CFG80211=n
endif
# OpenWrt
ifeq ($(CONFIG_OPENWRT_SUPPORT), y)
ifeq ($(CPTCFG_CFG80211),y)
	CONFIG_STA_CFG80211=y
else ifeq ($(CPTCFG_CFG80211),m)
	CONFIG_STA_CFG80211=y
else
	CONFIG_STA_CFG80211=n
endif
endif
# Enable CFG80211 for uAP
ifeq ($(CONFIG_CFG80211),y)
	CONFIG_UAP_CFG80211=y
else ifeq ($(CONFIG_CFG80211),m)
	CONFIG_UAP_CFG80211=y
else
	CONFIG_UAP_CFG80211=n
endif
# OpenWrt
ifeq ($(CONFIG_OPENWRT_SUPPORT), y)
ifeq ($(CPTCFG_CFG80211),y)
	CONFIG_STA_CFG80211=y
else ifeq ($(CPTCFG_CFG80211),m)
	CONFIG_STA_CFG80211=y
else
	CONFIG_STA_CFG80211=n
endif
endif
#--------------------------------------------------
# Disable Feature if STA and UAP is not supported
#--------------------------------------------------
ifneq ($(CONFIG_STA_SUPPORT),y)
	CONFIG_WIFI_DIRECT_SUPPORT=n
	CONFIG_STA_WEXT=n
	CONFIG_STA_CFG80211=n
endif
ifneq ($(CONFIG_UAP_SUPPORT),y)
	CONFIG_WIFI_DIRECT_SUPPORT=n
	CONFIG_UAP_WEXT=n
	CONFIG_UAP_CFG80211=n
endif
endif
#############################################################################
# Select Platform Tools
#############################################################################
ifeq ($(ANDROID), yes)
# Set target Android SDK version.
# ANDROID_SDK_VERSION 29 corresponds to Android 10 Android 10
# ANDROID_SDK_VERSION 30 corresponds to Android 11 (Red Velvet Cake)
# ANDROID_SDK_VERSION 31 corresponds to Android 12 (Snow Cone)
# ANDROID_SDK_VERSION 33 corresponds to Android 13 (Tiramisu)
# ANDROID_SDK_VERSION 34 corresponds to Android 14 (Upside Down Cake)
# ANDROID_SDK_VERSION 35 corresponds to Android 15 (Vanilla Ice Cream)
# ANDROID_SDK_VERSION 36 corresponds to Android 16
    KERNEL_CFLAGS += -DANDROID
    PWD := $(shell pwd)
    KERNELDIR ?= $(KERNEL_SRC)
    ccflags-y += -DANDROID_SDK_VERSION=$(ANDROID_SDK_VERSION)
endif

MODEXT = ko
ccflags-y += -I$(PWD)/mlan
ccflags-y += -DLINUX







KERNELVERSION_X86 := 	$(shell uname -r)
KERNELDIR ?= /lib/modules/$(KERNELVERSION_X86)/build
LD += -S

BINDIR = bin_wlan


APPDIR= $(shell if test -d "mapp"; then echo mapp; fi)

#############################################################################
# Compiler Flags
#############################################################################

	ccflags-y += -I$(KERNELDIR)/include
	ccflags-y += -DMLAN_RELEASE_VERSION='"540.p34"'

	ccflags-y += -DFPNUM='"92"'

ifeq ($(CONFIG_DEBUG),1)
	ccflags-y += -DDEBUG_LEVEL1
endif

ifeq ($(CONFIG_DEBUG),2)
	ccflags-y += -DDEBUG_LEVEL1
	ccflags-y += -DDEBUG_LEVEL2
	DBG=	-dbg
endif

ifeq ($(CONFIG_64BIT), y)
	ccflags-y += -DMLAN_64BIT
endif

ifeq ($(CONFIG_STA_SUPPORT),y)
	ccflags-y += -DSTA_SUPPORT
ifeq ($(CONFIG_REASSOCIATION),y)
	ccflags-y += -DREASSOCIATION
endif
endif

ifeq ($(CONFIG_UAP_SUPPORT),y)
	ccflags-y += -DUAP_SUPPORT

endif

ifeq ($(CONFIG_WIFI_DIRECT_SUPPORT),y)
	ccflags-y += -DWIFI_DIRECT_SUPPORT
endif

ifeq ($(CONFIG_MFG_CMD_SUPPORT),y)
	ccflags-y += -DMFG_CMD_SUPPORT
endif

ifeq ($(CONFIG_BIG_ENDIAN),y)
	ccflags-y += -DBIG_ENDIAN_SUPPORT
endif

ifeq ($(CONFIG_USERSPACE_32BIT_OVER_KERNEL_64BIT),y)
	ccflags-y += -DUSERSPACE_32BIT_OVER_KERNEL_64BIT
endif

ifeq ($(CONFIG_SDIO_SUSPEND_RESUME),y)
	ccflags-y += -DSDIO_SUSPEND_RESUME
endif

ifeq ($(CONFIG_DFS_TESTING_SUPPORT),y)
	ccflags-y += -DDFS_TESTING_SUPPORT
endif


ifeq ($(CONFIG_ANDROID_KERNEL), y)
	ccflags-y += -DANDROID_KERNEL
	CONFIG_DUMP_TO_PROC=y
endif

ifeq ($(CONFIG_DUMP_TO_PROC), y)
	ccflags-y += -DDUMP_TO_PROC
endif

ifeq ($(CONFIG_FWDUMP_VIA_PRINT), y)
	ccflags-y += -DFWDUMP_VIA_PRINT
endif


ifeq ($(CONFIG_OPENWRT_SUPPORT), y)
	ccflags-y += -DOPENWRT
endif


ifeq ($(CONFIG_SD8997),y)
	CONFIG_SDIO=y
	ccflags-y += -DSDIO
	ccflags-y += -DSDIO_MMC
	ccflags-y += -DSD8997
endif


#############################################################################
# Make Targets
#############################################################################

ifneq ($(KERNELRELEASE),)
ifeq ($(CONFIG_STA_SUPPORT),y)
ifeq ($(CONFIG_STA_WEXT),y)
	ccflags-y += -DSTA_WEXT
endif
ifeq ($(CONFIG_STA_CFG80211),y)
	ccflags-y += -DSTA_CFG80211
endif
endif
ifeq ($(CONFIG_UAP_SUPPORT),y)
ifeq ($(CONFIG_UAP_WEXT),y)
	ccflags-y += -DUAP_WEXT
endif
ifeq ($(CONFIG_UAP_CFG80211),y)
	ccflags-y += -DUAP_CFG80211
endif
endif

print:
ifeq ($(CONFIG_STA_SUPPORT),y)
ifeq ($(CONFIG_STA_WEXT),n)
ifeq ($(CONFIG_STA_CFG80211),n)
	@echo "Can not build STA without WEXT or CFG80211"
	exit 2
endif
endif
endif
ifeq ($(CONFIG_UAP_SUPPORT),y)
ifeq ($(CONFIG_UAP_WEXT),n)
ifeq ($(CONFIG_UAP_CFG80211),n)
	@echo "Can not build UAP without WEXT or CFG80211"
	exit 2
endif
endif
endif





# Default for out-of-tree builds
CONFIG_NXP_WLAN_DRIVER ?= m

MOALOBJS =	mlinux/moal_main.o \
		mlinux/moal_ioctl.o \
		mlinux/moal_shim.o \
		mlinux/moal_eth_ioctl.o \
		mlinux/moal_init.o

MLANOBJS =	mlan/mlan_shim.o mlan/mlan_init.o \
		mlan/mlan_txrx.o \
		mlan/mlan_cmdevt.o mlan/mlan_misc.o \
		mlan/mlan_cfp.o \
		mlan/mlan_module.o

MLANOBJS += mlan/mlan_wmm.o
MLANOBJS += mlan/mlan_sdio.o
MLANOBJS += mlan/mlan_11n_aggr.o
MLANOBJS += mlan/mlan_11n_rxreorder.o
MLANOBJS += mlan/mlan_11n.o
MLANOBJS += mlan/mlan_11ac.o
MLANOBJS += mlan/mlan_11d.o
MLANOBJS += mlan/mlan_11h.o
ifeq ($(CONFIG_STA_SUPPORT),y)
MLANOBJS += mlan/mlan_meas.o
MLANOBJS += mlan/mlan_scan.o \
			mlan/mlan_sta_ioctl.o \
			mlan/mlan_sta_rx.o \
			mlan/mlan_sta_tx.o \
			mlan/mlan_sta_event.o \
			mlan/mlan_sta_cmd.o \
			mlan/mlan_sta_cmdresp.o \
			mlan/mlan_join.o
ifeq ($(CONFIG_STA_WEXT),y)
MOALOBJS += mlinux/moal_priv.o \
            mlinux/moal_wext.o
endif
endif
ifeq ($(CONFIG_UAP_SUPPORT),y)
MLANOBJS += mlan/mlan_uap_ioctl.o
MLANOBJS += mlan/mlan_uap_cmdevent.o
MLANOBJS += mlan/mlan_uap_txrx.o
MOALOBJS += mlinux/moal_uap.o
ifeq ($(CONFIG_UAP_WEXT),y)
MOALOBJS += mlinux/moal_uap_priv.o
MOALOBJS += mlinux/moal_uap_wext.o
endif
endif
ifeq ($(CONFIG_STA_CFG80211),y)
MOALOBJS += mlinux/moal_cfg80211.o
MOALOBJS += mlinux/moal_cfg80211_util.o
MOALOBJS += mlinux/moal_sta_cfg80211.o
endif
ifeq ($(CONFIG_UAP_CFG80211),y)
MOALOBJS += mlinux/moal_cfg80211.o
MOALOBJS += mlinux/moal_cfg80211_util.o
MOALOBJS += mlinux/moal_uap_cfg80211.o
endif

ifdef CONFIG_PROC_FS
MOALOBJS += mlinux/moal_proc.o
MOALOBJS += mlinux/moal_debug.o
endif


obj-$(CONFIG_NXP_WLAN_DRIVER) := mlan.o
mlan-objs := $(MLANOBJS)

MOALOBJS += mlinux/moal_sdio_mmc.o
obj-$(CONFIG_NXP_WLAN_DRIVER) += sdxxx.o
sdxxx-objs := $(MOALOBJS)

# Otherwise we were called directly from the command line; invoke the kernel build system.
else

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

endif

###############################################################

export		CC LD ccflags-y KERNELDIR

.PHONY: clean distclean
	@echo "Finished Making NXP Wlan Linux Driver"

appsbuild:

	@if [ ! -d $(BINDIR) ]; then \
		mkdir $(BINDIR); \
	fi

ifeq ($(CONFIG_STA_SUPPORT),y)
	cp -f README $(BINDIR)
endif

build:		echo default

	@if [ ! -d $(BINDIR) ]; then \
		mkdir $(BINDIR); \
	fi

	cp -f mlan.$(MODEXT) $(BINDIR)/mlan$(DBG).$(MODEXT)

	cp -f sdxxx.$(MODEXT) $(BINDIR)/sd8997$(DBG).$(MODEXT)
	cp -rpf script/sdio_mmc/* $(BINDIR)/

clean:
	-find . -name "*.o" -exec rm {} \;
	-find . -name "*.ko" -exec rm {} \;
	-find . -name ".*.cmd" -exec rm {} \;
	-find . -name "*.mod.c" -exec rm {} \;
	-find . -name "Module.symvers" -exec rm {} \;
	-find . -name "Module.markers" -exec rm {} \;
	-find . -name "modules.order" -exec rm {} \;
	-find . -name ".*.dwo" -exec rm {} \;
	-find . -name "*dwo" -exec rm {} \;
	-find . -name "*.mod" -exec rm {} \;
	-find . -name ".*.o.d" -exec rm {} \;
	-rm -rf .tmp_versions

install: default

	cp -f mlan.$(MODEXT) $(INSTALLDIR)/mlan$(DBG).$(MODEXT)
	cp -f ../io/sdio/$(PLATFORM)/sdio.$(MODEXT) $(INSTALLDIR)
	cp -f sdxxx.$(MODEXT) $(INSTALLDIR)/sd8997$(DBG).$(MODEXT)
	echo "sd8997 Driver Installed"

distclean:
	-find . -name "*.o" -exec rm {} \;
	-find . -name "*.orig" -exec rm {} \;
	-find . -name "*.swp" -exec rm {} \;
	-find . -name "*.*~" -exec rm {} \;
	-find . -name "*~" -exec rm {} \;
	-find . -name "*.d" -exec rm {} \;
	-find . -name "*.a" -exec rm {} \;
	-find . -name "tags" -exec rm {} \;
	-find . -name ".*" -exec rm -rf 2> /dev/null \;
	-find . -name "*.ko" -exec rm {} \;
	-find . -name ".*.cmd" -exec rm {} \;
	-find . -name "*.mod.c" -exec rm {} \;
	-find . -name ".*.dwo" -exec rm {} \;
	-find . -name "*dwo" -exec rm {} \;
	-find . -name "*.mod" -exec rm {} \;
	-find . -name ".*.o.d" -exec rm {} \;
	-rm -rf .tmp_versions

# End of file
