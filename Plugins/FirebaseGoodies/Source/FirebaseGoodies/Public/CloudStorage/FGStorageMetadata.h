// Copyright (c) 2022 Nineva Studios

#pragma once

#if PLATFORM_ANDROID
#include "Android/AndroidJNI.h"
#endif

#if PLATFORM_IOS
@class FIRStorageMetadata;
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
#include "firebase/storage.h"
#endif

#include "FGStorageMetadata.generated.h"

UCLASS(BlueprintType)
class FIREBASEGOODIES_API UFGStorageMetadata : public UObject
{
	GENERATED_BODY()

	virtual ~UFGStorageMetadata();

public:
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Storage|Storage Metadata")
	FString GetContentType();

	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Storage|Storage Metadata")
	TMap<FString, FString> GetCustomMetadata();

	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Storage|Storage Metadata")
	FString GetPath();

	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Storage|Storage Metadata")
	FString GetName();

	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Storage|Storage Metadata")
	FString GetBucket();

	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Storage|Storage Metadata")
	FString GetGeneration();

	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Storage|Storage Metadata")
	FString GetMetaGeneration();

	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Storage|Storage Metadata")
	int64 GetCreationTimeMillis();

	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Storage|Storage Metadata")
	int64 GetUpdatedTimeMillis();

	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Storage|Storage Metadata")
	int64 GetSizeBytes();

	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Storage|Storage Metadata")
	FString GetMd5Hash();

	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Storage|Storage Metadata")
	FString GetCacheControl();

	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Storage|Storage Metadata")
	FString GetContentDisposition();

	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Storage|Storage Metadata")
	FString GetContentEncoding();

	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Storage|Storage Metadata")
	FString GetContentLanguage();

	//TODO StorageReference GetReference

	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Storage|Storage Metadata")
	bool IsMetadataValid();

#if PLATFORM_ANDROID
	void Init(jobject javaMetadata);
	jobject JavaMetadataObject;
#endif

#if PLATFORM_IOS
	void Init(FIRStorageMetadata* metadata);
	FIRStorageMetadata* iosMetadata;
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	void Init(firebase::storage::Metadata metadata);
	firebase::storage::Metadata desktopMetadata;
#endif
};
