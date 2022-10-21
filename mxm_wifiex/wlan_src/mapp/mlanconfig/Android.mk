LOCAL_PATH := $(my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := mlanconfig
OBJS = mlanconfig.c mlanhostcmd.c mlanmisc.c
LOCAL_SRC_FILES := $(OBJS)
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)
