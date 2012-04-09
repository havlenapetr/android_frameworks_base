/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ANDROID_SURFACE_H
#define ANDROID_SURFACE_H

#include <stdint.h>
#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

struct ASurface;
typedef struct ASurface ASurface;

enum {
    //
    // these constants need to match those
    // in graphics/PixelFormat.java & pixelflinger/format.h
    //
    ANDROID_PIXEL_FORMAT_UNKNOWN    =   0,
    ANDROID_PIXEL_FORMAT_NONE       =   0,

    // logical pixel formats used by the SurfaceFlinger -----------------------
    ANDROID_PIXEL_FORMAT_CUSTOM         = -4,
        // Custom pixel-format described by a PixelFormatInfo structure

    ANDROID_PIXEL_FORMAT_TRANSLUCENT    = -3,
        // System chooses a format that supports translucency (many alpha bits)

    ANDROID_PIXEL_FORMAT_TRANSPARENT    = -2,
        // System chooses a format that supports transparency
        // (at least 1 alpha bit)

    ANDROID_PIXEL_FORMAT_OPAQUE         = -1,
        // System chooses an opaque format (no alpha bits required)

    ANDROID_PIXEL_FORMAT_RGBA_8888      = 1,
    ANDROID_PIXEL_FORMAT_RGBX_8888      = 2,
    ANDROID_PIXEL_FORMAT_RGB_888        = 3,
    ANDROID_PIXEL_FORMAT_RGB_565        = 4,
    ANDROID_PIXEL_FORMAT_RGBA_5551      = 6,
    ANDROID_PIXEL_FORMAT_RGBA_4444      = 7,
    ANDROID_PIXEL_FORMAT_YCbCr_420_SP   = 17,
};

typedef int32_t APixelFormat;

typedef struct {
    uint32_t    w;
    uint32_t    h;
    uint32_t    s;
    uint32_t    usage;
    APixelFormat format;
    void*       bits;
} AndroidSurfaceInfo;

int ASurface_init(JNIEnv* env, jobject jsurface, int sdkVersion, ASurface** aSurface);

void ASurface_deinit(ASurface** aSurface);

int ASurface_lock(ASurface* aSurface, AndroidSurfaceInfo* info);

int ASurface_rotate(ASurface* aSurface, AndroidSurfaceInfo* src, uint32_t degrees);

void ASurface_scaleToFullScreen(ASurface* aSurface, AndroidSurfaceInfo* src, AndroidSurfaceInfo* dst);

int ASurface_unlockAndPost(ASurface* aSurface);

#ifdef __cplusplus
}
#endif

#endif
