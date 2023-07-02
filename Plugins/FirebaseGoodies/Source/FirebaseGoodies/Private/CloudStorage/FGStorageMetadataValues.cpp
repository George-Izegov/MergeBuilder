// Copyright (c) 2022 Nineva Studios

#include "CloudStorage/FGStorageMetadataValues.h"

#include "FirebaseGoodiesLog.h"

#if PLATFORM_ANDROID
#include "Android/Utils/FGJavaConvertor.h"
#include "Android/Utils/FGMethodCallUtils.h"
#endif

#if PLATFORM_IOS
#import <FirebaseStorage/FirebaseStorage.h>
#endif

UFGStorageMetadataValues::~UFGStorageMetadataValues()
{
#if PLATFORM_ANDROID
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	Env->DeleteLocalRef(JavaMetadataBuilderObject);
	JavaMetadataBuilderObject = nullptr;
#elif PLATFORM_IOS
	(FIRStorageMetadata*)CFBridgingRelease(iosMetadata);
	iosMetadata = nil;
#endif
}

bool UFGStorageMetadataValues::IsMetadataValid() const
{
	bool bIsMetadataValid = false;
#if PLATFORM_ANDROID
	bIsMetadataValid = JavaMetadataBuilderObject != nullptr;
#elif PLATFORM_IOS
	bIsMetadataValid = iosMetadata != nil;
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	bIsMetadataValid = desktopMetadata.is_valid();
#endif
	if (!bIsMetadataValid)
	{
		UE_LOG(LogFirebaseGoodies, Error, TEXT("Metadata object is not valid."));
	}

	return bIsMetadataValid;
}

UFGStorageMetadataValues* UFGStorageMetadataValues::NewStorageMetadataValues()
{
	UFGStorageMetadataValues* values = NewObject<UFGStorageMetadataValues>();
#if PLATFORM_ANDROID
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();

	jobject MetadataBuilder = FGMethodCallUtils::CallStaticObjectMethod("com/ninevastudios/unrealfirebase/FGCloudStorage", "getMetadataBuilder", "()Lcom/google/firebase/storage/StorageMetadata$Builder;");

	values->JavaMetadataBuilderObject = MetadataBuilder;
#elif PLATFORM_IOS
	FIRStorageMetadata* metadata = [[FIRStorageMetadata alloc] init];
	values->iosMetadata = (FIRStorageMetadata*)CFBridgingRetain(metadata);
#endif

	return values;
}

void UFGStorageMetadataValues::SetCacheControl(const FString& Value)
{
	if (!IsMetadataValid()) return;

#if PLATFORM_ANDROID
	jstring javaValue = *FJavaClassObject::GetJString(Value);
	JavaMetadataBuilderObject = FGMethodCallUtils::CallObjectMethod(JavaMetadataBuilderObject, "setCacheControl", "(Ljava/lang/String;)Lcom/google/firebase/storage/StorageMetadata$Builder;", javaValue);
#elif PLATFORM_IOS
	iosMetadata.cacheControl = Value.GetNSString();
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	desktopMetadata.set_cache_control(TCHAR_TO_ANSI(*Value));
#endif
}

void UFGStorageMetadataValues::SetContentDisposition(const FString& Value)
{
	if (!IsMetadataValid()) return;

#if PLATFORM_ANDROID
	jstring javaValue = *FJavaClassObject::GetJString(Value);
	JavaMetadataBuilderObject = FGMethodCallUtils::CallObjectMethod(JavaMetadataBuilderObject, "setContentDisposition", "(Ljava/lang/String;)Lcom/google/firebase/storage/StorageMetadata$Builder;", javaValue);
#elif PLATFORM_IOS
	iosMetadata.contentDisposition = Value.GetNSString();
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	desktopMetadata.set_content_disposition(TCHAR_TO_ANSI(*Value));
#endif
}

void UFGStorageMetadataValues::SetContentEncoding(const FString& Value)
{
	if (!IsMetadataValid()) return;

#if PLATFORM_ANDROID
	jstring javaValue = *FJavaClassObject::GetJString(Value);
	JavaMetadataBuilderObject = FGMethodCallUtils::CallObjectMethod(JavaMetadataBuilderObject, "setContentEncoding", "(Ljava/lang/String;)Lcom/google/firebase/storage/StorageMetadata$Builder;", javaValue);
#elif PLATFORM_IOS
	iosMetadata.contentEncoding = Value.GetNSString();
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	desktopMetadata.set_content_encoding(TCHAR_TO_ANSI(*Value));
#endif
}

void UFGStorageMetadataValues::SetContentLanguage(const FString& Value)
{
	if (!IsMetadataValid()) return;

#if PLATFORM_ANDROID
	jstring javaValue = *FJavaClassObject::GetJString(Value);
	JavaMetadataBuilderObject = FGMethodCallUtils::CallObjectMethod(JavaMetadataBuilderObject, "setContentLanguage", "(Ljava/lang/String;)Lcom/google/firebase/storage/StorageMetadata$Builder;", javaValue);
#elif PLATFORM_IOS
	iosMetadata.contentLanguage = Value.GetNSString();
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	desktopMetadata.set_content_language(TCHAR_TO_ANSI(*Value));
#endif
}

void UFGStorageMetadataValues::SetContentType(const FString& Value)
{
	if (!IsMetadataValid()) return;

#if PLATFORM_ANDROID
	jstring javaValue = *FJavaClassObject::GetJString(Value);
	JavaMetadataBuilderObject = FGMethodCallUtils::CallObjectMethod(JavaMetadataBuilderObject, "setContentType", "(Ljava/lang/String;)Lcom/google/firebase/storage/StorageMetadata$Builder;", javaValue);
#elif PLATFORM_IOS
	iosMetadata.contentType = Value.GetNSString();
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	desktopMetadata.set_content_type(TCHAR_TO_ANSI(*Value));
#endif
}

void UFGStorageMetadataValues::SetCustomMetadata(const TMap<FString, FString>& Values)
{
	if (!IsMetadataValid()) return;

#if PLATFORM_ANDROID
	for (const auto& DataPair : Values)
	{
		jstring Key = *FJavaClassObject::GetJString(DataPair.Key);
		jstring Value = *FJavaClassObject::GetJString(DataPair.Value);

		JavaMetadataBuilderObject = FGMethodCallUtils::CallObjectMethod(JavaMetadataBuilderObject, "setCustomMetadata", "(Ljava/lang/String;Ljava/lang/String;)Lcom/google/firebase/storage/StorageMetadata$Builder;", Key, Value);
	}
#elif PLATFORM_IOS
	NSMutableDictionary<NSString*, NSString*>* dict = [NSMutableDictionary dictionary];

	for (const auto& DataPair : Values)
	{
		NSString* Key = DataPair.Key.GetNSString();
		NSString* Value = DataPair.Value.GetNSString();
		dict[Key] = Value;
	}

	iosMetadata.customMetadata = dict;
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	auto metadata = desktopMetadata.custom_metadata();
	metadata->clear();

	for (auto Pair : Values)
	{
		metadata->insert({ TCHAR_TO_ANSI(*Pair.Key), TCHAR_TO_ANSI(*Pair.Value) });
	}
#endif
}
