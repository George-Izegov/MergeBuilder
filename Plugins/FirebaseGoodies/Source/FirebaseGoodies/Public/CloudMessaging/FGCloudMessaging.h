// Copyright (c) 2022 Nineva Studios

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "FGCloudMessaging.generated.h"

class UFGRemoteMessageBuilder;
class UFGRemoteMessage;

DECLARE_DYNAMIC_DELEGATE(FCloudMessagingVoidDelegate);
DECLARE_DYNAMIC_DELEGATE_OneParam(FCloudMessagingStringDelegate, FString, String);
DECLARE_DYNAMIC_DELEGATE_OneParam(FCloudMessagingMessageDelegate, UFGRemoteMessage*, Message);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FCloudMessagingTwoStringDelegate, FString, FirstString, FString, SecondString);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FCloudMessagingRemoteNotificationDelegate, FString, Payload, int, State);

/**
 * Class for using Firebase Cloud Messaging API.
 */
UCLASS()
class FIREBASEGOODIES_API UFGCloudMessaging : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
	/**
	 * Get token and instance ID for this application instance.
	 *
	 * @param OnSuccess - action to perform with the obtained token (first string) and instance ID (second string).
	 * @param OnError - action to perform with the error description.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud messaging")
	static void GetInstanceIdData(const FCloudMessagingTwoStringDelegate& OnSuccess,
								  const FCloudMessagingStringDelegate& OnError);
	
	/**
	 * Delete the Instance ID and the data associated with it.
	 * This stops the periodic sending of data to the Firebase backend started when the Instance ID was generated,
	 * unless another library that requires InstanceId (like FCM, RemoteConfig or Analytics) is used or it's configured to be executed automatically.
	 * A new Instance ID is generated asynchronously if Firebase Cloud Messaging auto-init is enabled.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud messaging")
	static void DeleteInstanceId();
	
	/**
	 * Determines whether FCM auto-initialization is enabled or disabled.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud messaging")
	static bool IsAutoInitEnabled();
	
	/**
	 * Enables or disables auto-initialization of Firebase Cloud Messaging.
	 *
	 * When enabled, Firebase Cloud Messaging generates a registration token on app startup
	 * if there is no valid one and generates a new token when it is deleted.
	 * This setting is persisted across app restarts and overrides the setting specified in your manifest.
	 * By default, Firebase Cloud Messaging auto-initialization is enabled.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud messaging")
	static void SetAutoInitEnabled(bool Enabled);
	
	/**
	 * Subscribes to topic in the background.
	 * The subscribe operation is persisted and will be retried until successful.
	 * This uses a Firebase Instance ID token to identify the app instance and periodically sends data to the Firebase backend.
	 *
	 * @param Topic - topic name to subscribe to.
	 * @param OnSuccess - action to be performed if the operation was successful.
	 * Returns the topic name as the parameter to distinguish between topics.
	 * @param OnError - action to be performed if the operation was not successful.
	 * Returns the topic name as the first parameter to distinguish between topics and error description as second.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud messaging")
	static void SubscribeToTopic(const FString& Topic,
								 const FCloudMessagingVoidDelegate& OnSuccess,
								 const FCloudMessagingStringDelegate& OnError);
	
	/**
	 * Unsubscribes from topic in the background.
	 * The unsubscribe operation is persisted and will be retried until successful.
	 * This does not stop FirebaseInstanceId's periodic sending of data started by SubscribeToTopic.
	 *
	 * @param Topic - topic name to unsubscribe from.
	 * @param OnSuccess - action to be performed if the operation was successful.
	 * Returns the topic name as the parameter to distinguish between topics.
	 * @param OnError - action to be performed if the operation was not successful.
	 * Returns the topic name as the first parameter to distinguish between topics and error description as second.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud messaging")
	static void UnSubscribeFromTopic(const FString& Topic,
									 const FCloudMessagingVoidDelegate& OnSuccess,
									 const FCloudMessagingStringDelegate& OnError);
	
	/**
	 * Bind a custom event to be invoked every time the app receives a Remote Message.
	 * Is used mostly for Android, as the downstream messages on iOS are handled by the BindEventToOnRemoteNotificationReceived.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud messaging")
	static void BindEventToOnMessageReceived(const FCloudMessagingMessageDelegate& EventDelegate)
	{ OnMessageReceivedCallback = EventDelegate; }
	
	/**
	 * Bind a custom event to be invoked every time Remote Messages are deleted. (Android only)
	 *
	 * Called when the Firebase Cloud Messaging server deletes pending messages. This may be due to:
	 * - Too many messages stored on the Firebase Cloud Messaging server. This can occur when the app's servers send a bunch of non-collapsible messages to Firebase Cloud
	 * Messaging servers while the device is offline.
	 * - The device hasn't connected in a long time and the app server has recently (within the last 4 weeks) sent a message to the app on that device.
	 * It is recommended that the app do a full sync with the app server after receiving this call.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud messaging")
	static void BindEventToOnMessagesDeleted(const FCloudMessagingVoidDelegate& EventDelegate)
	{ OnMessagesDeletedCallback = EventDelegate; }
	
	/**
	 * Bind a custom event to be invoked every time upstream Remote Message is sent. (Android only)
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud messaging")
	static void BindEventToOnMessageSent(const FCloudMessagingStringDelegate& EventDelegate)
	{ OnMessageSentCallback = EventDelegate; }
	
	/**
	 * Bind a custom event to be invoked every time upstream Remote Message is not sent. (Android only)
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud messaging")
	static void BindEventToOnMessageSendError(const FCloudMessagingTwoStringDelegate& EventDelegate)
	{ OnMessageSendErrorCallback = EventDelegate; }
	
	/**
	 * Bind a custom event to be invoked every time a new token is generated.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud messaging")
	static void BindEventToOnNewToken(const FCloudMessagingStringDelegate& EventDelegate)
	{ OnNewTokenCallback = EventDelegate; }
	
	/**
	 * Bind a custom event to be invoked every time a Remote Notification is received by iOS.
	 * It is presented as a JSON-serialized data in the similar format:
	 * {"borsch":"carl",
	 * 	"google.c.sender.id":"YOUR_SENDER_ID",
	 * 	"google.c.a.e":"1",
	 * 	"google.c.a.c_l":"Test",
	 * 	"aps":
	 * 		{"alert":
	 * 			{"title":"Test",
	 * 			"body":"AAAAAAAAAAAAAAAAAAAAA"},
	 * 		"mutable-content":"1"},
	 * 	"gcm.n.e":"1",
	 * 	"google.c.a.c_id":"5430013481095281072",
	 * 	"fcm_options":
	 * 		{"image":"https://vignette.wikia.nocookie.net/jaygt/images/4/41/Hidethepainharold.png"},
	 * 	"google.c.a.udt":"0",
	 * 	"google.c.a.ts":"1597240321",
	 * 	"gcm.message_id":"1597240322278253"}
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud messaging")
	static void BindEventToOnRemoteNotificationReceived(const FCloudMessagingRemoteNotificationDelegate& EventDelegate)
	{ OnRemoteNotificationReceivedCallback = EventDelegate; }
	
	static FCloudMessagingTwoStringDelegate OnInstanceIdDataReceivedCallback;
	static FCloudMessagingStringDelegate OnInstanceIdDataReceiveErrorCallback;
	
	static FCloudMessagingVoidDelegate OnTopicOperationSuccessCallback;
	static FCloudMessagingStringDelegate OnTopicOperationErrorCallback;
	
	static FCloudMessagingMessageDelegate OnMessageReceivedCallback;
	static FCloudMessagingVoidDelegate OnMessagesDeletedCallback;
	static FCloudMessagingStringDelegate OnMessageSentCallback;
	static FCloudMessagingTwoStringDelegate OnMessageSendErrorCallback;
	static FCloudMessagingStringDelegate OnNewTokenCallback;
	
	static FCloudMessagingRemoteNotificationDelegate OnRemoteNotificationReceivedCallback;
	
	static const ANSICHAR* FGCloudMessagingClassName;
};

