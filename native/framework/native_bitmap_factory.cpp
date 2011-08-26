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

#define LOG_TAG "BitmapFactory"
#include <utils/Log.h>

#include <jni.h>

#include <android/native_bitmap_factory.h>

#include <SkBitmap.h>
#include <SkStream.h>
#include <SkImageDecoder.h>

using namespace android;

typedef struct ANativeBitmap {
    SkBitmap*       bitmap;
    char*           path;
    int             density;
} ANativeBitmap;

/*ANativeBitmap* ANativeBitmapFactory_fromBitmap(JNIEnv* env, jobject jbitmap) {
    jclass clazz_bitmap = env->FindClass("android/graphics/Bitmap");
	jfieldID field_bitmap = env->GetFieldID(clazz_bitmap, "mNativeBitmap", "I");
	if(field_bitmap == NULL) {
        LOGE("Can't obtain native bitmap pointer");
		return NULL;
	}
    
    ANativeBitmap* bitmap = (ANativeBitmap *) malloc(sizeof(ANativeBitmap));
    bitmap->bitmap = (SkBitmap *) env->GetIntField(jbitmap, field_bitmap);
    return bitmap;
}*/

SkBitmap* ANativeBitmapFactory_getSkBitmap(ANativeBitmap* bitmap) {
    return bitmap->bitmap;
}

int ANativeBitmapFactory_getDensity(ANativeBitmap* bitmap) {
    return bitmap->density;
}

int32_t ANativeBitmapFactory_isValid(ANativeBitmap* bitmap) {
    SkBitmap* skBitmap = bitmap->bitmap;
    return skBitmap->isNull() ? -1 : 0;
}

void ANativeBitmapFactory_release(ANativeBitmap* bitmap) {
    delete bitmap->bitmap;
    free(bitmap);
}

int32_t ANativeBitmapFactory_getWidth(ANativeBitmap* bitmap) {
    SkBitmap* skBitmap = bitmap->bitmap;
    return skBitmap->width();
}

int32_t ANativeBitmapFactory_getHeight(ANativeBitmap* bitmap) {
    SkBitmap* skBitmap = bitmap->bitmap;
    return skBitmap->height();
}

void* ANativeBitmapFactory_getPixels(ANativeBitmap* bitmap) {
    SkBitmap* skBitmap = bitmap->bitmap;
    return skBitmap->getPixels();
}

int32_t ANativeBitmapFactory_bytesPerPixel(ANativeBitmap* bitmap) {
    SkBitmap* skBitmap = bitmap->bitmap;
    return skBitmap->bytesPerPixel();
}

ANativeBitmap* ANativeBitmapFactory_loadFromFile(char* path) {
    SkImageDecoder*     decoder;
    SkFILEStream*       is;
    
    ANativeBitmap* bitmap = (ANativeBitmap *) malloc(sizeof(ANativeBitmap));
    memset(bitmap, 0, sizeof(ANativeBitmap));
    bitmap->density = -1;
    bitmap->bitmap = new SkBitmap;

    is = new SkFILEStream(path);
    if(!is->isValid()) {
        LOGE("SkStream isn't valid!");
        goto end;
    }
    
    decoder = SkImageDecoder::Factory(is);
    if(decoder == NULL) {
        LOGE("Can't obtain SkImageDecoder!");
        goto end;
    }

    if(decoder->decode(is, bitmap->bitmap, SkImageDecoder::kDecodePixels_Mode)) {
        bitmap->path = path;
    }
    
end:
    delete is;
    return bitmap;
}