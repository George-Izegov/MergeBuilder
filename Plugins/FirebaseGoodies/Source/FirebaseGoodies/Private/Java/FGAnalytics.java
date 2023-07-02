// Copyright (c) 2022 Nineva Studios

package com.ninevastudios.unrealfirebase;

import android.app.Activity;
import android.os.Bundle;

import com.google.firebase.analytics.FirebaseAnalytics;

@SuppressWarnings("unused")
public class FGAnalytics {

	public static void SetAnalyticsCollectionEnabled(Activity activity, boolean enabled) {
		FirebaseAnalytics analytics = FirebaseAnalytics.getInstance(activity);
		analytics.setAnalyticsCollectionEnabled(enabled);
	}

	public static void ResetAnalyticsData(Activity activity) {
		FirebaseAnalytics analytics = FirebaseAnalytics.getInstance(activity);
		analytics.resetAnalyticsData();
	}

	public static void SetSessionTimeoutDuration(Activity activity, long milliseconds) {
		FirebaseAnalytics analytics = FirebaseAnalytics.getInstance(activity);
		analytics.setSessionTimeoutDuration(milliseconds);
	}

	public static void SetUserId(Activity activity, String id) {
		FirebaseAnalytics analytics = FirebaseAnalytics.getInstance(activity);
		analytics.setUserId(!id.isEmpty() ? id : null);
	}

	public static void SetUserProperty(Activity activity, String name, String value) {
		FirebaseAnalytics analytics = FirebaseAnalytics.getInstance(activity);
		analytics.setUserProperty(name, !value.isEmpty() ? value : null);
	}

	public static void SetCurrentScreen(final Activity activity, final String name, final String className) {
		activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				FirebaseAnalytics analytics = FirebaseAnalytics.getInstance(activity);
				analytics.setCurrentScreen(activity,
						!name.isEmpty() ? name : null,
						!className.isEmpty() ? className : null);
			}
		});
	}

	public static void LogEvent(Activity activity, String eventName) {
		FirebaseAnalytics analytics = FirebaseAnalytics.getInstance(activity);
		analytics.logEvent(eventName, null);
	}

	public static void LogEvent(Activity activity, String eventName, Bundle parameters) {
		FirebaseAnalytics analytics = FirebaseAnalytics.getInstance(activity);
		analytics.logEvent(eventName, parameters);
	}
}
