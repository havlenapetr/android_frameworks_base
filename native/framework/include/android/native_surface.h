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

#ifndef ANDROID_NATIVE_surface_H
#define ANDROID_NATIVE_surface_H

#include <android/rect.h>
#include <android/native_canvas.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Pixel formats that a surface can use.
 */
enum {
    surface_FORMAT_RGBA_8888          = 1,
    surface_FORMAT_RGBX_8888          = 2,
    surface_FORMAT_RGB_565            = 4,
};
    
struct ANativeSurface;
typedef struct ANativeSurface ANativeSurface;

typedef struct ANativeSurface_Buffer {
    // The number of pixels that are show horizontally.
    int32_t width;

    // The number of pixels that are shown vertically.
    int32_t height;

    // The number of *pixels* that a line in the buffer takes in
    // memory.  This may be >= width.
    int32_t stride;

    // The format of the buffer.  One of surface_FORMAT_*
    int32_t format;

    // The actual bits.
    void* bits;
    
    // Do not touch.
    uint32_t reserved[6];
} ANativeSurface_Buffer;
    
ANativeCanvas* ANativeSurface_lockCanvas(ANativeSurface* surface, ARect* inOutDirtyBounds);

int32_t ANativeSurface_unlockCanvasAndPost(ANativeSurface* surface);

/**
 * Lock the surface's next drawing surface for writing.
 */
int32_t ANativeSurface_lock(ANativeSurface* surface, ANativeSurface_Buffer* outBuffer,
        ARect* inOutDirtyBounds);

/**
 * Unlock the surface's drawing surface after previously locking it,
 * posting the new buffer to the display.
 */
int32_t ANativeSurface_unlockAndPost(ANativeSurface* surface);

#ifdef __cplusplus
};
#endif

#endif // ANDROID_NATIVE_surface_H