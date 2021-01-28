# Copyright 2021 NXP
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

KERNEL_DIR := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ
TARGET_ARCH := $(TARGET_KERNEL_ARCH)
MXMWIFI_CROSS_COMPILE := aarch64-linux-gnu-

MXMWIFI_SRC_PATH := $(MXMWIFI_PATH)/mxm_wifiex/wlan_src
MXMWIFI_OUT := $(TARGET_OUT_INTERMEDIATES)/MXMWIFI_OBJ

KERNEL_CFLAGS ?= KCFLAGS=-mno-android
ARCH ?= $(TARGET_ARCH)

MXMWIFI_KERNELENVSH := $(MXMWIFI_OUT)/kernelenv.sh
$(MXMWIFI_KERNELENVSH):
	mkdir -p $(MXMWIFI_OUT)
	echo 'export KERNEL_DIR=$(KERNEL_DIR)' > $(MXMWIFI_KERNELENVSH)
	echo 'export CROSS_COMPILE=$(MXMWIFI_CROSS_COMPILE)' >> $(MXMWIFI_KERNELENVSH)
	echo 'export ARCH=$(ARCH)' >> $(MXMWIFI_KERNELENVSH)

mxmwifi: $(MXMWIFI_KERNELENVSH) $(MXMWIFI_SRC_PATH)
	$(hide) if [ ${clean_build} = 1 ]; then \
		PATH=$$PATH $(MAKE)  -C $(MXMWIFI_SRC_PATH) ANDROID=yes clean; \
	fi
	@ . $(MXMWIFI_KERNELENVSH); $(kernel_build_shell_env) \
	$(MAKE)  -C $(MXMWIFI_SRC_PATH) ANDROID=yes \
		PWD=$(ANDROID_BUILD_TOP)/vendor/nxp-opensource/nxp-mwifiex/mxm_wifiex/wlan_src \
		$(CLANG_TO_COMPILE) \
		$(KERNEL_CFLAGS) \
		ARCH=$(ARCH) \
		DEBUG=$(DEBUG); \
	cp $(MXMWIFI_SRC_PATH)/mlan.ko $(MXMWIFI_OUT);
	cp $(MXMWIFI_SRC_PATH)/moal.ko $(MXMWIFI_OUT);
