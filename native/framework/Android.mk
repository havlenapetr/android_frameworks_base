LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

# our source files
#
LOCAL_SRC_FILES:= \
    native_canvas.cpp \
    native_surface.cpp \
    native_bitmap_factory.cpp

LOCAL_SHARED_LIBRARIES := \
    libutils \
    libsurfaceflinger_client \
    libskia \
    libui

LOCAL_C_INCLUDES := \
    external/skia/include/core \
    external/skia/include/images \
    frameworks/base/native/framework/include

LOCAL_MODULE:= libframework
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)