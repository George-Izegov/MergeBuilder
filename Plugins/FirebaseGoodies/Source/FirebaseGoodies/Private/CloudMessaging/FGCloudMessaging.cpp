// Copyright (c) 2022 Nineva Studios

#include "CloudMessaging/FGCloudMessaging.h"
#include "CloudMessaging/FGRemoteMessage.h"

#include "Async/Async.h"

#include "FirebaseGoodiesLog.h"

#if PLATFORM_ANDROID
#include "Android/Utils/FGJavaConvertor.h"
#include "Android/Utils/FGMethodCallUtils.h"
#endif

#if PLATFORM_IOS
#import <FirebaseMessaging/FirebaseMessaging.h>
#import <FirebaseInstallations/FirebaseInstallations.h>
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
#include "firebase/messaging.h"
#include "firebase/installations.h"
#endif

FCloudMessagingTwoStringDelegate UFGCloudMessaging::OnInstanceIdDataReceivedCallback;
FCloudMessagingStringDelegate UFGCloudMessaging::OnInstanceIdDataReceiveErrorCallback;

FCloudMessagingVoidDelegate UFGCloudMessaging::OnTopicOperationSuccessCallback;
FCloudMessagingStringDelegate UFGCloudMessaging::OnTopicOperationErrorCallback;

FCloudMessagingMessageDelegate UFGCloudMessaging::OnMessageReceivedCallback;
FCloudMessagingVoidDelegate UFGCloudMessaging::OnMessagesDeletedCallback;
FCloudMessagingStringDelegate UFGCloudMessaging::OnMessageSentCallback;
FCloudMessagingTwoStringDelegate UFGCloudMessaging::OnMessageSendErrorCallback;
FCloudMessagingStringDelegate UFGCloudMessaging::OnNewTokenCallback;

FCloudMessagingRemoteNotificationDelegate UFGCloudMessaging::OnRemoteNotificationReceivedCallback;

const ANSICHAR* UFGCloudMessaging::FGCloudMessagingClassName = "com/ninevastudios/unrealfirebase/FGCloudMessaging";

void UFGCloudMessaging::GetInstanceIdData(const FCloudMessagingTwoStringDelegate& OnSuccess,
                                          const FCloudMessagingStringDelegate& OnError)
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGCloudMessaging::GetInstanceIdData"));

	OnInstanceIdDataReceivedCallback = OnSuccess;
	OnInstanceIdDataReceiveErrorCallback = OnError;
#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(FGCloudMessagingClassName, "getInstanceIdData", "()V");
#elif PLATFORM_IOS
	[[FIRMessaging messaging] tokenWithCompletion:^(NSString *token, NSError *tokenError) {
		if (tokenError != nil) {
			FString ErrorMessage = FString(tokenError.localizedDescription);
			AsyncTask(ENamedThreads::GameThread, [=]() {
				 UFGCloudMessaging::OnInstanceIdDataReceiveErrorCallback.ExecuteIfBound(ErrorMessage);
			});
		} else {
			[[FIRInstallations installations] installationIDWithCompletion:^(NSString *identifier, NSError *error) {
				 if (error != nil) {
					FString ErrorMessage = FString(error.localizedDescription);
					AsyncTask(ENamedThreads::GameThread, [=]() {
						UFGCloudMessaging::OnInstanceIdDataReceiveErrorCallback.ExecuteIfBound(ErrorMessage);
					});
				 } else {
					 FString Token = FString(token);
					 FString Id = FString(identifier);
					 AsyncTask(ENamedThreads::GameThread, [=]() {
						  UFGCloudMessaging::OnInstanceIdDataReceivedCallback.ExecuteIfBound(Token, Id);
					 });
				}
			}];
	  }
	}];
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	auto task = firebase::messaging::GetToken();
	task.AddOnCompletion([=](const ::firebase::Future<std::string>& callback)
	{
		if (callback.status() == firebase::FutureStatus::kFutureStatusComplete)
		{
			FString Token = callback.result()->c_str();

			auto installations = firebase::installations::Installations::GetInstance(firebase::App::GetInstance());
			auto instanceIdTask = installations->GetId();
			instanceIdTask.AddOnCompletion([=](const ::firebase::Future<std::string>& callback)
			{
				if (callback.status() == firebase::FutureStatus::kFutureStatusComplete)
				{
					FString InstanceId = callback.result()->c_str();

					AsyncTask(ENamedThreads::GameThread, [=]()
					{
						OnInstanceIdDataReceivedCallback.ExecuteIfBound(Token, InstanceId);
					});
				}
				else
				{
					FString ErrorMessage = FString(callback.error_message());
					UE_LOG(LogFirebaseGoodies, Error, TEXT("GetInstanceIdData error: %s"), *ErrorMessage);

					AsyncTask(ENamedThreads::GameThread, [=]()
					{
						OnInstanceIdDataReceiveErrorCallback.ExecuteIfBound(ErrorMessage);
					});
				}
			});
		}
		else
		{
			FString ErrorMessage = FString(callback.error_message());
			UE_LOG(LogFirebaseGoodies, Error, TEXT("GetInstanceIdData error: %s"), *ErrorMessage);

			AsyncTask(ENamedThreads::GameThread, [=]()
			{
				OnInstanceIdDataReceiveErrorCallback.ExecuteIfBound(ErrorMessage);
			});
		}
	});
