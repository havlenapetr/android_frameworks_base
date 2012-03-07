/*
 * Copyright (C) 2012 Havlena Petr
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

#define LOG_TAG "ASurface"
#include <utils/Log.h>

#include <surfaceflinger/Surface.h>

#include <SkCanvas.h>
#include <SkBitmap.h>
#include <SkMatrix.h>
#include <SkRect.h>

#include "surface.h"

#define CHECK(val) \
    if(!val) { \
        LOGE("%s [%i]: NULL pointer exception!", __func__, __LINE__); \
        return -1; \
    }

#define SDK_VERSION_FROYO 8

using namespace android;

typedef struct ASurface {
    /* our private members here */
    Surface* surface;
    SkCanvas canvas;
} ASurface;

static Surface* getNativeSurface(JNIEnv* env, jobject jsurface, int sdkVersion) {
    /* we know jsurface is a valid local ref, so use it */
    jclass clazz = env->GetObjectClass(jsurface);
    if(clazz == NULL) {
        LOGE("Can't find surface class!");
        return NULL;
    }

    jfieldID field_surface = env->GetFieldID(clazz,
                                             sdkVersion > SDK_VERSION_FROYO ? "mNativeSurface" : "mSurface",
                                             "I");
    if(field_surface == NULL) {
        LOGE("Can't find native surface field!");
        return NULL;
    }
    return (Surface *) env->GetIntField(jsurface, field_surface);
}

int ASurface_init(JNIEnv* env, jobject jsurface, int sdkVersion, ASurface** aSurface) {
    if(!env || jsurface == NULL) {
        LOGE("JNIEnv or jsurface obj is NULL!");
        return -1;
    }

    Surface* surface = getNativeSurface(env, jsurface, sdkVersion);
    if(!surface) {
        LOGE("Can't obtain native surface!");
        return -1;
    }

    *aSurface = (ASurface *) malloc(sizeof(ASurface));
    (*aSurface)->surface = surface;
    return 0;
}

void ASurface_deinit(ASurface** aSurface) {
    free(*aSurface);
    *aSurface = NULL;
}

int ASurface_lock(ASurface* aSurface, AndroidSurfaceInfo* info) {
    static Surface::SurfaceInfo surfaceInfo;

    CHECK(aSurface);
    CHECK(aSurface->surface);

    Surface* surface = aSurface->surface;
    if (!surface->isValid()) {
        LOGE("Native surface isn't valid!");
        return -1;
    }

    int res = surface->lock(&surfaceInfo);
    if(res < 0) {
        LOGE("Can't lock native surface!");
        return res;
    }

    info->w = surfaceInfo.w;
    info->h = surfaceInfo.h;
    info->s = surfaceInfo.s;
    info->usage = surfaceInfo.usage;
    info->format = surfaceInfo.format;
    info->bits = surfaceInfo.bits;

    return 0;
}

static SkBitmap::Config
convertPixelFormat(APixelFormat format) {
    switch(format) {
        case ANDROID_PIXEL_FORMAT_RGBX_8888:
        case ANDROID_PIXEL_FORMAT_RGBA_8888:
            return SkBitmap::kARGB_8888_Config;
        case ANDROID_PIXEL_FORMAT_RGB_565:
            return SkBitmap::kRGB_565_Config;
    }
    return SkBitmap::kNo_Config;
}

static void
initBitmap(SkBitmap& bitmap, AndroidSurfaceInfo* info) {
    bitmap.setConfig(convertPixelFormat(info->format), info->w, info->h);
    if (info->format == ANDROID_PIXEL_FORMAT_RGBX_8888) {
        bitmap.setIsOpaque(true);
    }
    if (info->w > 0 && info->h > 0) {
        bitmap.setPixels(info->bits);
    } else {
        // be safe with an empty bitmap.
        bitmap.setPixels(NULL);
    }
}

void ASurface_scaleToFullScreen(ASurface* aSurface, AndroidSurfaceInfo* src, AndroidSurfaceInfo* dst) {
    SkBitmap    srcBitmap;
    SkBitmap    dstBitmap;
    SkMatrix    matrix;

    initBitmap(srcBitmap, src);
    initBitmap(dstBitmap, dst);
    matrix.setRectToRect(SkRect::MakeWH(srcBitmap.width(), srcBitmap.height()),
                         SkRect::MakeWH(dstBitmap.width(), dstBitmap.height()),
                         SkMatrix::kFill_ScaleToFit);

    aSurface->canvas.setBitmapDevice(dstBitmap);
    aSurface->canvas.drawBitmapMatrix(srcBitmap, matrix);
}

int ASurface_rotate(ASurface* aSurface, AndroidSurfaceInfo* src, uint32_t degrees) {
    SkBitmap    bitmap;

    CHECK(aSurface);
    CHECK(src);

    initBitmap(bitmap, src);
    aSurface->canvas.setBitmapDevice(bitmap);
    return aSurface->canvas.rotate(SkScalar(degrees)) ? 0 : -1;
}

int ASurface_unlockAndPost(ASurface* aSurface) {
    CHECK(aSurface);
    CHECK(aSurface->surface);
    return aSurface->surface->unlockAndPost();
}
