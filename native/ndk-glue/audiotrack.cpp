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

#define LOG_TAG "AAudioTrack"
#include <utils/Log.h>

#include <media/AudioTrack.h>
#include <media/AudioSystem.h>

#define TAG "AudioTrackWrapper"

#define CHECK(val) \
    if(!val) { \
        LOGE("%s [%i]: NULL pointer exception!", __func__, __LINE__); \
        return -1; \
    }

using namespace android;

typedef struct AAudioTrack {
    /* our private members here */
    AudioTrack* audioTrack;
} AAudioTrack;

static AudioTrack* getNativeAudioTrack(JNIEnv* env, jobject jaudioTrack, int sdkVersion) {
    /* we know jsurface is a valid local ref, so use it */
    jclass clazz = env->GetObjectClass(jaudioTrack);
    if(clazz == NULL) {
        LOGE("Can't find audio track class!");
        return NULL;
    }

    jfieldID field_track = env->GetFieldID(clazz,
                                             "mNativeTrackInJavaObj",
                                             "I");
    if(field_track == NULL) {
        LOGE("Can't find native audio track field!");
        return NULL;
    }
    return (Surface *) env->GetIntField(jaudioTrack, field_surface);
}

int AAudioTrack_init(JNIEnv* env, jobject jaudioTrack, int sdkVersion, AAudioTrack** aAudioTrack) {
    if(!env || jaudioTrack == NULL) {
        LOGE("JNIEnv or jaudioTrack obj is NULL!");
        return -1;
    }

    AudioTrack* audioTrack = getNativeAudioTrack(env, jsurface, sdkVersion);
    if(!audioTrack) {
        LOGE("Can't obtain native audio track!");
        return -1;
    }

    *aAudioTrack = (AAudioTrack *) malloc(sizeof(AAudioTrack));
    (*aAudioTrack)->audioTrack = audioTrack;
    return 0;
}

void AAudioTrack_deinit(AAudioTrack** aAudioTrack) {
    free(*aAudioTrack);
    *aAudioTrack = NULL;
}

void AAudioTrack_start(AAudioTrack* aAudioTrack) {
    aAudioTrack->audioTrack->start();
}

void AAudioTrack_stop(AAudioTrack* aAudioTrack) {
    aAudioTrack->audioTrack->flush();
    aAudioTrack->audioTrack->stop();
}
