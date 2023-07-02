package com.ninevastudios.unrealfirebase;

import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.Task;
import com.google.firebase.crashlytics.FirebaseCrashlytics;

@SuppressWarnings("unused")
public class FGCrashlytics {

	public static native void OnCheckForUnsentUserReportsSuccess(boolean present);
	public static native void OnCheckForUnsentUserReportsError(String error);

	public static void recordException(String message) {
		FirebaseCrashlytics.getInstance().recordException(new Throwable(message));
	}

	public static void log(String message) {

		FirebaseCrashlytics.getInstance().log(message);
	}

	public static void setUserId(String identifier) {
		FirebaseCrashlytics.getInstance().setUserId(identifier);
	}

	public static void setCustomKey(String key, boolean value) {
		FirebaseCrashlytics.getInstance().setCustomKey(key, Boolean.toString(value));
	}

	public static void setCustomKey(String key, double value) {
		FirebaseCrashlytics.getInstance().setCustomKey(key, Double.toString(value));
	}

	public static void setCustomKey(String key, float value) {
		FirebaseCrashlytics.getInstance().setCustomKey(key, Float.toString(value));
	}

	public static void setCustomKey(String key, int value) {
		FirebaseCrashlytics.getInstance().setCustomKey(key, Integer.toString(value));
	}

	public static void setCustomKey(String key, long value) {
		FirebaseCrashlytics.getInstance().setCustomKey(key, Long.toString(value));
	}

	public static void setCustomKey(String key, String value) {
		FirebaseCrashlytics.getInstance().setCustomKey(key, value);
	}

	public static void checkForUnsentReports() {
		FirebaseCrashlytics.getInstance().checkForUnsentReports().addOnCompleteListener(new OnCompleteListener<Boolean>() {
			@Override
			public void onComplete(Task<Boolean> task) {
				if (task.isSuccessful()) {
					OnCheckForUnsentUserReportsSuccess(task.getResult());
				} else {
					OnCheckForUnsentUserReportsError(task.getException().toString());
				}
			}
		});
	}

	public static void sendUnsentReports() {
		FirebaseCrashlytics.getInstance().sendUnsentReports();
	}

	public static void deleteUnsentReports() {
		FirebaseCrashlytics.getInstance().deleteUnsentReports();
	}

	public static boolean didCrashOnPreviousExecution() {
		return FirebaseCrashlytics.getInstance().didCrashOnPreviousExecution();
	}

	public static void setCrashlyticsCollectionEnabled(boolean enabled) {
		FirebaseCrashlytics.getInstance().setCrashlyticsCollectionEnabled(enabled);
	}
}
