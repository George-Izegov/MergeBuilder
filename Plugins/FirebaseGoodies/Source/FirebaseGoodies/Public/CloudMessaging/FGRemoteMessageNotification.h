// Copyright (c) 2022 Nineva Studios

#pragma once

#if PLATFORM_ANDROID
#include "Android/AndroidJNI.h"
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
#include "firebase/messaging.h"
#endif

#include "FGRemoteMessageNotification.generated.h"

/**
 * Remote Firebase notification details.
 * This class maps to the fields of a notification message.
 */
UCLASS(BlueprintType)
class FIREBASEGOODIES_API UFGRemoteMessageNotification : public UObject
{
	GENERATED_BODY()

		virtual ~UFGRemoteMessageNotification();

public:
	
	/**
	 * Gets the body of the notification.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Messaging|Remote Message Notification")
	FString GetBody();

	/**
	 * Gets the variable string values to be used as format specifiers in the body localization key.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Messaging|Remote Message Notification")
	TArray<FString> GetBodyLocalizationArgs();

	/**
	 * Gets the string resource name to use to localize the body of the notification.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Messaging|Remote Message Notification")
	FString GetBodyLocalizationKey();

	/**
	 * Gets the channel id from the notification.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Messaging|Remote Message Notification")
	FString GetChannelId();

	/**
	 * Gets the action to be performed on the user opening the notification.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Messaging|Remote Message Notification")
	FString GetClickAction();

	/**
	 * Gets the color of the notification.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Messaging|Remote Message Notification")
	FString GetColor();

	/**
	 * Gets the image resource name of the icon of the notification.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Messaging|Remote Message Notification")
	FString GetIcon();

	/**
	 * Gets the image URL from the notification.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Messaging|Remote Message Notification")
	FString GetImageUri();

	/**
	 * Gets the deep link from the notification.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Messaging|Remote Message Notification")
	FString GetLink();

	/**
	 * Gets the sound to be played when the notification is shown.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Messaging|Remote Message Notification")
	FString GetSound();

	/**
	 * Gets the tag of the notification.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Messaging|Remote Message Notification")
	FString GetTag();

	/**
	 * Gets the title of the notification, or null if not set.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Messaging|Remote Message Notification")
	FString GetTitle();

	/**
	 * Gets the variable string values to be used as format specifiers in the title localization key.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Messaging|Remote Message Notification")
	TArray<FString> GetTitleLocalizationArgs();

	/**
	 * Gets the string resource name to use to localize the title of the notification.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Messaging|Remote Message Notification")
	FString GetTitleLocalizationKey();


#if PLATFORM_ANDROID
	void Init(jobject javaRemoteMessageNotification);
	jobject JavaRemoteMessageNotificationObject;
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	void Init(firebase::messaging::Notification* notification);
	firebase::messaging::Notification* Notification;
#endif

};
