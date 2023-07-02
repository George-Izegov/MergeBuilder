// Copyright (c) 2021 Nineva Studios

#pragma once

#include "Engine/EngineTypes.h"
#include "FGAndroidDynamicLinkEntry.generated.h"

USTRUCT(BlueprintType)
struct FIREBASEGOODIES_API FFGAndroidDynamicLinkEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Firebase Goodies|Dynamic Links")
	bool AutoVerifyDomain = false;

	UPROPERTY(EditAnywhere, Category = "Firebase Goodies|Dynamic Links")
	FString Scheme = "";

	UPROPERTY(EditAnywhere, Category = "Firebase Goodies|Dynamic Links")
	FString Host = "";

	UPROPERTY(EditAnywhere, Category = "Firebase Goodies|Dynamic Links")
	FString PathPrefix = "";
};