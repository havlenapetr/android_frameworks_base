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

#include <android/audiotrack.h>
#include <utils/Log.h>
#include <media/AudioTrack.h>
#include <media/AudioSystem.h>
#include <utils/Errors.h>

#include <binder/MemoryHeapBase.h>
#include <binder/MemoryBase.h>

#define TAG "AudioTrackWrapper"

using namespace android;

//struct audiotrack_fields_t {
static AudioTrack*						track;
//sp<MemoryHeapBase>					memHeap;
//sp<MemoryBase>						memBase;
//};
//static struct audiotrack_fields_t audio;

static AudioTrack* getNativeAudioTrack(JNIEnv* env, jobject jaudioTrack) {
	jclass clazz = env->FindClass("android/media/AudioTrack");
	jfieldID field_track = env->GetFieldID(clazz, "mNativeTrackInJavaObj", "I");
	if(field_track == NULL) {
		return NULL;
	}
	return (AudioTrack *) env->GetIntField(jaudioTrack, field_track);
}

/*
static bool allocSharedMem(int sizeInBytes) {
	memHeap = new MemoryHeapBase(sizeInBytes);
	if (memHeap->getHeapID() < 0) {
		return false;
	}
	memBase = new MemoryBase(memHeap, 0, sizeInBytes);
	return true;
}
*/

int AndroidAudioTrack_register() {
	__android_log_print(ANDROID_LOG_INFO, TAG, "registering audio track");
	track = new AudioTrack();
	if(track == NULL) {
	     return ANDROID_AUDIOTRACK_RESULT_JNI_EXCEPTION;
	}
	__android_log_print(ANDROID_LOG_INFO, TAG, "registered");
	return ANDROID_AUDIOTRACK_RESULT_SUCCESS;
}

int AndroidAudioTrack_start() {
	//__android_log_print(ANDROID_LOG_INFO, TAG, "starting audio track");
    if(track == NULL) {
        return ANDROID_AUDIOTRACK_RESULT_ALLOCATION_FAILED;
    }
	track->start();
    return ANDROID_AUDIOTRACK_RESULT_SUCCESS;
}

int AndroidAudioTrack_set(int streamType,
						  uint32_t sampleRate,
						  int format,
						  int channels) {
	if(track == NULL) {
        return ANDROID_AUDIOTRACK_RESULT_ALLOCATION_FAILED;
    }
	
	__android_log_print(ANDROID_LOG_INFO, TAG, "setting audio track");
	
	status_t ret = track->set(streamType, 
							  sampleRate, 
							  format, 
							  channels, 
							  0, 
							  0,
							  0, 
							  0,
							  false);
	
	if (ret != NO_ERROR) {
		return ANDROID_AUDIOTRACK_RESULT_ERRNO;
	}
	return ANDROID_AUDIOTRACK_RESULT_SUCCESS;
}

int AndroidAudioTrack_flush() {
    if(track == NULL) {
        return ANDROID_AUDIOTRACK_RESULT_ALLOCATION_FAILED;
    }
	track->flush();
    return ANDROID_AUDIOTRACK_RESULT_SUCCESS;
}

int AndroidAudioTrack_stop() {
    if(track == NULL) {
        return ANDROID_AUDIOTRACK_RESULT_ALLOCATION_FAILED;
    }
	track->stop();
    return ANDROID_AUDIOTRACK_RESULT_SUCCESS;
}

int AndroidAudioTrack_reload() {
    if(track == NULL) {
        return ANDROID_AUDIOTRACK_RESULT_ALLOCATION_FAILED;
    }
	if(track->reload() != NO_ERROR) {
		return ANDROID_AUDIOTRACK_RESULT_ERRNO;
	}
    return ANDROID_AUDIOTRACK_RESULT_SUCCESS;
}

int AndroidAudioTrack_unregister() {
	__android_log_print(ANDROID_LOG_INFO, TAG, "unregistering audio track");
    if(!track->stopped()) {
        track->stop();
    }
	//memBase.clear();
	//memHeap.clear();
	free(track);
	//track = NULL;
	__android_log_print(ANDROID_LOG_INFO, TAG, "unregistered");
    return ANDROID_AUDIOTRACK_RESULT_SUCCESS;
}

int AndroidAudioTrack_write(void *buffer, int buffer_size) {
	// give the data to the native AudioTrack object (the data starts at the offset)
    ssize_t written = 0;
    // regular write() or copy the data to the AudioTrack's shared memory?
    if (track->sharedBuffer() == 0) {
        written = track->write(buffer, buffer_size);
    } else {
		// writing to shared memory, check for capacity
		if ((size_t)buffer_size > track->sharedBuffer()->size()) {
			__android_log_print(ANDROID_LOG_INFO, TAG, "buffer size was too small");
			buffer_size = track->sharedBuffer()->size();
		}
		memcpy(track->sharedBuffer()->pointer(), buffer, buffer_size);
		written = buffer_size;
	}
	return written;
}
