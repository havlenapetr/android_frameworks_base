LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

# our source files
#
LOCAL_SRC_FILES:= \
	audiotrack.cpp

LOCAL_SHARED_LIBRARIES := \
	libbinder \
    	libmedia \
    	libutils \
	liblog

LOCAL_C_INCLUDES += \
	$(JNI_H_INCLUDE) \
	frameworks/base/include \
	frameworks/base/native/include

# Optional tag would mean it doesn't get installed by default
LOCAL_MODULE_TAGS := optional

LOCAL_PRELINK_MODULE := false

LOCAL_MODULE:= libjniaudio

include $(BUILD_SHARED_LIBRARY)