#endif
}

void UFGCloudMessaging::DeleteInstanceId()
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGCloudMessaging::DeleteInstanceId"));

#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(FGCloudMessagingClassName, "deleteInstanceId", "()V");
#elif PLATFORM_IOS
	[[FIRInstallations installations] deleteWithCompletion:^(NSError *error) {
		if (error != nil) {
			UE_LOG(LogFirebaseGoodies, Error, TEXT("DeleteInstanceId failed with error: %s"), *(FString(error.localizedDescription)));
		} else {
			UE_LOG(LogFirebaseGoodies, Verbose, TEXT("DeleteInstanceId was successful"));
		}
	}];
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	auto installations = firebase::installations::Installations::GetInstance(firebase::App::GetInstance());
	auto task = installations->Delete();
	task.AddOnCompletion([=](const ::firebase::Future<void>& callback)
	{
		if (callback.status() == firebase::FutureStatus::kFutureStatusComplete)
		{
			UE_LOG(LogFirebaseGoodies, Error, TEXT("DeleteInstanceId success!"));
		}
		else
		{
			FString ErrorMessage = FString(callback.error_message());
			UE_LOG(LogFirebaseGoodies, Error, TEXT("DeleteInstanceId error: %s"), *ErrorMessage);
		}
	});
#endif
}

bool UFGCloudMessaging::IsAutoInitEnabled()
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGCloudMessaging::IsAutoInitEnabled"));

	bool Result = false;

#if PLATFORM_ANDROID
	Result = FGMethodCallUtils::CallStaticBoolMethod(FGCloudMessagingClassName, "isAutoInitEnabled", "()Z");
#elif PLATFORM_IOS
	Result = [FIRMessaging messaging].isAutoInitEnabled;
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	Result = firebase::messaging::IsTokenRegistrationOnInitEnabled();
#endif

	return Result;
}

void UFGCloudMessaging::SetAutoInitEnabled(bool Enabled)
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGCloudMessaging::SetAutoInitEnabled"));

#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(FGCloudMessagingClassName, "setAutoInitEnabled", "(Z)V", Enabled);
#elif PLATFORM_IOS
	[FIRMessaging messaging].autoInitEnabled = Enabled;
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	firebase::messaging::SetTokenRegistrationOnInitEnabled(Enabled);
#endif
}

void UFGCloudMessaging::SubscribeToTopic(const FString& Topic, const FCloudMessagingVoidDelegate& OnSuccess,
                                         const FCloudMessagingStringDelegate& OnError)
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGCloudMessaging::SubscribeToTopic"));

	OnTopicOperationSuccessCallback = OnSuccess;
	OnTopicOperationErrorCallback = OnError;

#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(FGCloudMessagingClassName, "subscribeToTopic", "(Ljava/lang/String;)V", *FJavaClassObject::GetJString(Topic));
#elif PLATFORM_IOS
	[[FIRMessaging messaging] subscribeToTopic:Topic.GetNSString()
									completion:^(NSError * _Nullable error) {
		if (error != nil) {
			FString ErrorMessage = FString(error.localizedDescription);
			
			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGCloudMessaging::OnTopicOperationErrorCallback.ExecuteIfBound(ErrorMessage);
			});
		} else {
			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGCloudMessaging::OnTopicOperationSuccessCallback.ExecuteIfBound();
			});
		}
	}];
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	auto task = firebase::messaging::Subscribe(TCHAR_TO_ANSI(*Topic));
	task.AddOnCompletion([=](const ::firebase::Future<void>& callback)
	{
		if (callback.status() == firebase::FutureStatus::kFutureStatusComplete)
		{
			AsyncTask(ENamedThreads::GameThread, [=]()
			{
				OnTopicOperationSuccessCallback.ExecuteIfBound();
			});
		}
		else
		{
			FString ErrorMessage = FString(callback.error_message());
			UE_LOG(LogFirebaseGoodies, Error, TEXT("SubscribeToTopic error: %s"), *ErrorMessage);

			AsyncTask(ENamedThreads::GameThread, [=]()
			{
				OnTopicOperationErrorCallback.ExecuteIfBound(ErrorMessage);
			});
		}
	});
#endif
}

void UFGCloudMessaging::UnSubscribeFromTopic(const FString& Topic, const FCloudMessagingVoidDelegate& OnSuccess,
                                             const FCloudMessagingStringDelegate& OnError)
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGCloudMessaging::UnSubscribeFromTopic"));

	OnTopicOperationSuccessCallback = OnSuccess;
	OnTopicOperationErrorCallback = OnError;

