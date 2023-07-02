// Copyright (c) 2022 Nineva Studios

#include "Auth/FGFirebaseUserInfo.h"
#include "FirebaseGoodiesLog.h"

#if PLATFORM_ANDROID
#include "Android/Utils/FGJavaConvertor.h"
#include "Android/Utils/FGMethodCallUtils.h"
#endif

UFGFirebaseUserInfo::~UFGFirebaseUserInfo()
{
#if PLATFORM_ANDROID
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	Env->DeleteGlobalRef(JavaFirebaseUserInfoObject);
	JavaFirebaseUserInfoObject = nullptr;
#elif PLATFORM_IOS
	(id<FIRUserInfo>)CFBridgingRelease(iosUserInfo);
	iosUserInfo = nil;
#endif
}

FString UFGFirebaseUserInfo::GetUid()
{
	FString result;
	
	if (!IsUserInfoValid())
	{
		return result;
	}
	
#if PLATFORM_ANDROID
	result = FGMethodCallUtils::CallStringMethod(JavaFirebaseUserInfoObject, "getUid", "()Ljava/lang/String;");
#elif PLATFORM_IOS
	result = FString([iosUserInfo uid]);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	result = desktopUser->uid().c_str();
#endif
	
	return result;
}

FString UFGFirebaseUserInfo::GetProviderId()
{
	FString result;
	
	if (!IsUserInfoValid())
	{
		return result;
	}
	
#if PLATFORM_ANDROID
	result = FGMethodCallUtils::CallStringMethod(JavaFirebaseUserInfoObject, "getProviderId", "()Ljava/lang/String;");
#elif PLATFORM_IOS
	result = FString([iosUserInfo providerID]);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	result = desktopUser->provider_id().c_str();
#endif
	
	return result;
}

FString UFGFirebaseUserInfo::GetDisplayName()
{
	FString result;
	
	if (!IsUserInfoValid())
	{
		return result;
	}
	
#if PLATFORM_ANDROID
	result = FGMethodCallUtils::CallStringMethod(JavaFirebaseUserInfoObject, "getDisplayName", "()Ljava/lang/String;");
#elif PLATFORM_IOS
	result = FString([iosUserInfo displayName]);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	result = desktopUser->display_name().c_str();
#endif
	
	return result;
}

FString UFGFirebaseUserInfo::GetPhotoUrl()
{
	FString result;
	
	if (!IsUserInfoValid())
	{
		return result;
	}
	
#if PLATFORM_ANDROID
	result = FGMethodCallUtils::CallStaticStringMethod(UFGAuthLibrary::FGAuthClassName, "getUserInfoPhotoUrl", "(Lcom/google/firebase/auth/UserInfo;)Ljava/lang/String;", JavaFirebaseUserInfoObject);
#elif PLATFORM_IOS
	result = FString([iosUserInfo photoURL].absoluteString);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	result = desktopUser->photo_url().c_str();
#endif
	
	return result;
}

FString UFGFirebaseUserInfo::GetEmail()
{
	FString result;
	
	if (!IsUserInfoValid())
	{
		return result;
	}
	
#if PLATFORM_ANDROID
	result = FGMethodCallUtils::CallStringMethod(JavaFirebaseUserInfoObject, "getEmail", "()Ljava/lang/String;");
#elif PLATFORM_IOS
	result = FString([iosUserInfo email]);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	result = desktopUser->email().c_str();
#endif
	
	return result;
}

FString UFGFirebaseUserInfo::GetPhoneNumber()
{
	FString result;
	
	if (!IsUserInfoValid())
	{
		return result;
	}
	
#if PLATFORM_ANDROID
	result = FGMethodCallUtils::CallStringMethod(JavaFirebaseUserInfoObject, "getPhoneNumber", "()Ljava/lang/String;");
#elif PLATFORM_IOS
	result = FString([iosUserInfo phoneNumber]);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	result = desktopUser->phone_number().c_str();
#endif
	
	return result;
}

bool UFGFirebaseUserInfo::IsUserInfoValid()
{
	bool bIsUserInfoValid = false;
#if PLATFORM_ANDROID
	bIsUserInfoValid = JavaFirebaseUserInfoObject != nullptr;
#elif PLATFORM_IOS
	bIsUserInfoValid = iosUserInfo != nil;
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	bIsUserInfoValid = desktopUser != nullptr;
#endif
	if (!bIsUserInfoValid)
	{
		UE_LOG(LogFirebaseGoodies, Error, TEXT("User info is not valid."));
	}
	return bIsUserInfoValid;
}


#if PLATFORM_ANDROID
void UFGFirebaseUserInfo::InitUserInfo(jobject javaUserInfo)
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	JavaFirebaseUserInfoObject = Env->NewGlobalRef(javaUserInfo);
}
#endif

#if PLATFORM_IOS
void UFGFirebaseUserInfo::InitUserInfo(id<FIRUserInfo> userInfo)
{
	iosUserInfo = (id<FIRUserInfo>)CFBridgingRetain(userInfo);
}
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
void UFGFirebaseUserInfo::InitUserInfo(firebase::auth::UserInfoInterface* user)
{
	desktopUser = user;
}
#endif

