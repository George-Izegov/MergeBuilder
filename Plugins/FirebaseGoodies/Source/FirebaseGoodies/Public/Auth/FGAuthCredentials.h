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

#include "FGAuthCredentials.generated.h"

UCLASS(BlueprintType)
class FIREBASEGOODIES_API UFGAuthCredentials : public UObject
{
	GENERATED_BODY()

	virtual ~UFGAuthCredentials();

public:
	/**
	* Get name of the provider, associated with these credentials.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Auth|Credentials")
	FString GetProvider();

	/**
	* Convenience method to check if the native iOS/Android credentials object is valid.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Auth|Credentials")
	bool AreValid();

#if PLATFORM_ANDROID
	void Init(jobject javaCredential);

	jobject JavaAuthCredentialObject;
#endif

#if PLATFORM_IOS
	void Init(FIRAuthCredential* credential);
	FIRAuthCredential* iosCredential;
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	void Init(firebase::auth::Credential credential);
	firebase::auth::Credential desktopCredential;
#endif
};
