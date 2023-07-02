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

#include "FGStorageMetadataValues.generated.h"

UCLASS(BlueprintType)
class FIREBASEGOODIES_API UFGStorageMetadataValues : public UObject
{
	GENERATED_BODY()

public:
	virtual ~UFGStorageMetadataValues();

	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Storage")
	static UFGStorageMetadataValues* NewStorageMetadataValues();

	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Storage")
	void SetCacheControl(const FString& Value);

	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Storage")
	void SetContentDisposition(const FString& Value);

	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Storage")
	void SetContentEncoding(const FString& Value);

	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Storage")
	void SetContentLanguage(const FString& Value);

	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Storage")
	void SetContentType(const FString& Value);

	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Storage")
	void SetCustomMetadata(const TMap<FString, FString>& Values);

#if PLATFORM_ANDROID
	jobject JavaMetadataBuilderObject;
#endif

#if PLATFORM_IOS
	FIRStorageMetadata* iosMetadata;
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	firebase::storage::Metadata desktopMetadata = firebase::storage::Metadata();
#endif

private:
	bool IsMetadataValid() const;
};
