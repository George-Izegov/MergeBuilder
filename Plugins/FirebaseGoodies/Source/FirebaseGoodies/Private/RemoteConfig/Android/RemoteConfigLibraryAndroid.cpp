// Copyright (c) 2022 Nineva Studios

#include "RemoteConfigLibraryAndroid.h"

#include "RemoteConfig/FGRemoteConfigParameterVariant.h"
#include "RemoteConfig/Callbacks/FGRemoteConfigCallback.h"

#include "Android/Utils/FGMethodCallUtils.h"
#include "Android/Utils/FGJavaConvertor.h"

#include "Android/AndroidApplication.h"
#include "Android/AndroidJNI.h"
#include "Android/AndroidJava.h"

static const ANSICHAR* FGRemoteConfigClassName = "com/ninevastudios/unrealfirebase/FGRemoteConfig";

RemoteConfigLibraryAndroid::~RemoteConfigLibraryAndroid()
{
}

void RemoteConfigLibraryAndroid::SetConfigSettings(int64 MinFetchInterval, int64 FetchTimeout)
{
	FGMethodCallUtils::CallStaticVoidMethod(FGRemoteConfigClassName, "SetConfigSettings", "(JJ)V",
		MinFetchInterval, FetchTimeout);
}

void RemoteConfigLibraryAndroid::Fetch(const FOnRemoteConfigSuccess& OnSuccess, const FOnRemoteConfigError& OnError)
{
	UFGRemoteConfigCallback* NativeCallback = NewObject<UFGRemoteConfigCallback>();
	NativeCallback->AddToRoot();
	NativeCallback->BindSuccessDelegate(OnSuccess);
	NativeCallback->BindErrorDelegate(OnError);

	FGMethodCallUtils::CallStaticVoidMethod(FGRemoteConfigClassName, "Fetch", "(Landroid/app/Activity;J)V",
		FJavaWrapper::GameActivityThis, (jlong) NativeCallback);
}

void RemoteConfigLibraryAndroid::FetchWithInterval(int64 TimeInterval, const FOnRemoteConfigSuccess& OnSuccess, const FOnRemoteConfigError& OnError)
{
	UFGRemoteConfigCallback* NativeCallback = NewObject<UFGRemoteConfigCallback>();
	NativeCallback->AddToRoot();
	NativeCallback->BindSuccessDelegate(OnSuccess);
	NativeCallback->BindErrorDelegate(OnError);

	FGMethodCallUtils::CallStaticVoidMethod(FGRemoteConfigClassName, "FetchWithInterval", "(Landroid/app/Activity;JJ)V",
		FJavaWrapper::GameActivityThis, TimeInterval, (jlong) NativeCallback);
}

void RemoteConfigLibraryAndroid::FetchAndActivate(const FOnRemoteConfigSuccess& OnSuccess, const FOnRemoteConfigError& OnError)
{
	UFGRemoteConfigCallback* NativeCallback = NewObject<UFGRemoteConfigCallback>();
	NativeCallback->AddToRoot();
	NativeCallback->BindSuccessDelegate(OnSuccess);
	NativeCallback->BindErrorDelegate(OnError);

	FGMethodCallUtils::CallStaticVoidMethod(FGRemoteConfigClassName, "FetchAndActivate", "(Landroid/app/Activity;J)V",
		FJavaWrapper::GameActivityThis, (jlong) NativeCallback);
}

void RemoteConfigLibraryAndroid::Activate(const FOnRemoteConfigSuccess& OnSuccess, const FOnRemoteConfigError& OnError)
{
	UFGRemoteConfigCallback* NativeCallback = NewObject<UFGRemoteConfigCallback>();
	NativeCallback->AddToRoot();
	NativeCallback->BindSuccessDelegate(OnSuccess);
	NativeCallback->BindErrorDelegate(OnError);

	FGMethodCallUtils::CallStaticVoidMethod(FGRemoteConfigClassName, "Activate", "(Landroid/app/Activity;J)V",
		FJavaWrapper::GameActivityThis, (jlong) NativeCallback);
}

bool RemoteConfigLibraryAndroid::GetBoolean(FString Key)
{
	return FGMethodCallUtils::CallStaticBoolMethod(FGRemoteConfigClassName, "GetBoolean", "(Ljava/lang/String;)Z",
		*FJavaClassObject::GetJString(Key));
}

float RemoteConfigLibraryAndroid::GetFloat(FString Key)
{
	return (float) FGMethodCallUtils::CallStaticDoubleMethod(FGRemoteConfigClassName, "GetDouble", "(Ljava/lang/String;)D",
		*FJavaClassObject::GetJString(Key));
}

int64 RemoteConfigLibraryAndroid::GetLong(FString Key)
{
	return (int64) FGMethodCallUtils::CallStaticLongMethod(FGRemoteConfigClassName, "GetLong", "(Ljava/lang/String;)J",
		*FJavaClassObject::GetJString(Key));
}

FString RemoteConfigLibraryAndroid::GetString(FString Key)
{
	return FGMethodCallUtils::CallStaticStringMethod(FGRemoteConfigClassName, "GetString", "(Ljava/lang/String;)Ljava/lang/String;",
		*FJavaClassObject::GetJString(Key));
}

void RemoteConfigLibraryAndroid::SetDefaults(const TMap<FString, UFGRemoteConfigParameterVariant*>& Values, const FOnRemoteConfigSuccess& OnSuccess,
	const FOnRemoteConfigError& OnError)
{
	UFGRemoteConfigCallback* NativeCallback = NewObject<UFGRemoteConfigCallback>();
	NativeCallback->AddToRoot();
	NativeCallback->BindSuccessDelegate(OnSuccess);
	NativeCallback->BindErrorDelegate(OnError);

	jobject HashMap = FGJavaConvertor::RemoteConfigParameterMapToJavaMap(Values);

	FGMethodCallUtils::CallStaticVoidMethod(FGRemoteConfigClassName, "SetDefaults", "(Landroid/app/Activity;JLjava/util/Map;)V",
		FJavaWrapper::GameActivityThis, (jlong) NativeCallback, HashMap);
}
