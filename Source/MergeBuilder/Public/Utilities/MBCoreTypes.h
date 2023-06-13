// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MBCoreTypes.generated.h"

DECLARE_MULTICAST_DELEGATE(FNoParamsSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNoParamsSignatureDyn);

UENUM(BlueprintType)
enum class EUIColor : uint8
{
	Orange,
	Green,
	Grey
};

UCLASS()
class MERGEBUILDER_API UMBCoreTypes : public UObject
{
	GENERATED_BODY()
};

