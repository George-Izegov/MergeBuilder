// Copyright (c) 2022 Nineva Studios

#include "Crashlytics/FGCrashlytics.h"
#include "FirebaseGoodiesLog.h"

#if PLATFORM_ANDROID
#include "Android/Utils/FGJavaConvertor.h"
#include "Android/Utils/FGMethodCallUtils.h"
#endif

#if PLATFORM_IOS
#import <FirebaseCrashlytics/FirebaseCrashlytics.h>
#endif

FCrashlyticsBoolDelegate UFGCrashlytics::OnCheckForUnsentReportsSuccess;
FCrashlyticsStringDelegate UFGCrashlytics::OnCheckForUnsentReportsError;

const ANSICHAR* UFGCrashlytics::FGCrashlyticsClassName = "com/ninevastudios/unrealfirebase/FGCrashlytics";

void UFGCrashlytics::RecordException(const FString& Message)
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGCrashlytics::RecordException"));
	
#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(FGCrashlyticsClassName, "recordException", "(Ljava/lang/String;)V", *FJavaClassObject::GetJString(Message));
#elif PLATFORM_IOS
	UE_LOG(LogFirebaseGoodies, Warning, TEXT("UFGCrashlytics::RecordException is not availbale for iOS. Calling UFGCrashlytics::LogMessage"));
	LogMessage(Message);
#endif
}

void UFGCrashlytics::LogMessage(const FString& Message)
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGCrashlytics::LogMessage"));
	
#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(FGCrashlyticsClassName, "log", "(Ljava/lang/String;)V", *FJavaClassObject::GetJString(Message));
#elif PLATFORM_IOS
	NSError* error = [NSError errorWithDomain:@"com.firebase.crashlytics"
										 code:147
									 userInfo:@{NSLocalizedFailureReasonErrorKey:Message.GetNSString()}];
	[[FIRCrashlytics crashlytics] recordError:error];
#endif
}

void UFGCrashlytics::SetUserId(const FString& Identifier)
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGCrashlytics::SetUserId"));
	
#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(FGCrashlyticsClassName, "setUserId", "(Ljava/lang/String;)V", *FJavaClassObject::GetJString(Identifier));
#elif PLATFORM_IOS
	[[FIRCrashlytics crashlytics] setUserID:Identifier.GetNSString()];
#endif
}

void UFGCrashlytics::SetCustomBoolKey(const FString& Key, bool Value)
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGCrashlytics::SetCustomBoolKey"));
	
#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(FGCrashlyticsClassName, "setCustomKey", "(Ljava/lang/String;Z)V", *FJavaClassObject::GetJString(Key), Value);
#elif PLATFORM_IOS
	[[FIRCrashlytics crashlytics] setCustomValue:@(Value) forKey:Key.GetNSString()];
#endif
}

void UFGCrashlytics::SetCustomFloatKey(const FString& Key, float Value)
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGCrashlytics::SetCustomFloatKey"));
	
#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(FGCrashlyticsClassName, "setCustomKey", "(Ljava/lang/String;F)V", *FJavaClassObject::GetJString(Key), Value);
#elif PLATFORM_IOS
	[[FIRCrashlytics crashlytics] setCustomValue:@(Value) forKey:Key.GetNSString()];
#endif
}

void UFGCrashlytics::SetCustomIntKey(const FString& Key, int Value)
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGCrashlytics::SetCustomIntKey"));
	
#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(FGCrashlyticsClassName, "setCustomKey", "(Ljava/lang/String;I)V", *FJavaClassObject::GetJString(Key), Value);
#elif PLATFORM_IOS
	[[FIRCrashlytics crashlytics] setCustomValue:@(Value) forKey:Key.GetNSString()];
#endif
}

void UFGCrashlytics::SetCustomLongKey(const FString& Key, int64 Value)
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGCrashlytics::SetCustomLongKey"));
	
