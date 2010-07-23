LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

# our source files
#
LOCAL_SRC_FILES:= \
	surface.cpp

LOCAL_SHARED_LIBRARIES := \
	libskia \
    	libsurfaceflinger_client \
    	libutils \
	liblog

LOCAL_C_INCLUDES += \
	$(JNI_H_INCLUDE) \
	external/skia/src/core \
	external/skia/include/core \
	frameworks/base/include \
	frameworks/base/native/include

# Optional tag would mean it doesn't get installed by default
LOCAL_MODULE_TAGS := optional

LOCAL_PRELINK_MODULE := false

LOCAL_MODULE:= libjnivideo

include $(BUILD_SHARED_LIBRARY)

