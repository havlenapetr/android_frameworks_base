/*
** Copyright 2006, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License"); 
** you may not use this file except in compliance with the License. 
** You may obtain a copy of the License at 
**
**     http://www.apache.org/licenses/LICENSE-2.0 
**
** Unless required by applicable law or agreed to in writing, software 
** distributed under the License is distributed on an "AS IS" BASIS, 
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
** See the License for the specific language governing permissions and 
** limitations under the License.
*/
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <android_runtime/AndroidRuntime.h>
#include <JNIHelp.h>
#include <jni.h>
#include <utils/Log.h>
#include <utils/misc.h>

#include <cutils/properties.h>

#include <dbus/dbus.h>

#define VNC_ADAPTER "org.vnc"
#define SERVER_INTERFACE "org.vnc.server"

#define TAG "VNCService.cpp"

namespace android {
	
// LOGE and free a D-Bus error
// Using #define so that __FUNCTION__ resolves usefully
#define LOG_AND_FREE_DBUS_ERROR_WITH_MSG(err, msg) \
{   LOGE("%s: D-Bus error in %s: %s (%s)", __FUNCTION__, \
dbus_message_get_member((msg)), (err)->name, (err)->message); \
dbus_error_free((err)); }
#define LOG_AND_FREE_DBUS_ERROR(err) \
{   LOGE("%s: D-Bus error: %s (%s)", __FUNCTION__, \
(err)->name, (err)->message); \
dbus_error_free((err)); }
	
// We initialize these variables when we load class
static jfieldID field_mNativeData;

typedef struct {
    JNIEnv *env;
    DBusConnection *conn;
    const char *adapter;  // dbus object name of the local adapter
} native_data_t;
	
jfieldID get_field(JNIEnv *env, jclass clazz, const char *member, const char *mtype);

/** Get native data stored in the opaque (Java code maintained) pointer mNativeData
 *  Perform quick sanity check, if there are any problems return NULL
 */
static inline native_data_t * get_native_data(JNIEnv *env, jobject object) {
    native_data_t *nat =
            (native_data_t *)(env->GetIntField(object, field_mNativeData));
    if (nat == NULL || nat->conn == NULL) {
        LOGE("Uninitialized native data\n");
        return NULL;
    }
    return nat;
}

static void classInitNative(JNIEnv* env, jclass clazz) {
    LOGV(__FUNCTION__);
    field_mNativeData = get_field(env, clazz, "mNativeData", "I");
}

/* Returns true on success (even if adapter is present but disabled).
 * Return false if dbus is down, or another serious error (out of memory)
*/
static jboolean initializeNativeDataNative(JNIEnv* env, jobject object) {
    LOGV(__FUNCTION__);
	
    native_data_t *nat = (native_data_t *)calloc(1, sizeof(native_data_t));
    if (NULL == nat) {
        LOGE("%s: out of memory!", __FUNCTION__);
        return false;
    }
    nat->env = env;

    env->SetIntField(object, field_mNativeData, (jint)nat);
    DBusError err;
    dbus_error_init(&err);
    dbus_threads_init_default();
    nat->conn = dbus_bus_get(DBUS_BUS_SYSTEM, &err);
    if (dbus_error_is_set(&err)) {
        LOGE("Could not get onto the system bus: %s", err.message);
        dbus_error_free(&err);
        return false;
    }
    dbus_connection_set_exit_on_disconnect(nat->conn, FALSE);
    return true;
}

static void cleanupNativeDataNative(JNIEnv* env, jobject object) {
    LOGV(__FUNCTION__);
	
    native_data_t *nat =
        (native_data_t *)env->GetIntField(object, field_mNativeData);
    if (nat) {
        free(nat);
        nat = NULL;
    }
}

static jboolean enableNative(JNIEnv *env, jobject object) {
	LOGV(__FUNCTION__);
    DBusMessage *msg = NULL;
    DBusMessage *reply = NULL;
    DBusError err;
    const char *str_error;
    jboolean ret = JNI_FALSE;
	int attempt = 0;
	
    native_data_t *nat = get_native_data(env, object);
    if (nat == NULL) {
        goto done;
    }
	
	dbus_error_init(&err);
	/* Compose the command */
	msg = dbus_message_new_method_call(VNC_ADAPTER, "/",
									   SERVER_INTERFACE, "Enable");
	if (msg == NULL) {
		if (!dbus_error_is_set(&err) && msg != NULL) {
			LOG_AND_FREE_DBUS_ERROR(&err);
		}
		goto done;
	}
	
	// start vncserver daemon
	if (property_set("ctl.start", "vncserverd") < 0) {
        LOGE("Failed to start vncserverd");
        return JNI_FALSE;
    }
	
	for (attempt = 1000; attempt > 0;  attempt--) {
		/* Send the command. */
		reply = dbus_connection_send_with_reply_and_block(nat->conn, msg, -1, &err);
		if (reply) {
			break;
		}
		dbus_error_free(&err);
		dbus_error_init(&err);
		usleep(10000);  // 10 ms retry delay
	}
	if (attempt == 0) {
		if (dbus_error_is_set(&err)) {
			LOG_AND_FREE_DBUS_ERROR(&err);
		}
        goto done;
    }
	
	if (!dbus_message_get_args(reply, 
							   &err, 
							   DBUS_TYPE_BOOLEAN, &ret,
							   DBUS_TYPE_STRING, &str_error,
							   DBUS_TYPE_INVALID)){
		if (dbus_error_is_set(&err)) {
			LOG_AND_FREE_DBUS_ERROR(&err);
		}
		goto done;
	}
	
done:
    if (reply) dbus_message_unref(reply);
    if (msg) dbus_message_unref(msg);
    return ret;
}

static jboolean disableNative(JNIEnv *env, jobject object) {
	LOGV(__FUNCTION__);
    DBusMessage *msg = NULL;
    DBusMessage *reply = NULL;
    DBusError err;
    jboolean ret = JNI_FALSE;
	
    native_data_t *nat = get_native_data(env, object);
    if (nat == NULL) {
        goto done;
    }
	
    dbus_error_init(&err);
	
    /* Compose the command */
    msg = dbus_message_new_method_call(VNC_ADAPTER, "/",
									   SERVER_INTERFACE, "Disable");
	
    if (msg == NULL) {
        if (dbus_error_is_set(&err)) {
            LOG_AND_FREE_DBUS_ERROR_WITH_MSG(&err, msg);
        }
        goto done;
    }
	
    /* Send the command. */
    reply = dbus_connection_send_with_reply_and_block(nat->conn, msg, -1, &err);
    if (dbus_error_is_set(&err)) {
		LOG_AND_FREE_DBUS_ERROR_WITH_MSG(&err, msg);
		goto done;
    }
	
	// stop vncserver daemon
	if (property_set("ctl.stop", "vncserverd") < 0) {
        LOGE("Error stopping vncserverd");
        goto done;
    }
	
    ret = JNI_TRUE;
done:
    if (reply) dbus_message_unref(reply);
    if (msg) dbus_message_unref(msg);
    return ret;
}

static JNINativeMethod sMethods[] = {
     /* name, signature, funcPtr */
    {"classInitNative", "()V", (void*)classInitNative},
    {"initializeNativeDataNative", "()Z", (void *)initializeNativeDataNative},
    {"cleanupNativeDataNative", "()V", (void *)cleanupNativeDataNative},

    {"enableNative", "()Z", (void *)enableNative},
    {"disableNative", "()Z", (void *)disableNative},
};

int register_android_server_VNCService(JNIEnv *env) {
    return AndroidRuntime::registerNativeMethods(env,
                "android/server/VNCService", sMethods, NELEM(sMethods));
}

} /* namespace android */
