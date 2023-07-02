// Copyright (c) 2022 Nineva Studios

package com.ninevastudios.unrealfirebase;


import android.os.AsyncTask;

import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.Task;
import com.google.firebase.iid.FirebaseInstanceId;
import com.google.firebase.iid.InstanceIdResult;
import com.google.firebase.messaging.FirebaseMessaging;
import com.google.firebase.messaging.RemoteMessage;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

@SuppressWarnings("unused")
public class FGCloudMessaging {

	public static native void OnTokenReceived(String token, String id);

	public static native void OnTokenReceiveError(String error);

	public static native void OnSubscribedToTopic();

	public static native void OnSubscribeToTopicError(String error);

    public static native void OnUnsubscribedFromTopic();

    public static native void OnUnsubscribeFromTopicError(String error);

	public static void getInstanceIdData() {
		FirebaseInstanceId.getInstance().getInstanceId().addOnCompleteListener(
				new OnCompleteListener<InstanceIdResult>() {
					@Override
					public void onComplete(Task<InstanceIdResult> task) {
						if (!task.isSuccessful() && task.getException() != null) {
							OnTokenReceiveError(task.getException().getMessage());
						} else if (task.getResult() != null) {
							OnTokenReceived(task.getResult().getToken(), task.getResult().getId());
						} else {
							OnTokenReceiveError("Both result and exception are absent. Considered as error");
						}
					}
				});
	}

	public static void deleteInstanceId() {
		Runnable runnable = new Runnable() {
			@Override
			public void run() {
				try {
					FirebaseInstanceId.getInstance().deleteInstanceId();
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		};
		AsyncTask.execute(runnable);
	}

	public static RemoteMessage.Builder getRemoteMessageBuilder(String senderId) {
		return new RemoteMessage.Builder(senderId + "@gcm.googleapis.com");
	}

	public static void sendMessage(RemoteMessage.Builder messageBuilder) {
		FirebaseMessaging.getInstance().send(messageBuilder.build());
	}

	public static void setAutoInitEnabled(boolean enabled) {
		FirebaseMessaging.getInstance().setAutoInitEnabled(enabled);
	}

	public static boolean isAutoInitEnabled() {
		return FirebaseMessaging.getInstance().isAutoInitEnabled();
	}

	public static void subscribeToTopic(String topic) {
		FirebaseMessaging.getInstance().subscribeToTopic(topic).addOnCompleteListener(new OnCompleteListener<Void>() {
			@Override
			public void onComplete(Task<Void> task) {
				if (!task.isSuccessful() && task.getException() != null) {
					OnSubscribeToTopicError(task.getException().getMessage());
				} else {
					OnSubscribedToTopic();
				}
			}
		});
	}

    public static void unsubscribeFromTopic(String topic) {
        FirebaseMessaging.getInstance().unsubscribeFromTopic(topic).addOnCompleteListener(new OnCompleteListener<Void>() {
            @Override
            public void onComplete(Task<Void> task) {
                if (!task.isSuccessful() && task.getException() != null) {
                    OnUnsubscribeFromTopicError(task.getException().getMessage());
                } else {
                    OnUnsubscribedFromTopic();
                }
            }
        });
    }

    public static HashMap<String, String> getData(RemoteMessage message) {
	    Map<String, String> map = message.getData();
	    return new HashMap<>(map);
    }
}
