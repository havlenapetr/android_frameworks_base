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

#define LOG_TAG "ACamera"
#include <utils/Log.h>

#include <camera/Camera.h>

#include <jni.h>

#define CHECK(val) \
    if(!val) { \
        LOGE("%s [%i]: NULL pointer exception!", __func__, __LINE__); \
        return -1; \
    }

using namespace android;

typedef struct ACamera {
    /* our private members here */
    sp<Camera> camera;
} ACamera;

// helper function to extract a native Camera object from a Camera Java object
extern sp<Camera> get_native_camera(JNIEnv *env, jobject jcamera, struct JNICameraContext** context);

int ACamera_init(JNIEnv *env, jobject jcamera, int sdkVersion, ACamera** aCamera) {
    sp<Camera> camera = get_native_camera(env, jcamera, NULL);
    if(camera == NULL) {
        LOGE("Cant' obtain native camera!");
        return -1;
    }

    *aCamera = (ACamera *) malloc(sizeof(ACamera));
    (*aCamera)->camera = camera;
    return 0;
}

int ACamera_deinit(ACamera** aCamera) {
    if((*aCamera) && (*aCamera)->camera != NULL)
        (*aCamera)->camera->disconnect();
    if((*aCamera))
        free(*aCamera);
    *aCamera = NULL;
    return 0;
}
