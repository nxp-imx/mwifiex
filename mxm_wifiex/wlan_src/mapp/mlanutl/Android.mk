# Copyright 2021 NXP
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
##      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

LOCAL_PATH := $(my-dir)
include $(CLEAR_VARS)

# Enable DEBUG Level
DEBUG_LEVEL=1
ifeq ($(DEBUG_LEVEL),1)
LOCAL_CFLAGS += -DDEBUG_LEVEL1
endif
ifeq ($(DEBUG_LEVEL),2)
LOCAL_CFLAGS += -DDEBUG_LEVEL1
LOCAL_CFLAGS += -DDEBUG_LEVEL2
endif
# Enable STA mode support
CONFIG_STA_SUPPORT=y
ifeq ($(CONFIG_STA_SUPPORT),y)
LOCAL_CFLAGS += -DSTA_SUPPORT
endif
# Enable uAP mode support
CONFIG_UAP_SUPPORT=y
ifeq ($(CONFIG_UAP_SUPPORT),y)
LOCAL_CFLAGS += -DUAP_SUPPORT
endif
CONFIG_WIFI_DIRECT_SUPPORT=y
ifeq ($(CONFIG_WIFI_DIRECT_SUPPORT), y)
LOCAL_CFLAGS += -DWIFI_DIRECT_SUPPORT
endif

# if 64bit Android is used (e.g. PXA1928)
# Please turn on following compiler flag
CONFIG_USERSPACE_32BIT_OVER_KERNEL_64BIT=y
ifeq ($(CONFIG_USERSPACE_32BIT_OVER_KERNEL_64BIT), y)
LOCAL_CFLAGS += -DUSERSPACE_32BIT_OVER_KERNEL_64BIT
endif

LOCAL_MODULE := mlanutl
LOCAL_VENDOR_MODULE := true
OBJS = mlanutl.c
LOCAL_SRC_FILES := $(OBJS)
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)
