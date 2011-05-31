/*
 * Copyright (C) 2011 Petr Havlena  havlenapetr@gmail.com
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
#include <android/surface.h>
#include <surfaceflinger/Surface.h>
#include <utils/Log.h>
#include <SkBitmap.h>
#include <SkCanvas.h>

#define TAG "SurfaceWrapper"

using namespace android;

static Surface*         surface;
static Surface::SurfaceInfo surfaceInfo;

static SkBitmap         bmpClient;
static SkBitmap         bmpSurface;
static SkRect           rect_bmpClient;
static SkRect           rect_bmpSurface;
static SkMatrix         matrix;

static
Surface* getNativeSurface(JNIEnv* env, jobject jsurface) {
	jclass clazz = env->FindClass("android/view/Surface");
	jfieldID field_surface = env->GetFieldID(clazz, "mSurface", "I");
	if(field_surface == NULL) {
		return NULL;
	}
	return (Surface *) env->GetIntField(jsurface, field_surface);
}

static
void initBitmap(SkBitmap *bitmap, int format, int width, int height) {
	switch (format) {
        case PIXEL_FORMAT_RGBA_8888:
            bitmap->setConfig(SkBitmap::kARGB_8888_Config,
                width, height);
            break;
			
        case PIXEL_FORMAT_RGBA_4444:
            bitmap->setConfig(SkBitmap::kARGB_4444_Config,
                width, height);
            break;
			
        case PIXEL_FORMAT_RGB_565:
            bitmap->setConfig(SkBitmap::kRGB_565_Config,
                width, height);
            break;
			
        case PIXEL_FORMAT_A_8:
            bitmap->setConfig(SkBitmap::kA8_Config,
                width, height);
            break;
			
        default:
            bitmap->setConfig(SkBitmap::kNo_Config,
                width, height);
            break;
    }
}

int AndroidSurface_register(JNIEnv* env, jobject jsurface) {
	__android_log_print(ANDROID_LOG_INFO, TAG, "registering video surface");
	
	surface = getNativeSurface(env, jsurface);
	if(surface == NULL) {
	     return ANDROID_SURFACE_RESULT_JNI_EXCEPTION;
	}
	
	__android_log_print(ANDROID_LOG_INFO, TAG, "registered");
	
	return ANDROID_SURFACE_RESULT_SUCCESS;
}

int AndroidSurface_getPixels(int width, int height, void** pixels) {
	__android_log_print(ANDROID_LOG_INFO, TAG, "getting surface's pixels %ix%i", width, height);
	if(surface == NULL) {
        return ANDROID_SURFACE_RESULT_JNI_EXCEPTION;
	}

    initBitmap(&bmpClient, PIXEL_FORMAT_RGB_565, width, height);
    bmpClient.setIsOpaque(true);
    //-- alloc array of pixels
    if(!bmpClient.allocPixels()) {
        return ANDROID_SURFACE_RESULT_COULDNT_INIT_BITMAP_CLIENT;
	}
    *pixels = bmpClient.getPixels();
	
    __android_log_print(ANDROID_LOG_INFO, TAG, "getted");
    return ANDROID_SURFACE_RESULT_SUCCESS;
}

static
void doUpdateSurface(bool autoscale) {
    SkCanvas canvas(bmpSurface);

    if(autoscale)
    {
        rect_bmpSurface.set(0, 0, bmpSurface.width(), bmpSurface.height());
        rect_bmpClient.set(0, 0, bmpClient.width(), bmpClient.height());
        matrix.setRectToRect(rect_bmpClient, rect_bmpSurface, SkMatrix::kFill_ScaleToFit);
        canvas.drawBitmapMatrix(bmpClient, matrix);
    }
    else
    {
        canvas.drawBitmap(bmpClient, 0, 0);
    }
}

int AndroidSurface_updateSurface(bool autoscale) {
    if(surface == NULL) {
        return ANDROID_SURFACE_RESULT_JNI_EXCEPTION;
    }
    if (!surface->isValid()) {
        return ANDROID_SURFACE_RESULT_NOT_VALID;
    }
    if (surface->lock(&surfaceInfo) < 0) {
        return ANDROID_SURFACE_RESULT_COULDNT_LOCK;
    }

    /* create surface bitmap with pixels of surface */
    if(bmpSurface.width() != surfaceInfo.w ||
        bmpSurface.height() != surfaceInfo.h)
    {
        initBitmap(&bmpSurface, surfaceInfo.format,
            surfaceInfo.w, surfaceInfo.h);
    }
    bmpSurface.setPixels(surfaceInfo.bits);

    /* redraw surface screen */
    doUpdateSurface(autoscale);
	
    if (surface->unlockAndPost() < 0) {
        return ANDROID_SURFACE_RESULT_COULDNT_UNLOCK_AND_POST;
    }
    return ANDROID_SURFACE_RESULT_SUCCESS;
}

int AndroidSurface_unregister() {
    __android_log_print(ANDROID_LOG_INFO, TAG, "unregistering video surface");
    __android_log_print(ANDROID_LOG_INFO, TAG, "unregistered");
    return ANDROID_SURFACE_RESULT_SUCCESS;
}
