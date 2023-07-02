// Copyright (c) 2021 Nineva Studios

package com.ninevastudios.unrealfirebase;

import com.google.firebase.dynamiclinks.PendingDynamicLinkData;

@SuppressWarnings("unused")
public class FGDynamicLinks {
	static PendingDynamicLinkData pendingDynamicLinkData;

	public static String getDynamicLinkUrl() {
		String result = "";
		try {
			result = pendingDynamicLinkData.getLink().toString();
		} catch (Exception e) {
			e.printStackTrace();
		}

		return result;
	}

	public static int getMinimumAppVersion() {
		return pendingDynamicLinkData == null ? 0 : pendingDynamicLinkData.getMinimumAppVersion();
	}

	public static void clearDynamicLink() {
		pendingDynamicLinkData = null;
	}

	public static boolean wasAppOpenViaDynamicLink() {
		return pendingDynamicLinkData != null;
	}
}
