package com.ninevastudios.unrealfirebase;

import androidx.annotation.NonNull;

import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.ValueEventListener;

public class FGDatabaseValueEventListener implements ValueEventListener {

    public static native void onDataChangedCallback(long callbackAddr, FGDataSnapshot data);

    public static native void onCancelledCallback(long callbackAddr, int errorCode, String errorMessage);

    private long mCallbackAddr;

    public FGDatabaseValueEventListener(long callbackAddr) {
        mCallbackAddr = callbackAddr;
    }

    @Override
    public void onDataChange(@NonNull DataSnapshot snapshot) {
        FGDataSnapshot data = new FGDataSnapshot(snapshot);
        onDataChangedCallback(mCallbackAddr, data);
    }

    @Override
    public void onCancelled(@NonNull DatabaseError error) {
        onCancelledCallback(mCallbackAddr, error.getCode(), error.getMessage());
    }
}
