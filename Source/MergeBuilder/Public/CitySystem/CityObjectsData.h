// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "MergeItemData.h"
#include "CityobjectsData.generated.h"

USTRUCT(BlueprintType)
struct FGeneratorSettings
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FMergeFieldItem GeneratedBox;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		int32 MinutesToRestore;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		int32 RequiredEmployees;
};

USTRUCT(BlueprintType)
struct FCityObject
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FName ObjectName = NAME_None;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FVector Location = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		float Rotation = 0.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		float Scale = 1.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FDateTime RestoreTime;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FString QuestID;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		int32 ObjectID = -1;
};

UENUM(BlueprintType)
enum class ECityObjectCategory : uint8
{
	Other,
	Buildings,
	Plants,
	Infrastructure
};

// City Rating structure
USTRUCT(BlueprintType)
struct FCityRatings
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 Greening = 0;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 Infrastructure = 0;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 Comfort = 0;

	const FCityRatings& operator+=(const FCityRatings& Other)
	{
		Greening += Other.Greening;
		Infrastructure += Other.Infrastructure;
		Comfort += Other.Comfort;
		return *this;
	}
};


USTRUCT(BlueprintType)
struct FCityObjectData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		TSoftClassPtr<class AMBBaseCityObjectActor> ObjectClass;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FName NextLevelObjectName;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool IsInShop = true;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool FitForQuest = true;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		ECityObjectCategory Category;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FText LocalizedName;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<FRequiredItem> RequiredItems;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 CostInCoins;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	EConsumableParamType CoinsType = EConsumableParamType::SoftCoin;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		bool IsGenerator = false;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FGeneratorSettings GeneratorSettings;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FCityRatings AdditionalRatings;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 AdditionalPopulation = 0;
};
