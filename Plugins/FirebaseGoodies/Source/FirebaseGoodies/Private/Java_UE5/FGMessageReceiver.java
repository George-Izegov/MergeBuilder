package com.ninevastudios.unrealfirebase;

import android.content.SharedPreferences;
import android.util.Log;

import com.epicgames.unreal.GameActivity;
import com.google.firebase.BuildConfig;
import com.google.firebase.messaging.FirebaseMessagingService;
import com.google.firebase.messaging.RemoteMessage;

public class FGMessageReceiver extends FirebaseMessagingService {

	public static final String TAG = FGMessageReceiver.class.getSimpleName();
	private final String PREF_NAME = TAG;
	private final String PREF_FIELD_NAME = "HasLaunchedBefore";

	public static native void OnMessageReceived(RemoteMessage message);

	public static native void OnDeletedMessages();

	public static native void OnMessageSent(String msgId);

	public static native void OnSendError(String msgId, String error);

	public static native void OnNewToken(String token);

	public void onMessageReceived(RemoteMessage message) {
		OnMessageReceived(message);
	}

	public void onDeletedMessages() {
		OnDeletedMessages();
	}

	public void onMessageSent(String msgId) {
		OnMessageSent(msgId);
	}

	public void onSendError(String msgId, Exception exception) {
		OnSendError(msgId, exception.getMessage());
	}

	public void onNewToken(String token) {
		if (BuildConfig.DEBUG) {
			Log.d(TAG, "Token received: " + token);
		}

		if (GameActivity.Get() == null || !GameActivity.Get().ninevaIsResumed()) {
			Log.w(TAG, "[onNewToken()] GameActivity is is null or not resumed, returning...");
			return;
		}

		if (!hasLaunchedBefore()) {
			saveLaunchedState();
			Log.w(TAG,
					"[onNewToken()] Not invoking onNewToken on first application launch, " +
							"because the JNI modules are not loaded yet. This would result in a crash.");
			return;
		}

		OnNewToken(token);
	}

	private boolean hasLaunchedBefore() {
		SharedPreferences sharedPreferences = getSharedPreferences(PREF_NAME, MODE_PRIVATE);
		return sharedPreferences.getBoolean(PREF_FIELD_NAME, false);
	}

	private void saveLaunchedState() {
		SharedPreferences sharedPreferences = getSharedPreferences(PREF_NAME, MODE_PRIVATE);
		SharedPreferences.Editor myEdit = sharedPreferences.edit();
		myEdit.putBoolean(PREF_FIELD_NAME, true);
		myEdit.apply();
	}
}
