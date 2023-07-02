// Copyright (c) 2022 Nineva Studios

#include "CloudMessaging/FGRemoteMessage.h"
#include "CloudMessaging/FGRemoteMessageNotification.h"

#include "FirebaseGoodiesLog.h"

#if PLATFORM_ANDROID
#include "Android/Utils/FGJavaConvertor.h"
#include "Android/Utils/FGMethodCallUtils.h"
#endif

const ANSICHAR* UFGRemoteMessage::FGRemoteMessageClassName = "com/google/firebase/messaging/RemoteMessage";

UFGRemoteMessage::~UFGRemoteMessage()
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGRemoteMessage::~UFGRemoteMessage"));
	
	if (!IsMessageValid())
	{
		return;
	}
	
#if PLATFORM_ANDROID
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	Env->DeleteGlobalRef(JavaRemoteMessageObject);
	JavaRemoteMessageObject = nullptr;
#endif
}

FString UFGRemoteMessage::GetCollapseKey()
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGRemoteMessage::GetCollapseKey"));
	
	FString result;
	
	if (!IsMessageValid())
	{
		return result;
	}

#if PLATFORM_ANDROID
	result = FGMethodCallUtils::CallStringMethod(JavaRemoteMessageObject, "getCollapseKey", "()Ljava/lang/String;");
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	result = Message.collapse_key.c_str();
#endif

	return result;
}

TMap<FString, FString> UFGRemoteMessage::GetData()
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGRemoteMessage::GetData"));
	
	TMap<FString, FString> result;
	
	if (!IsMessageValid())
	{
		return result;
	}

#if PLATFORM_ANDROID
	jobject javaMap = FGMethodCallUtils::CallStaticObjectMethod("com/ninevastudios/unrealfirebase/FGCloudMessaging", "getData", "(Lcom/google/firebase/messaging/RemoteMessage;)Ljava/util/HashMap;", JavaRemoteMessageObject);
	result = FGJavaConvertor::FromJavaMap(javaMap);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	for (const auto& entry : Message.data)
	{
		result.Add(entry.first.c_str(), entry.second.c_str());
	}
#endif

	return result;
}

FString UFGRemoteMessage::GetFrom()
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGRemoteMessage::GetFrom"));
	
	FString result;
	
	if (!IsMessageValid())
	{
		return result;
	}

#if PLATFORM_ANDROID
	result = FGMethodCallUtils::CallStringMethod(JavaRemoteMessageObject, "getFrom", "()Ljava/lang/String;");
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	result = Message.from.c_str();
#endif

	return result;
}

FString UFGRemoteMessage::GetMessageId()
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGRemoteMessage::GetMessageId"));
	
	FString result;
	
	if (!IsMessageValid())
	{
		return result;
	}

#if PLATFORM_ANDROID
	result = FGMethodCallUtils::CallStringMethod(JavaRemoteMessageObject, "getMessageId", "()Ljava/lang/String;");
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	result = Message.message_id.c_str();
#endif

	return result;
}

FString UFGRemoteMessage::GetMessageType()
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGRemoteMessage::GetMessageType"));
	
	FString result;
	
	if (!IsMessageValid())
	{
		return result;
	}

#if PLATFORM_ANDROID
	result = FGMethodCallUtils::CallStringMethod(JavaRemoteMessageObject, "getMessageType", "()Ljava/lang/String;");
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	result = Message.message_type.c_str();
#endif

	return result;
}

UFGRemoteMessageNotification* UFGRemoteMessage::GetNotification()
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGRemoteMessage::GetNotification"));
	
	UFGRemoteMessageNotification* result = NewObject<UFGRemoteMessageNotification>();
	
	if (!IsMessageValid())
	{
		return result;
	}
	
#if PLATFORM_ANDROID
	jobject javaNotification = FGMethodCallUtils::CallObjectMethod(JavaRemoteMessageObject, "getNotification", "()Lcom/google/firebase/messaging/RemoteMessage$Notification;");
	result->Init(javaNotification);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	result->Init(Message.notification);
