// Copyright (c) 2022 Nineva Studios

#include "CloudStorage/FGStorageMetadata.h"

#include "FirebaseGoodiesLog.h"

#if PLATFORM_ANDROID
#include "Android/Utils/FGJavaConvertor.h"
#include "Android/Utils/FGMethodCallUtils.h"
#endif

#if PLATFORM_IOS
#import <FirebaseStorage/FirebaseStorage.h>
#endif

UFGStorageMetadata::~UFGStorageMetadata()
{
#if PLATFORM_ANDROID
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	Env->DeleteGlobalRef(JavaMetadataObject);
	JavaMetadataObject = nullptr;
#elif PLATFORM_IOS
	(FIRStorageMetadata*)CFBridgingRelease(iosMetadata);
	iosMetadata = nil;
#endif
}

FString UFGStorageMetadata::GetContentType()
{
	FString result;

	if (!IsMetadataValid())
	{
		return result;
	}

#if PLATFORM_ANDROID
	result = FGMethodCallUtils::CallStringMethod(JavaMetadataObject, "getContentType", "()Ljava/lang/String;");
#elif PLATFORM_IOS
	result = FString(iosMetadata.contentType);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	result = desktopMetadata.content_type();
#endif

	return result;
}

TMap<FString, FString> UFGStorageMetadata::GetCustomMetadata()
{
	TMap<FString, FString> result;

	if (!IsMetadataValid())
	{
		return result;
	}

#if PLATFORM_ANDROID
	jobject javaMap = FGMethodCallUtils::CallStaticObjectMethod("com/ninevastudios/unrealfirebase/FGCloudStorage", "getCustomMetadata", "(Lcom/google/firebase/storage/StorageMetadata;)Ljava/util/HashMap;", JavaMetadataObject);

	result = FGJavaConvertor::FromJavaMap(javaMap);
#elif PLATFORM_IOS
	__block TMap<FString, FString> intermediateMapIos;

	NSDictionary<NSString*, NSString*>* customMetadata = iosMetadata.customMetadata;
	[customMetadata enumerateKeysAndObjectsUsingBlock:^(NSString* key, NSString* value, BOOL* stop) {
		intermediateMapIos.Add(FString(key), FString(value));
	}];

	result = intermediateMapIos;
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	for (auto pair : *desktopMetadata.custom_metadata())
	{
		result.Add(pair.first.c_str(), pair.second.c_str());
	}
#endif

	return result;
}

FString UFGStorageMetadata::GetPath()
{
	FString result;

	if (!IsMetadataValid())
	{
		return result;
	}

#if PLATFORM_ANDROID
	result = FGMethodCallUtils::CallStringMethod(JavaMetadataObject, "getPath", "()Ljava/lang/String;");
#elif PLATFORM_IOS
	result = FString(iosMetadata.path);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	result = desktopMetadata.path();
#endif

	return result;
}

FString UFGStorageMetadata::GetName()
{
	FString result;

	if (!IsMetadataValid())
	{
		return result;
	}
#if PLATFORM_ANDROID
	result = FGMethodCallUtils::CallStringMethod(JavaMetadataObject, "getName", "()Ljava/lang/String;");
#elif PLATFORM_IOS
	result = FString(iosMetadata.name);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	result = desktopMetadata.name();
#endif

	return result;
}

FString UFGStorageMetadata::GetBucket()
{
	FString result;

	if (!IsMetadataValid())
	{
		return result;
	}
#if PLATFORM_ANDROID
	result = FGMethodCallUtils::CallStringMethod(JavaMetadataObject, "getBucket", "()Ljava/lang/String;");
#elif PLATFORM_IOS
	result = FString(iosMetadata.bucket);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	result = desktopMetadata.bucket();
#endif

	return result;
}

FString UFGStorageMetadata::GetGeneration()
{
	FString result;

	if (!IsMetadataValid())
	{
		return result;
	}
#if PLATFORM_ANDROID
	result = FGMethodCallUtils::CallStringMethod(JavaMetadataObject, "getGeneration", "()Ljava/lang/String;");
#elif PLATFORM_IOS
	result = FString::Printf(TEXT("%d"), iosMetadata.generation);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	result = FString::Printf(TEXT("%d"), desktopMetadata.generation());
#endif

	return result;
}

FString UFGStorageMetadata::GetMetaGeneration()
{
	FString result;

	if (!IsMetadataValid())
	{
		return result;
	}
#if PLATFORM_ANDROID
	result = FGMethodCallUtils::CallStringMethod(JavaMetadataObject, "getMetadataGeneration", "()Ljava/lang/String;");
#elif PLATFORM_IOS
	result = FString::Printf(TEXT("%d"), iosMetadata.metageneration);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	result = FString::Printf(TEXT("%d"), desktopMetadata.metadata_generation());
#endif

	return result;
}

