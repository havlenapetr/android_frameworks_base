/*
 * Copyright (C) 2011  Havlena Petr <havlenapetr@gmail.com>
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

#define LOG_TAG "Surface"
#include <utils/Log.h>

#include <android/native_surface_jni.h>
#include <surfaceflinger/Surface.h>

#include <SkCanvas.h>

#define SURFACE_CLASS_PATH  "android/view/Surface"

using namespace android;

typedef struct ANativeSurface {
    Surface*            surface;
    ANativeCanvas*      canvas;
    int                 saveCount;
} ANativeSurface;

extern SkCanvas* ANativeCanvas_getSkCanvas(ANativeCanvas* canvas);
extern ANativeCanvas* ANativeCanvas_fromSurface(JNIEnv* env, jobject jsurface);
extern void ANativeCanvas_setWidth(ANativeCanvas* canvas, int32_t width);
extern void ANativeCanvas_setHeight(ANativeCanvas* canvas, int32_t height);
extern void ANativeCanvas_release(ANativeCanvas* canvas);
//extern ANativeCanvas* ANativeCanvas_acquire();
//extern void ANativeCanvas_release(ANativeCanvas* canvas);

static inline SkBitmap::Config convertPixelFormat(PixelFormat format) {
    /* note: if PIXEL_FORMAT_RGBX_8888 means that all alpha bytes are 0xFF, then
     we can map to SkBitmap::kARGB_8888_Config, and optionally call
     bitmap.setIsOpaque(true) on the resulting SkBitmap (as an accelerator)
     */
    switch (format) {
        case PIXEL_FORMAT_RGBX_8888:    return SkBitmap::kARGB_8888_Config;
        case PIXEL_FORMAT_RGBA_8888:    return SkBitmap::kARGB_8888_Config;
        case PIXEL_FORMAT_RGBA_4444:    return SkBitmap::kARGB_4444_Config;
        case PIXEL_FORMAT_RGB_565:      return SkBitmap::kRGB_565_Config;
        case PIXEL_FORMAT_A_8:          return SkBitmap::kA8_Config;
        default:                        return SkBitmap::kNo_Config;
    }
}

ANativeSurface* ANativeSurface_fromSurface(JNIEnv* env, jobject jsurface) {
    jclass clazz = env->FindClass(SURFACE_CLASS_PATH);
	jfieldID field_surface = env->GetFieldID(clazz, "mNativeSurface", "I");
	if(field_surface == NULL) {
        LOGE("Can't obtain native surface pointer");
		return NULL;
	}
	
    ANativeSurface* surf = (ANativeSurface *) malloc(sizeof(ANativeSurface));
    surf->surface = (Surface *) env->GetIntField(jsurface, field_surface);
    
    ANativeCanvas* canvas = ANativeCanvas_fromSurface(env, jsurface);
    if(canvas == NULL) {
        LOGE("Can't obtain ANativeCanvas");
        return NULL;
    }
    surf->canvas = canvas;
    return surf;
}

void ANativeSurface_release(ANativeSurface* surface) {
    ANativeCanvas_release(surface->canvas);
}

ANativeCanvas* ANativeSurface_lockCanvas(ANativeSurface* surface, ARect* inOutDirtyBounds) {
    Region dirtyRegion;
    Region* dirtyParam = NULL;
    if (inOutDirtyBounds != NULL) {
        dirtyRegion.set(*(Rect*)inOutDirtyBounds);
        dirtyParam = &dirtyRegion;
    }
    
    Surface::SurfaceInfo info;
    Surface* surf = surface->surface;
    if (!Surface::isValid(surf)) {
        LOGE("Surface is not valid!");
        return NULL;
    }

    status_t res = surf->lock(&info, dirtyParam);
    if (res != OK) {
        LOGE("Can't lock surface!");
        return NULL;
    }
    
    // set size for canvas
    ANativeCanvas_setWidth(surface->canvas, info.w);
    ANativeCanvas_setHeight(surface->canvas, info.h);

    // prepare bitmap, which will handle screen's pixels
    SkBitmap bitmap;
    ssize_t bpr = info.s * bytesPerPixel(info.format);
    bitmap.setConfig(convertPixelFormat(info.format), info.w, info.h, bpr);
    if (info.format == PIXEL_FORMAT_RGBX_8888) {
        bitmap.setIsOpaque(true);
    }
    if (info.w > 0 && info.h > 0) {
        bitmap.setPixels(info.bits);
    } else {
        // be safe with an empty bitmap.
        bitmap.setPixels(NULL);
    }
    SkCanvas* nativeCanvas = ANativeCanvas_getSkCanvas(surface->canvas);
    nativeCanvas->setBitmapDevice(bitmap);
    /*SkRegion clipReg;
    if (dirtyRegion.isRect()) { // very common case
        const Rect b(dirtyRegion.getBounds());
        clipReg.setRect(b.left, b.top, b.right, b.bottom);
    } else {
        size_t count;
        Rect const* r = dirtyRegion.getArray(&count);
        while (count) {
            clipReg.op(r->left, r->top, r->right, r->bottom, SkRegion::kUnion_Op);
            r++, count--;
        }
    }
    nativeCanvas->clipRegion(clipReg);*/
    surface->saveCount = nativeCanvas->save();
    return surface->canvas;
}

int32_t ANativeSurface_unlockCanvasAndPost(ANativeSurface* surface)
{
    Surface* surf = surface->surface;
    if (!Surface::isValid(surf)) {
        LOGE("Surface is not valid!");
        return -1;
    }
    // detach the canvas from the surface
    SkCanvas* nativeCanvas = ANativeCanvas_getSkCanvas(surface->canvas);
    int saveCount = surface->saveCount;
    nativeCanvas->restoreToCount(saveCount);
    nativeCanvas->setBitmapDevice(SkBitmap());
    surface->saveCount = 0;
    // unlock surface
    return surf->unlockAndPost();
}

int32_t ANativeSurface_lock(ANativeSurface* surface, ANativeSurface_Buffer* outBuffer,
        ARect* inOutDirtyBounds) {
    Region dirtyRegion;
    Region* dirtyParam = NULL;
    if (inOutDirtyBounds != NULL) {
        dirtyRegion.set(*(Rect*)inOutDirtyBounds);
        dirtyParam = &dirtyRegion;
    }
    
    Surface::SurfaceInfo info;
    Surface* surf = surface->surface;
    status_t res = surf->lock(&info, dirtyParam);
    if (res != OK) {
        return -1;
    }
    
    outBuffer->width = (int32_t)info.w;
    outBuffer->height = (int32_t)info.h;
    outBuffer->stride = (int32_t)info.s;
    outBuffer->format = (int32_t)info.format;
    outBuffer->bits = info.bits;
    
    if (inOutDirtyBounds != NULL) {
        *inOutDirtyBounds = dirtyRegion.getBounds();
    }
    
    return 0;
}

int32_t ANativeSurface_unlockAndPost(ANativeSurface* surface) {
    Surface* surf = surface->surface;
    status_t res = surf->unlockAndPost();
    return res == android::OK ? 0 : -1;
}