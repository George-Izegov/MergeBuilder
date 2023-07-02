// Copyright (c) 2022 Nineva Studios

package com.ninevastudios.unrealfirebase;

import android.app.Activity;
import android.util.Log;

import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.Task;

import com.google.firebase.remoteconfig.FirebaseRemoteConfig;
import com.google.firebase.remoteconfig.FirebaseRemoteConfigSettings;

import java.util.Map;

@SuppressWarnings("unused")
public class FGRemoteConfig {

	public static native void onSuccessCallback(long callbackAddr);
	public static native void onErrorCallback(long callbackAddr, String errorMessage);

	public static void SetConfigSettings(long minFetchInterval, long fetchTimeout) {
		FirebaseRemoteConfig mFirebaseRemoteConfig = FirebaseRemoteConfig.getInstance();
		FirebaseRemoteConfigSettings configSettings = new FirebaseRemoteConfigSettings.Builder()
				.setMinimumFetchIntervalInSeconds(minFetchInterval)
				.setFetchTimeoutInSeconds(fetchTimeout)
				.build();
		mFirebaseRemoteConfig.setConfigSettingsAsync(configSettings);
	}

	public static void Fetch(final Activity activity, final long callback) {
		final FirebaseRemoteConfig mFirebaseRemoteConfig = FirebaseRemoteConfig.getInstance();
		mFirebaseRemoteConfig.fetch().addOnCompleteListener(activity, new OnCompleteListener<Void>() {
			@Override
			public void onComplete(Task<Void> task) {
				if (task.isSuccessful()) {
					Log.d("FirebaseGoodies", "Fetch completed successfully!");
					onSuccessCallback(callback);
				} else {
					Log.e("FirebaseGoodies", "Failed to complete Fetch!");
					Log.e("FirebaseGoodies", task.getException().getMessage());
					onErrorCallback(callback, task.getException().getMessage());
				}
			}
		});
	}

	public static void FetchWithInterval(final Activity activity, long timeInterval, final long callback) {
		final FirebaseRemoteConfig mFirebaseRemoteConfig = FirebaseRemoteConfig.getInstance();
		mFirebaseRemoteConfig.fetch(timeInterval).addOnCompleteListener(activity, new OnCompleteListener<Void>() {
			@Override
			public void onComplete(Task<Void> task) {
				if (task.isSuccessful()) {
					Log.d("FirebaseGoodies", "FetchWithInterval completed successfully!");
					onSuccessCallback(callback);
				} else {
					Log.e("FirebaseGoodies", "Failed to complete FetchWithInterval!");
					Log.e("FirebaseGoodies", task.getException().getMessage());
					onErrorCallback(callback, task.getException().getMessage());
				}
			}
		});
	}

	public static void FetchAndActivate(final Activity activity, final long callback) {
		final FirebaseRemoteConfig mFirebaseRemoteConfig = FirebaseRemoteConfig.getInstance();
		mFirebaseRemoteConfig.fetchAndActivate().addOnCompleteListener(activity, new OnCompleteListener<Boolean>() {
			@Override
			public void onComplete(Task<Boolean> task) {
				if (task.isSuccessful()) {
					Log.d("FirebaseGoodies", "FetchAndActivate completed successfully!");
					onSuccessCallback(callback);
				} else {
					Log.e("FirebaseGoodies", "Failed to complete FetchAndActivate!");
					Log.e("FirebaseGoodies", task.getException().getMessage());
					onErrorCallback(callback, task.getException().getMessage());
				}
			}
		});
	}

	public static void Activate(final Activity activity, final long callback) {
		final FirebaseRemoteConfig mFirebaseRemoteConfig = FirebaseRemoteConfig.getInstance();
		mFirebaseRemoteConfig.activate().addOnCompleteListener(activity, new OnCompleteListener<Boolean>() {
			@Override
			public void onComplete(Task<Boolean> task) {
				if (task.isSuccessful()) {
					Log.d("FirebaseGoodies", "Activate completed successfully!");
					onSuccessCallback(callback);
				} else {
					Log.e("FirebaseGoodies", "Failed to complete Activate!");
					Log.e("FirebaseGoodies", task.getException().getMessage());
					onErrorCallback(callback, task.getException().getMessage());
				}
			}
		});
	}

	public static boolean GetBoolean(String key) {
		FirebaseRemoteConfig mFirebaseRemoteConfig = FirebaseRemoteConfig.getInstance();
		return mFirebaseRemoteConfig.getBoolean(key);
	}

	public static double GetDouble(String key) {
		FirebaseRemoteConfig mFirebaseRemoteConfig = FirebaseRemoteConfig.getInstance();
		return mFirebaseRemoteConfig.getDouble(key);
	}

	public static long GetLong(String key) {
		FirebaseRemoteConfig mFirebaseRemoteConfig = FirebaseRemoteConfig.getInstance();
		return mFirebaseRemoteConfig.getLong(key);
	}

	public static String GetString(String key) {
		FirebaseRemoteConfig mFirebaseRemoteConfig = FirebaseRemoteConfig.getInstance();
		return mFirebaseRemoteConfig.getString(key);
	}

	public static void SetDefaults(final Activity activity, final long callback,
								   Map<String, Object> values) {
		FirebaseRemoteConfig.getInstance().setDefaultsAsync(values).addOnCompleteListener(new OnCompleteListener<Void>() {
			@Override
			public void onComplete(Task<Void> task) {
				if (task.isSuccessful()) {
					Log.d("FirebaseGoodies", "Activate completed successfully!");
					onSuccessCallback(callback);
				} else {
					Log.e("FirebaseGoodies", "Failed to complete Activate!");
					Log.e("FirebaseGoodies", task.getException().getMessage());
					onErrorCallback(callback, task.getException().getMessage());
				}
			}
		});
	}
}