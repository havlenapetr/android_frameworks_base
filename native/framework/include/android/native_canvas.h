/*
 * Copyright (C) 2011 Havlena Petr, havlenapetr@gmail.com
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

#ifndef ANDROID_NATIVE_CANVAS_H
#define ANDROID_NATIVE_CANVAS_H

#define ACOLOR_BLACK   0xFF000000  //!< black SkColor value
#define ACOLOR_DKGRAY  0xFF444444  //!< dark gray SkColor value
#define ACOLOR_GRAY    0xFF888888  //!< gray SkColor value
#define ACOLOR_LTGRAY  0xFFCCCCCC  //!< light gray SkColor value
#define ACOLOR_WHITE   0xFFFFFFFF  //!< white SkColor value

#define ACOLOR_RED     0xFFFF0000  //!< red SkColor value
#define ACOLOR_GREEN   0xFF00FF00  //!< green SkColor value
#define ACOLOR_BLUE    0xFF0000FF  //!< blue SkColor value
#define ACOLOR_YELLOW  0xFFFFFF00  //!< yellow SkColor value
#define ACOLOR_CYAN    0xFF00FFFF  //!< cyan SkColor value
#define ACOLOR_MAGENTA 0xFFFF00FF  //!< magenta SkColor value

#ifdef __cplusplus
extern "C" {
#endif

struct ANativeCanvas;
typedef struct ANativeCanvas ANativeCanvas;

/*ANativeCanvas* ANativeCanvas_acquire(ANativeSurface* window, int w, int h);
    
void ANativeCanvas_release(ANativeCanvas* canvas);*/
    
int32_t ANativeCanvas_getWidth(ANativeCanvas* canvas);
    
int32_t ANativeCanvas_getHeight(ANativeCanvas* canvas);
        
    
int32_t ANativeCanvas_drawCircle(ANativeCanvas* canvas, int x, int y, int radius);
    
int32_t ANativeCanvas_drawRectange(ANativeCanvas* canvas, int x, int y, int w, int h);
    
int32_t ANativeCanvas_drawPolygon(ANativeCanvas* canvas, int* points, int points_size);
    
int32_t ANativeCanvas_drawText(ANativeCanvas* canvas, const void* text, size_t text_length,
                               int text_size, int x, int y, int dx, int dy);
    
int32_t ANativeCanvas_drawPolyline(ANativeCanvas* canvas, int* points, int points_size);
    
int32_t ANativeCanvas_draw(ANativeCanvas* canvas);

#ifdef __cplusplus
};
#endif

#endif // ANDROID_NATIVE_CANVAS_H
