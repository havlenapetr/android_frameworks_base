LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

USE_SURFACE_WRAPPER := true

# our source files
#
LOCAL_SRC_FILES:=

LOCAL_C_INCLUDES += \
    $(JNI_H_INCLUDE) \
    frameworks/base/include

LOCAL_SHARED_LIBRARIES := \
    libutils

# for surface API
ifeq ($(USE_SURFACE_WRAPPER),true)
LOCAL_SRC_FILES += surface.cpp

LOCAL_C_INCLUDES += \
    external/skia/include/core

LOCAL_SHARED_LIBRARIES += \
    libskia \
    libgui \
    libsurfaceflinger_client
endif

# for camera API
ifeq ($(USE_CAMERA_WRAPPER),true)
LOCAL_SRC_FILES += camera.cpp
LOCAL_SHARED_LIBRARIES += \
    libcamera_client \
    libandroid_runtime
endif

# Optional tag would mean it doesn't get installed by default
LOCAL_MODULE_TAGS := optional

LOCAL_PRELINK_MODULE := false

LOCAL_MODULE:= libndk-glue

include $(BUILD_SHARED_LIBRARY)