#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(FGCloudMessagingClassName, "unsubscribeFromTopic", "(Ljava/lang/String;)V", *FJavaClassObject::GetJString(Topic));
#elif PLATFORM_IOS
	[[FIRMessaging messaging] unsubscribeFromTopic:Topic.GetNSString()
										completion:^(NSError * _Nullable error) {
		if (error != nil) {
			FString ErrorMessage = FString(error.localizedDescription);
			
			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGCloudMessaging::OnTopicOperationErrorCallback.ExecuteIfBound(ErrorMessage);
			});
		} else {
			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGCloudMessaging::OnTopicOperationSuccessCallback.ExecuteIfBound();
			});
		}
	}];
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	auto task = firebase::messaging::Unsubscribe(TCHAR_TO_ANSI(*Topic));
	task.AddOnCompletion([=](const ::firebase::Future<void>& callback)
	{
		if (callback.status() == firebase::FutureStatus::kFutureStatusComplete)
		{
			AsyncTask(ENamedThreads::GameThread, [=]()
			{
				OnTopicOperationSuccessCallback.ExecuteIfBound();
			});
		}
		else
		{
			FString ErrorMessage = FString(callback.error_message());
			UE_LOG(LogFirebaseGoodies, Error, TEXT("SubscribeToTopic error: %s"), *ErrorMessage);

			AsyncTask(ENamedThreads::GameThread, [=]()
			{
				OnTopicOperationErrorCallback.ExecuteIfBound(ErrorMessage);
			});
		}
	});
#endif
}

#if PLATFORM_ANDROID

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGCloudMessaging_OnTokenReceived(JNIEnv* env, jclass clazz, jstring instanceToken, jstring instanceId)
{
	FString Token = FGJavaConvertor::FromJavaString(instanceToken);
	FString Id = FGJavaConvertor::FromJavaString(instanceId);
	
	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGCloudMessaging::OnInstanceIdDataReceivedCallback.ExecuteIfBound(Token, Id);
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGCloudMessaging_OnTokenReceiveError(JNIEnv* env, jclass clazz, jstring error)
{
	FString ErrorMessage = FGJavaConvertor::FromJavaString(error);
	
	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGCloudMessaging::OnInstanceIdDataReceiveErrorCallback.ExecuteIfBound(ErrorMessage);
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGCloudMessaging_OnSubscribedToTopic(JNIEnv* env, jclass clazz)
{
	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGCloudMessaging::OnTopicOperationSuccessCallback.ExecuteIfBound();
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGCloudMessaging_OnSubscribeToTopicError(JNIEnv* env, jclass clazz, jstring error)
{
	FString ErrorMessage = FGJavaConvertor::FromJavaString(error);
	
	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGCloudMessaging::OnTopicOperationErrorCallback.ExecuteIfBound(ErrorMessage);
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGCloudMessaging_OnUnsubscribedFromTopic(JNIEnv* env, jclass clazz)
{
	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGCloudMessaging::OnTopicOperationSuccessCallback.ExecuteIfBound();
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGCloudMessaging_OnUnsubscribeFromTopicError(JNIEnv* env, jclass clazz, jstring error)
{
	FString ErrorMessage = FGJavaConvertor::FromJavaString(error);
	
	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGCloudMessaging::OnTopicOperationErrorCallback.ExecuteIfBound(ErrorMessage);
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGMessageReceiver_OnMessageReceived(JNIEnv* env, jclass clazz, jobject message)
{
	UFGRemoteMessage* Message = NewObject<UFGRemoteMessage>();
	Message->Init(message);
	
	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGCloudMessaging::OnMessageReceivedCallback.ExecuteIfBound(Message);
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGMessageReceiver_OnDeletedMessages(JNIEnv* env, jclass clazz)
{
	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGCloudMessaging::OnMessagesDeletedCallback.ExecuteIfBound();
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGMessageReceiver_OnMessageSent(JNIEnv* env, jclass clazz, jstring msgId)
{
	FString MessageId = FGJavaConvertor::FromJavaString(msgId);
	
	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGCloudMessaging::OnMessageSentCallback.ExecuteIfBound(MessageId);
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGMessageReceiver_OnSendError(JNIEnv* env, jclass clazz, jstring msgId, jstring error)
{
	FString ErrorMessage = FGJavaConvertor::FromJavaString(error);
	FString MessageId = FGJavaConvertor::FromJavaString(msgId);
	
	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGCloudMessaging::OnMessageSendErrorCallback.ExecuteIfBound(MessageId, ErrorMessage);
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGMessageReceiver_OnNewToken(JNIEnv* env, jclass clazz, jstring token)
{
	FString Token = FGJavaConvertor::FromJavaString(token);
	
	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGCloudMessaging::OnNewTokenCallback.ExecuteIfBound(Token);
	});
}

#endif
