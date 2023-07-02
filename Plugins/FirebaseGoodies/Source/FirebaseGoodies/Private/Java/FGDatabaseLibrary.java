// Copyright (c) 2022 Nineva Studios

package com.ninevastudios.unrealfirebase;

import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ServerValue;

import java.util.HashMap;

@SuppressWarnings("unused")
public class FGDatabaseLibrary {

	public static void GoOffline() {
		FirebaseDatabase.getInstance().goOffline();
	}

	public static void GoOnline() {
		FirebaseDatabase.getInstance().goOnline();
	}

	public static void PurgeOutstandingWrites() {
		FirebaseDatabase.getInstance().purgeOutstandingWrites();
	}

	public static void SetPersistenceCacheSizeBytes(long bytes) {
		FirebaseDatabase.getInstance().setPersistenceCacheSizeBytes(bytes);
	}

	public static void SetPersistenceEnabled(boolean isEnabled) {
		FirebaseDatabase.getInstance().setPersistenceEnabled(isEnabled);
	}

	public static HashMap<String, String> Timestamp() {
		return new HashMap<>(ServerValue.TIMESTAMP);
	}

}
