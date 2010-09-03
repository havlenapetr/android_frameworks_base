/*
 * Copyright (C) 2008 The Android Open Source Project
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

package android.server;

import android.content.Context;
import android.os.ServiceManager;
import android.os.IBinder;
import android.net.IRemoteDesktop;
import android.util.Log;
import java.io.IOException;

/**
 * @hide
 */
public class VNCService extends IRemoteDesktop.Stub {
    private static final String TAG = "VNCService";
    private static final boolean DBG = true;
	
	public static final String RDESKTOP_SERVICE_NAME = "rdesktop";

    private int mNativeData;
    private final Context mContext;
	private boolean mEnabled;

    static {
        classInitNative();
    }

    public VNCService(Context context) throws IOException {
        mContext = context;
		mEnabled = false;
        if(!initializeNativeDataNative()) {
			throw new IOException("Couldn't init native data");
		}
    }
	
	public static IRemoteDesktop obtain(Context context) {
		IBinder b = ServiceManager.getService(RDESKTOP_SERVICE_NAME);
		if (b != null) {
			return IRemoteDesktop.Stub.asInterface(b);
		}
		return null;
	}

    @Override
    protected void finalize() throws Throwable {
		if (mEnabled) {
			disable();
		}
        try {
            cleanupNativeDataNative();
        } finally {
            super.finalize();
        }
    }
	
	public boolean isEnabled() {
		return mEnabled;
	}

    /**
     * Bring down VNC server. Returns true on success.
     */
    public boolean disable() {
		if (!disableNative()) {
			return false;
		}
		mEnabled = false;
		return true;
    }

    /**
	 * Bring up VNC server. Returns true on success.
	 */
    public boolean enable() {
		if (!enableNative()) {
			return false;
		}
		mEnabled = true;
		return true;
    }
	
	private native static void classInitNative();
    private native boolean initializeNativeDataNative();
	private native void cleanupNativeDataNative();
	
	private native boolean enableNative();
    private native boolean disableNative();
}