#endif

	return result;
}

EFGRemoteMessagePriority UFGRemoteMessage::GetOriginalPriority()
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGRemoteMessage::GetOriginalPriority"));
	
	EFGRemoteMessagePriority result = EFGRemoteMessagePriority::Unknown;
	
	if (!IsMessageValid())
	{
		return result;
	}

#if PLATFORM_ANDROID
	result = (EFGRemoteMessagePriority) FGMethodCallUtils::CallIntMethod(JavaRemoteMessageObject, "getOriginalPriority", "()I");
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	FString PriorityString = Message.original_priority.c_str();

	if (PriorityString.ToLower() == "normal")
	{
		result = EFGRemoteMessagePriority::Normal;
	}
	else if (PriorityString.ToLower() == "high")
	{
		result = EFGRemoteMessagePriority::High;
	}
#endif

	return result;
}

EFGRemoteMessagePriority UFGRemoteMessage::GetPriority()
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGRemoteMessage::GetPriority"));
	
	EFGRemoteMessagePriority result = EFGRemoteMessagePriority::Unknown;
	
	if (!IsMessageValid())
	{
		return result;
	}

#if PLATFORM_ANDROID
	result = (EFGRemoteMessagePriority) FGMethodCallUtils::CallIntMethod(JavaRemoteMessageObject, "getPriority", "()I");
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	FString PriorityString = Message.priority.c_str();

	if (PriorityString.ToLower() == "normal")
	{
		result = EFGRemoteMessagePriority::Normal;
	}
	else if (PriorityString.ToLower() == "high")
	{
		result = EFGRemoteMessagePriority::High;
	}
#endif

	return result;
}

FDateTime UFGRemoteMessage::GetSentTime()
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGRemoteMessage::GetSentTime"));
	
	FDateTime result = FDateTime::MinValue();
	
	if (!IsMessageValid())
	{
		return result;
	}

#if PLATFORM_ANDROID
	result = FDateTime::FromUnixTimestamp(FGMethodCallUtils::CallLongMethod(JavaRemoteMessageObject, "getSentTime", "()J"));
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	result = FDateTime::FromUnixTimestamp(Message.sent_time);
#endif

	return result;
}

FString UFGRemoteMessage::GetTo()
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGRemoteMessage::GetTo"));
	
	FString result;
	
	if (!IsMessageValid())
	{
		return result;
	}

#if PLATFORM_ANDROID
	result = FGMethodCallUtils::CallStringMethod(JavaRemoteMessageObject, "getTo", "()Ljava/lang/String;");
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	result = Message.to.c_str();
#endif

	return result;
}

int UFGRemoteMessage::GetTtl()
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGRemoteMessage::GetTtl"));
	
	int result = -1;
	
	if (!IsMessageValid())
	{
		return result;
	}

#if PLATFORM_ANDROID
	result = FGMethodCallUtils::CallIntMethod(JavaRemoteMessageObject, "getTtl", "()I");
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	result = Message.time_to_live;
#endif

	return result;
}

bool UFGRemoteMessage::IsMessageValid()
{
	bool bIsMessageValid = false;
	
#if PLATFORM_ANDROID
	bIsMessageValid = JavaRemoteMessageObject != nullptr;
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	bIsMessageValid = true;
#endif
#if !WITH_EDITOR
	if (!bIsMessageValid)
	{
		UE_LOG(LogFirebaseGoodies, Error, TEXT("Message native object is not valid."));
	}
#endif
	return bIsMessageValid;
}

#if PLATFORM_ANDROID
void UFGRemoteMessage::Init(jobject javaRemoteMessage)
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	JavaRemoteMessageObject = Env->NewGlobalRef(javaRemoteMessage);
}
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
void UFGRemoteMessage::Init(firebase::messaging::Message message)
{
	Message = message;
}
#endif
