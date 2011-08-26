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

#ifndef ANDROID_NATIVE_bitmap_factory_H
#define ANDROID_NATIVE_bitmap_factory_H

#ifdef __cplusplus
extern "C" {
#endif
    
struct ANativeBitmap;
typedef struct ANativeBitmap ANativeBitmap;

ANativeBitmap* ANativeBitmapFactory_loadFromFile(char* path);
    
void* ANativeBitmapFactory_getPixels(ANativeBitmap* bitmap);

int32_t ANativeBitmapFactory_bytesPerPixel(ANativeBitmap* bitmap);

void ANativeBitmapFactory_release(ANativeBitmap* bitmap);
    
int32_t ANativeBitmapFactory_isValid(ANativeBitmap* bitmap);
    
int32_t ANativeBitmapFactory_getWidth(ANativeBitmap* bitmap);
    
int32_t ANativeBitmapFactory_getHeight(ANativeBitmap* bitmap);

#ifdef __cplusplus
};
#endif

#endif // ANDROID_NATIVE_bitmap_factory_H