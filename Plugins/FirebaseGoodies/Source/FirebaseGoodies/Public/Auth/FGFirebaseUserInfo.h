// Copyright (c) 2022 Nineva Studios

#pragma once

#if PLATFORM_ANDROID
#include "Android/AndroidApplication.h"
#include "Android/AndroidJNI.h"
#include "Android/AndroidJava.h"
#endif

#if PLATFORM_IOS
#import <FirebaseAuth/FirebaseAuth.h>
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
#include "firebase/auth.h"
#endif

#include "FGFirebaseUserInfo.generated.h"

UCLASS(BlueprintType)
class FIREBASEGOODIES_API UFGFirebaseUserInfo : public UObject
{
	GENERATED_BODY()
	
public:
	virtual ~UFGFirebaseUserInfo();
	
	/**
	 * @return a string used to uniquely identify your user in your Firebase project's user database.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Auth|Firebase User")
	FString GetUid();
	
	/**
	 * @return the provider ID for the user (For example, "Facebook").
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Auth|Firebase User")
	FString GetProviderId();
	
	/**
	 * @return the display name associated with the user, if any.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Auth|Firebase User")
	FString GetDisplayName();
	
	/**
	 * @return the photo url associated with the user, if any.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Auth|Firebase User")
	FString GetPhotoUrl();
	
	/**
	 * @return the email associated with the user, if any.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Auth|Firebase User")
	FString GetEmail();
	
	/**
	 * @return the phone number associated with the user, if any.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Auth|Firebase User")
	FString GetPhoneNumber();
	
	/**
	 * Convenience method to check if the native iOS/Android user info object is valid.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Auth|Firebase User")
	bool IsUserInfoValid();
	
#if PLATFORM_ANDROID
	void InitUserInfo(jobject javaUserInfo);
	jobject JavaFirebaseUserInfoObject;
#endif
	
#if PLATFORM_IOS
	void InitUserInfo(id<FIRUserInfo> userInfo);
	id<FIRUserInfo> iosUserInfo;
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	void InitUserInfo(firebase::auth::UserInfoInterface* user);
	firebase::auth::UserInfoInterface* desktopUser;
#endif
};