int64 UFGStorageMetadata::GetCreationTimeMillis()
{
	int64 result = -1;

	if (!IsMetadataValid())
	{
		return result;
	}
#if PLATFORM_ANDROID
	result = FGMethodCallUtils::CallLongMethod(JavaMetadataObject, "getCreationTimeMillis", "()J");
#elif PLATFORM_IOS
	result = [iosMetadata.timeCreated timeIntervalSince1970] * 1000;
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	result = desktopMetadata.creation_time();
#endif

	return result;
}

int64 UFGStorageMetadata::GetUpdatedTimeMillis()
{
	int64 result = -1;

	if (!IsMetadataValid())
	{
		return result;
	}
#if PLATFORM_ANDROID
	result = FGMethodCallUtils::CallLongMethod(JavaMetadataObject, "getUpdatedTimeMillis", "()J");
#elif PLATFORM_IOS
	result = [iosMetadata.updated timeIntervalSince1970] * 1000;
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	result = desktopMetadata.updated_time();
#endif

	return result;
}

int64 UFGStorageMetadata::GetSizeBytes()
{
	int64 result = -1;

	if (!IsMetadataValid())
	{
		return result;
	}
#if PLATFORM_ANDROID
	result = FGMethodCallUtils::CallLongMethod(JavaMetadataObject, "getSizeBytes", "()J");
#elif PLATFORM_IOS
	result = iosMetadata.size;
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	result = desktopMetadata.size_bytes();
#endif

	return result;
}

FString UFGStorageMetadata::GetMd5Hash()
{
	FString result;

	if (!IsMetadataValid())
	{
		return result;
	}
#if PLATFORM_ANDROID
	result = FGMethodCallUtils::CallStringMethod(JavaMetadataObject, "getMd5Hash", "()Ljava/lang/String;");
#elif PLATFORM_IOS
	result = FString(iosMetadata.md5Hash);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	result = desktopMetadata.md5_hash();
#endif

	return result;
}

FString UFGStorageMetadata::GetCacheControl()
{
	FString result;

	if (!IsMetadataValid())
	{
		return result;
	}
#if PLATFORM_ANDROID
	result = FGMethodCallUtils::CallStringMethod(JavaMetadataObject, "getCacheControl", "()Ljava/lang/String;");
#elif PLATFORM_IOS
	result = FString(iosMetadata.cacheControl);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	result = desktopMetadata.cache_control();
#endif

	return result;
}

FString UFGStorageMetadata::GetContentDisposition()
{
	FString result;

	if (!IsMetadataValid())
	{
		return result;
	}
#if PLATFORM_ANDROID
	result = FGMethodCallUtils::CallStringMethod(JavaMetadataObject, "getContentDisposition", "()Ljava/lang/String;");
#elif PLATFORM_IOS
	result = FString(iosMetadata.contentDisposition);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	result = desktopMetadata.content_disposition();
#endif

	return result;
}

FString UFGStorageMetadata::GetContentEncoding()
{
	FString result;

	if (!IsMetadataValid())
	{
		return result;
	}
#if PLATFORM_ANDROID
	result = FGMethodCallUtils::CallStringMethod(JavaMetadataObject, "getContentEncoding", "()Ljava/lang/String;");
#elif PLATFORM_IOS
	result = FString(iosMetadata.contentEncoding);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	result = desktopMetadata.content_encoding();
#endif

	return result;
}

FString UFGStorageMetadata::GetContentLanguage()
{
	FString result;

	if (!IsMetadataValid())
	{
		return result;
	}
#if PLATFORM_ANDROID
	result = FGMethodCallUtils::CallStringMethod(JavaMetadataObject, "getContentLanguage", "()Ljava/lang/String;");
#elif PLATFORM_IOS
	result = FString(iosMetadata.contentLanguage);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	result = desktopMetadata.content_language();
#endif

	return result;
}

bool UFGStorageMetadata::IsMetadataValid()
{
	bool bIsMetadataValid = false;
#if PLATFORM_ANDROID
	bIsMetadataValid = JavaMetadataObject != nullptr;
#elif PLATFORM_IOS
	bIsMetadataValid = iosMetadata != nil;
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	bIsMetadataValid = desktopMetadata.is_valid();
#endif
	if (!bIsMetadataValid)
	{
		UE_LOG(LogFirebaseGoodies, Error, TEXT("Metadata is not valid."));
	}
	return bIsMetadataValid;
}

#if PLATFORM_ANDROID
void UFGStorageMetadata::Init(jobject javaMetadata)
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	JavaMetadataObject = Env->NewGlobalRef(javaMetadata);
}
#endif

#if PLATFORM_IOS
void UFGStorageMetadata::Init(FIRStorageMetadata* metadata)
{
	iosMetadata = (FIRStorageMetadata*)CFBridgingRetain(metadata);
}
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
void UFGStorageMetadata::Init(firebase::storage::Metadata metadata)
{
	desktopMetadata = metadata;
}
#endif