#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(FGCrashlyticsClassName, "setCustomKey", "(Ljava/lang/String;J)V", *FJavaClassObject::GetJString(Key), Value);
#elif PLATFORM_IOS
	[[FIRCrashlytics crashlytics] setCustomValue:@(Value) forKey:Key.GetNSString()];
#endif
}

void UFGCrashlytics::SetCustomStringKey(const FString& Key, const FString& Value)
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGCrashlytics::SetCustomStringKey"));
	
#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(FGCrashlyticsClassName, "setCustomKey", "(Ljava/lang/String;Ljava/lang/String;)V", *FJavaClassObject::GetJString(Key), *FJavaClassObject::GetJString(Value));
#elif PLATFORM_IOS
	[[FIRCrashlytics crashlytics] setCustomValue:Value.GetNSString() forKey:Key.GetNSString()];
#endif
}

void UFGCrashlytics::CheckForUnsentReports(const FCrashlyticsBoolDelegate& OnSuccess, const FCrashlyticsStringDelegate& OnError)
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGCrashlytics::CheckForUnsentReports"));
	
	OnCheckForUnsentReportsSuccess = OnSuccess;
	OnCheckForUnsentReportsError = OnError;
	
#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(FGCrashlyticsClassName, "checkForUnsentReports", "()V");
#elif PLATFORM_IOS
	[[FIRCrashlytics crashlytics] checkForUnsentReportsWithCompletion:^(BOOL result) {
		AsyncTask(ENamedThreads::GameThread, [=]() {
			UFGCrashlytics::OnCheckForUnsentReportsSuccess.ExecuteIfBound(result);
		});
	}];
#endif
}

void UFGCrashlytics::SendUnsentReports()
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGCrashlytics::SendUnsentReports"));
	
#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(FGCrashlyticsClassName, "sendUnsentReports", "()V");
#elif PLATFORM_IOS
	[[FIRCrashlytics crashlytics] sendUnsentReports];
#endif
}

void UFGCrashlytics::DeleteUnsentReports()
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGCrashlytics::DeleteUnsentReports"));
	
#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(FGCrashlyticsClassName, "deleteUnsentReports", "()V");
#elif PLATFORM_IOS
	[[FIRCrashlytics crashlytics] deleteUnsentReports];
#endif
}

bool UFGCrashlytics::DidCrashOnPreviousExecution()
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGCrashlytics::DidCrashOnPreviousExecution"));
	
	bool Result = false;
	
#if PLATFORM_ANDROID
	Result = FGMethodCallUtils::CallStaticBoolMethod(FGCrashlyticsClassName, "didCrashOnPreviousExecution", "()Z");
#elif PLATFORM_IOS
	Result = [[FIRCrashlytics crashlytics] didCrashDuringPreviousExecution];
#endif
	
	return Result;
}

void UFGCrashlytics::SetCrashlyticsCollectionEnabled(bool Enabled)
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGCrashlytics::SetCrashlyticsCollectionEnabled"));
	
#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(FGCrashlyticsClassName, "setCrashlyticsCollectionEnabled", "(Z)V", Enabled);
#elif PLATFORM_IOS
	[[FIRCrashlytics crashlytics] setCrashlyticsCollectionEnabled:Enabled];
#endif
}

#if PLATFORM_ANDROID

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGCrashlytics_OnCheckForUnsentUserReportsSuccess(JNIEnv* env, jclass clazz, jboolean result)
{
	bool Result = static_cast<bool>(result);
	
	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGCrashlytics::OnCheckForUnsentReportsSuccess.ExecuteIfBound(Result);
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGCrashlytics_OnCheckForUnsentUserReportsError(JNIEnv* env, jclass clazz, jstring error)
{
	FString ErrorMessage = FGJavaConvertor::FromJavaString(error);
	
	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGCrashlytics::OnCheckForUnsentReportsError.ExecuteIfBound(ErrorMessage);
	});
}

#endif
