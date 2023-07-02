// Copyright (c) 2022 Nineva Studios

#include "Auth/FGAuthCredentials.h"

#include "Auth/FGAuthLibrary.h"
#include "FirebaseGoodiesLog.h"

#if PLATFORM_ANDROID
#include "Android/Utils/FGMethodCallUtils.h"
#endif

UFGAuthCredentials::~UFGAuthCredentials()
{
#if PLATFORM_ANDROID
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	Env->DeleteGlobalRef(JavaAuthCredentialObject);
	JavaAuthCredentialObject = nullptr;
#elif PLATFORM_IOS
	(FIRAuthCredential*)CFBridgingRelease(iosCredential);
	iosCredential = nil;
#endif
}

FString UFGAuthCredentials::GetProvider()
{
	FString result;

	if (!AreValid())
	{
		return result;
	}

#if PLATFORM_ANDROID
	result = FGMethodCallUtils::CallStringMethod(JavaAuthCredentialObject, "getProvider", "()Ljava/lang/String;");
#elif PLATFORM_IOS
	result = FString([iosCredential provider]);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	return desktopCredential.provider().c_str();
#endif

	return result;
}

bool UFGAuthCredentials::AreValid()
{
	bool bAreCredentialsValid = false;
#if PLATFORM_ANDROID
	bAreCredentialsValid = JavaAuthCredentialObject != nullptr;
#elif PLATFORM_IOS
	bAreCredentialsValid = iosCredential != nil;
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	bAreCredentialsValid = desktopCredential.is_valid();
#endif
	if (!bAreCredentialsValid)
	{
		UE_LOG(LogFirebaseGoodies, Error, TEXT("Auth Credentials are not valid."));
	}
	return bAreCredentialsValid;
}

#if PLATFORM_ANDROID
void UFGAuthCredentials::Init(jobject javaCredential)
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	JavaAuthCredentialObject = Env->NewGlobalRef(javaCredential);
}
#endif

#if PLATFORM_IOS
void UFGAuthCredentials::Init(FIRAuthCredential* credential)
{
	iosCredential = (FIRAuthCredential*)CFBridgingRetain(credential);
}
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
void UFGAuthCredentials::Init(firebase::auth::Credential credential)
{
	desktopCredential = credential;
}
#endif
