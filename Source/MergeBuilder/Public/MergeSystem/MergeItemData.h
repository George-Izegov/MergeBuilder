// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "PaperSprite.h"
#include "MergeItemData.generated.h"

UENUM(BlueprintType)
enum class EMergeItemType : uint8
{
	None,
	Lumber,
	Stones,
	Metal,
	PreciousMetal,
	Food,
	Drinks,
	Clothes,
	Tools,
	ToolsSpawnBox,
	Experience,
	Energy,
	SoftCoins,
	PremCoins,
	EnergyBox,
	SoftCoinBox,
	PremCoinBox,
	StonesBox,
	MetalBox,
	LumberBox,
	FoodBox,
	DrinksBox,
	ClothesBox
};

UENUM(BlueprintType)
enum class EItemInteractType : uint8
{
	None,
	SpawnItem,
	AddValue
};

UENUM(BlueprintType)
enum class EConsumableParamType : uint8
{
	None,
	SoftCoin,
	PremCoin,
	Energy,
	Experience
};

UENUM(BlueprintType)
enum class ESpawnProbability : uint8
{
	Rarely,
	Often
};

USTRUCT(BlueprintType)
struct FMergeFieldItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		EMergeItemType Type = EMergeItemType::None;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int32 Level = 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int32 RemainItemsToSpawn = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool IsDusty = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool IsInBox = false;

	bool operator==(const FMergeFieldItem& Other) const
	{
		return Type == Other.Type && Level == Other.Level;
	}

	bool operator!=(const FMergeFieldItem& Other) const
	{
		return Type != Other.Type || Level != Other.Level;
	}
};

USTRUCT(BlueprintType)
struct FRequiredItem
{
	GENERATED_BODY()

		UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FMergeFieldItem Item;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int32 RequiredNum;
};

USTRUCT(BlueprintType)
struct FSpawnItemData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FMergeFieldItem Item;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ESpawnProbability Probability;

};

USTRUCT(BlueprintType)
struct FMergeItemData
{
	GENERATED_BODY()

		UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TSoftObjectPtr<UTexture2D> Texture;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TSoftObjectPtr<UPaperSprite> Sprite;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FText LocalizedName;
		
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int32 SellPrice;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool Interactable;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Interaction")
		EItemInteractType InteractType;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interaction")
		TArray<FSpawnItemData> SpawnableItems;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interaction")
	int32 MaxItemsToSpawn;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interaction")
		EConsumableParamType AddValueType;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interaction")
		int32 AddValueCount;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interaction")
		int32 EnergyConsume;
};


USTRUCT(BlueprintType)
struct FMergeItemChainRow : public FTableRowBase
{
	GENERATED_BODY()

		UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<FMergeItemData> ItemsChain;
};

USTRUCT(BlueprintType)
struct FMergeItemsField : public FTableRowBase
{
	GENERATED_BODY()

		UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<FMergeFieldItem> ItemsRow;
};
