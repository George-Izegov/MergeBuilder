// Copyright (c) 2022 Nineva Studios

#pragma once

#if PLATFORM_ANDROID
#include "Android/AndroidJNI.h"
#endif

#if PLATFORM_IOS
#import <FirebaseMessaging/FirebaseMessaging.h>
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
#include "firebase/messaging.h"
#endif

class UFGRemoteMessageNotification;

#include "Misc/DateTime.h"

#include "FGRemoteMessage.generated.h"


UENUM(BlueprintType)
enum class EFGRemoteMessagePriority : uint8
{
	Unknown = 0,
	High = 1,
	Normal = 2
};

/**
 * An entity received from the FCM. (Android only).
 * IOS handles the received messages via BindEventToOnRemoteNotificationReceived. It is then received as a JSON-like playload.
 *
 * Messages will be received via BindEventToOnMessageReceived and can be sent via SendMessage().
 * Messages may have a UFGRemoteMessageNotification instance if they are received while the application is in the foreground,
 * otherwise they will be automatically posted to the notification tray.
 */
UCLASS(BlueprintType)
class FIREBASEGOODIES_API UFGRemoteMessage : public UObject
{
	GENERATED_BODY()

		virtual ~UFGRemoteMessage();
	
public:

	/**
	 * Gets the collapse key of the message.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Messaging|Remote Message")
	FString GetCollapseKey();

	/**
	 * Gets the message payload data.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Messaging|Remote Message")
	TMap<FString, FString> GetData();

	/**
	 * Get the sender of this message.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Messaging|Remote Message")
	FString GetFrom();

	/**
	 * Gets the message's ID.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Messaging|Remote Message")
	FString GetMessageId();

	/**
	 * Gets the type of message.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Messaging|Remote Message")
	FString GetMessageType();

	/**
	 * Gets the notification data from the message if set.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Messaging|Remote Message")
	UFGRemoteMessageNotification* GetNotification();

	/**
	 * Gets the original priority of message.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Messaging|Remote Message")
	EFGRemoteMessagePriority GetOriginalPriority();

	/**
	 * Gets the priority of message as delivered.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Messaging|Remote Message")
	EFGRemoteMessagePriority GetPriority();

	/**
	 * Gets the time  the message was sent.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Messaging|Remote Message")
	FDateTime GetSentTime();

	/**
	 * Gets the message destination.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Messaging|Remote Message")
	FString GetTo();

	/**
	 * Gets the message time to live (TTL) in seconds.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Messaging|Remote Message")
	int GetTtl();
	
	/**
	 * Convenience function to check whether the native Message object is valid.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud messaging")
	bool IsMessageValid();

#if PLATFORM_ANDROID
	void Init(jobject javaRemoteMessage);
	jobject JavaRemoteMessageObject;
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	void Init(firebase::messaging::Message message);
	firebase::messaging::Message Message;
#endif

	static const ANSICHAR* FGRemoteMessageClassName;
};
