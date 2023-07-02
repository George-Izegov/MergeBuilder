package com.ninevastudios.unrealfirebase;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.google.firebase.database.ChildEventListener;
import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;

public class FGDatabaseChildEventListener implements ChildEventListener {

    private static final int CHILD_EVENT_ADDED = 0;
    private static final int CHILD_EVENT_CHANGED = 1;
    private static final int CHILD_EVENT_REMOVED = 2;
    private static final int CHILD_EVENT_MOVED = 3;

    public static native void onChildEventCallback(long callbackAddr, int eventType, FGDataSnapshot data, String prevChildName);

    public static native void onChildCancelledCallback(long callbackAddr, int errorCode, String errorMessage);

    private long mCallbackAddr;

    public FGDatabaseChildEventListener(long callbackAddr) {
        mCallbackAddr = callbackAddr;
    }

    @Override
    public void onChildAdded(@NonNull DataSnapshot snapshot, @Nullable String previousChildName) {
        FGDataSnapshot data = new FGDataSnapshot(snapshot);
        onChildEventCallback(mCallbackAddr, CHILD_EVENT_ADDED, data, previousChildName);
    }

    @Override
    public void onChildChanged(@NonNull DataSnapshot snapshot, @Nullable String previousChildName) {
        FGDataSnapshot data = new FGDataSnapshot(snapshot);
        onChildEventCallback(mCallbackAddr, CHILD_EVENT_CHANGED, data, previousChildName);
    }

    @Override
    public void onChildRemoved(@NonNull DataSnapshot snapshot) {
        FGDataSnapshot data = new FGDataSnapshot(snapshot);
        onChildEventCallback(mCallbackAddr, CHILD_EVENT_REMOVED, data, "");
    }

    @Override
    public void onChildMoved(@NonNull DataSnapshot snapshot, @Nullable String previousChildName) {
        FGDataSnapshot data = new FGDataSnapshot(snapshot);
        onChildEventCallback(mCallbackAddr, CHILD_EVENT_MOVED, data, previousChildName);
    }

    @Override
    public void onCancelled(@NonNull DatabaseError error) {
        onChildCancelledCallback(mCallbackAddr, error.getCode(), error.getMessage());
    }
}
