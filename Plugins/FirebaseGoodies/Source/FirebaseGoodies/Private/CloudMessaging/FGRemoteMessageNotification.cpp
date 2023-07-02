// Copyright (c) 2022 Nineva Studios

#include "CloudMessaging/FGRemoteMessageNotification.h"

#include "FirebaseGoodiesLog.h"

#if PLATFORM_ANDROID
#include "Android/Utils/FGJavaConvertor.h"
#include "Android/Utils/FGMethodCallUtils.h"
#endif

UFGRemoteMessageNotification::~UFGRemoteMessageNotification()
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGRemoteMessageNotification::~UFGRemoteMessageNotification"));
	
#if PLATFORM_ANDROID
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	Env->DeleteGlobalRef(JavaRemoteMessageNotificationObject);
	JavaRemoteMessageNotificationObject = nullptr;
#endif
}

FString UFGRemoteMessageNotification::GetBody()
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGRemoteMessageNotification::GetBody"));
	
	FString result;

#if PLATFORM_ANDROID
	result = FGMethodCallUtils::CallStringMethod(JavaRemoteMessageNotificationObject, "getBody", "()Ljava/lang/String;");
#endif

	return result;
}

TArray<FString> UFGRemoteMessageNotification::GetBodyLocalizationArgs()
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGRemoteMessageNotification::GetBodyLocalizationArgs"));
	
	TArray<FString> result;

#if PLATFORM_ANDROID
	jobjectArray javaResult = static_cast<jobjectArray>(FGMethodCallUtils::CallObjectMethod(JavaRemoteMessageNotificationObject, "getBodyLocalizationArgs", "()[Ljava/lang/String;"));
	if (javaResult != nullptr) {
		result = FGJavaConvertor::ConvertToStringArray(javaResult);
	}
#endif

	return result;
}

FString UFGRemoteMessageNotification::GetBodyLocalizationKey()
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGRemoteMessageNotification::GetBodyLocalizationKey"));
	
	FString result;

#if PLATFORM_ANDROID
	result = FGMethodCallUtils::CallStringMethod(JavaRemoteMessageNotificationObject, "getBodyLocalizationKey", "()Ljava/lang/String;");
#endif

	return result;
}

FString UFGRemoteMessageNotification::GetChannelId()
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGRemoteMessageNotification::GetChannelId"));
	
	FString result;

#if PLATFORM_ANDROID
	result = FGMethodCallUtils::CallStringMethod(JavaRemoteMessageNotificationObject, "getChannelId", "()Ljava/lang/String;");
#endif

	return result;
}

FString UFGRemoteMessageNotification::GetClickAction()
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGRemoteMessageNotification::GetClickAction"));
	
	FString result;

#if PLATFORM_ANDROID
	result = FGMethodCallUtils::CallStringMethod(JavaRemoteMessageNotificationObject, "getClickAction", "()Ljava/lang/String;");
#endif

	return result;
}

FString UFGRemoteMessageNotification::GetColor()
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGRemoteMessageNotification::GetColor"));
	
	FString result;

#if PLATFORM_ANDROID
	result = FGMethodCallUtils::CallStringMethod(JavaRemoteMessageNotificationObject, "getColor", "()Ljava/lang/String;");
#endif

	return result;
}

FString UFGRemoteMessageNotification::GetIcon()
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGRemoteMessageNotification::GetIcon"));
	
	FString result;

#if PLATFORM_ANDROID
	result = FGMethodCallUtils::CallStringMethod(JavaRemoteMessageNotificationObject, "getIcon", "()Ljava/lang/String;");
#endif

	return result;
}

FString UFGRemoteMessageNotification::GetImageUri()
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGRemoteMessageNotification::GetImageUri"));
	
	FString result;

#if PLATFORM_ANDROID
	jobject javaUri = FGMethodCallUtils::CallObjectMethod(JavaRemoteMessageNotificationObject, "getImageUrl", "()Landroid/net/Uri;");
	if (javaUri != nullptr) {
		result = FGJavaConvertor::FromJavaUri(javaUri);
	}
#endif

	return result;
}

FString UFGRemoteMessageNotification::GetLink()
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGRemoteMessageNotification::GetLink"));
	
	FString result;

#if PLATFORM_ANDROID
	jobject javaUri = FGMethodCallUtils::CallObjectMethod(JavaRemoteMessageNotificationObject, "getLink", "()Landroid/net/Uri;");
	if (javaUri != nullptr) {
		result = FGJavaConvertor::FromJavaUri(javaUri);
	}
#endif

	return result;
}

FString UFGRemoteMessageNotification::GetSound()
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGRemoteMessageNotification::GetSound"));
	
	FString result;

#if PLATFORM_ANDROID
	result = FGMethodCallUtils::CallStringMethod(JavaRemoteMessageNotificationObject, "getSound", "()Ljava/lang/String;");
#endif

	return result;
}

FString UFGRemoteMessageNotification::GetTag()
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGRemoteMessageNotification::GetTag"));
	
	FString result;

#if PLATFORM_ANDROID
	result = FGMethodCallUtils::CallStringMethod(JavaRemoteMessageNotificationObject, "getTag", "()Ljava/lang/String;");
#endif

	return result;
}

FString UFGRemoteMessageNotification::GetTitle()
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGRemoteMessageNotification::GetTitle"));
	
	FString result;

#if PLATFORM_ANDROID
	result = FGMethodCallUtils::CallStringMethod(JavaRemoteMessageNotificationObject, "getTitle", "()Ljava/lang/String;");
#endif

	return result;
}

TArray<FString> UFGRemoteMessageNotification::GetTitleLocalizationArgs()
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGRemoteMessageNotification::GetTitleLocalizationArgs"));
	
	TArray<FString> result;

#if PLATFORM_ANDROID
	jobjectArray javaResult = static_cast<jobjectArray>(FGMethodCallUtils::CallObjectMethod(JavaRemoteMessageNotificationObject, "getTitleLocalizationArgs", "()[Ljava/lang/String;"));
	if (javaResult != nullptr) {
		result = FGJavaConvertor::ConvertToStringArray(javaResult);
	}
#endif

	return result;
}

FString UFGRemoteMessageNotification::GetTitleLocalizationKey()
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGRemoteMessageNotification::GetTitleLocalizationKey"));
	
	FString result;

#if PLATFORM_ANDROID
	result = FGMethodCallUtils::CallStringMethod(JavaRemoteMessageNotificationObject, "getTitleLocalizationKey", "()Ljava/lang/String;");
#endif

	return result;
}

#if PLATFORM_ANDROID
void UFGRemoteMessageNotification::Init(jobject javaRemoteMessageNotification)
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	JavaRemoteMessageNotificationObject = Env->NewGlobalRef(javaRemoteMessageNotification);
}
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
void UFGRemoteMessageNotification::Init(firebase::messaging::Notification* notification)
{
	Notification = notification;
}
#endif
