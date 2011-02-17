package com.android.internal.telephony;

import android.util.Log;

public class SimLockInfoResult {

    private static final String LOG_TAG = "SimLockInfoResult";
    private static final boolean D = true;
/*
    private static final int LOCK_ACL = 11;
    private static final int LOCK_CORP_PERS = 8;
    private static final int LOCK_FD = 4;
    private static final int LOCK_NETWORK_PERS = 5;
    private static final int LOCK_NETWORK_SUBSET_PERS = 6;
    private static final int LOCK_PH_FSIM = 2;
    private static final int LOCK_PH_SIM = 1;
    private static final int LOCK_SP_PERS = 7;
*/
    private static final int LOCK_PIN2 = 9;
    private static final int LOCK_PUK2 = 10;
    private static final int LOCK_READY = 0;
    private static final int LOCK_SIM = 3;
    
    private static final int NOT_NEED = 0;
    private static final int NO_SIM = 128;
    private static final int PERM_BLOCKED = 5;
    private static final int PIN = 1;
    private static final int PIN2 = 3;
    private static final int PIN2_DISABLE = 6;
    private static final int PUK = 2;
    private static final int PUK2 = 4;
    
    private int mPin2Retry;
    private int mPinRetry;
    private int mPuk2Retry;
    private int mPukRetry;
    private int mLockKey;
    private int mLockType;
    private int mNumRetry;
    private int mNumLockType;


    public SimLockInfoResult(int numLockType, int lockType, int lockKey, int numRetry) {
        mNumLockType = numLockType;
        mLockType = lockType;
        mLockKey = lockKey;
        mNumRetry = numRetry;
        if (lockType == LOCK_PUK2)
            mPuk2Retry = numRetry;
        switch (lockKey) {
        case NOT_NEED:
            mPinRetry = numRetry;
            if(D) Log.i(LOG_TAG, "NOT_NEED numRetry: " + mPinRetry);
            break;
        case PIN:
            mPinRetry = numRetry;
            if(D) Log.i(LOG_TAG, "PIN numRetry: " + mPinRetry);
            break;
        case PUK:
        case PERM_BLOCKED:
            mPukRetry = numRetry;
            if(D) Log.i(LOG_TAG, "PUK numRetry: " + mPukRetry);
            break;
        case PIN2:
        case PIN2_DISABLE:
            mPin2Retry = numRetry;
            if(D) Log.i(LOG_TAG, "PIN2 numRetry: " + mPin2Retry);
            break;
        case PUK2:
            mPuk2Retry = numRetry;
            if(D) Log.i(LOG_TAG, "PUK2 numRetry: " + mPuk2Retry);
        }
    }

    public int getLockKey() {
        return mLockKey;
    }

    public int getPin2Retry() {
        return mPin2Retry;
    }

    public int getPinRetry() {
        return mPinRetry;
    }

    public int getPuk2Retry() {
        return mPuk2Retry;
    }

    public int getPukRetry() {
        return mPukRetry;
    }

    public String toString() {
        return "num:" + mNumLockType + " lock_type:" + mLockType + 
            " lock_key:" + mLockKey + " num_of_retry:" + mNumRetry;
    }
}
