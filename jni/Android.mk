TOP_LOCAL_PATH:=$(call my-dir)
include $(call all-subdir-makefiles)
LOCAL_PATH := $(TOP_LOCAL_PATH)

include $(CLEAR_VARS)
LOCAL_MODULE    := thcipv6
LOCAL_SRC_FILES := $(wildcard $(LOCAL_PATH)/*.c)
LOCAL_C_INCLUDED += $(patsubst $(LOCAL_PATH)/%, %, $(wildcard folder/**/*.h))
 
include $(BUILD_SHARED_LIBRARY)