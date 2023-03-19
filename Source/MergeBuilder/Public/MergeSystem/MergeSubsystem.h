// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DataTable.h"
#include "PaperSprite.h"
#include "MergeSubsystem.generated.h"

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
	PremCoins
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
		FText Description;

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

const FIntPoint MergeFieldSize = FIntPoint(7, 9);

/**
 * 
 */
UCLASS()
class MERGEBUILDER_API UMergeSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	friend class AMBMergeFieldManager;
	
public:

	UMergeSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	virtual void Deinitialize() override;

	bool GetItemAt(const FIntPoint& Index, FMergeFieldItem& OutItem);

	bool GetClosestFreeIndex(const FIntPoint& Index, FIntPoint& ClosestFreeIndex);

	static void GetRandomItemWeight(const TArray<FSpawnItemData>& Items, FSpawnItemData& OutItem);

	static int32 GetWeightForProbability(ESpawnProbability Probability);

	bool HasFreePlace();

	int32 DecrementRemainItemsToSpawn(const FIntPoint& Index);

	void InitItem(FMergeFieldItem& OutItem);

	bool GetFirstReward(FMergeFieldItem& OutItem);

	void RemoveFirstReward();

	void AddNewReward(const FMergeFieldItem& NewRewardItem);

	UFUNCTION(BlueprintCallable)
	int32 GetItemTotalCount(const FMergeFieldItem& Item);

	void SpendItems(const FMergeFieldItem& Item, int32 Count);

protected:

	void ParseField(const FString& JsonString);

	void SaveField();

	void SetItemAt(const FIntPoint& Index, const FMergeFieldItem& Item);

	bool TryMergeItems(const FMergeFieldItem& Item, const FIntPoint& MergeIndex, FMergeFieldItem& MergedItem);

	void GetAllIndexVariants(int32 IndexSum, TArray<FIntPoint>& Variants);

	TArray<TArray<FMergeFieldItem>> MergeField;

	TArray<FMergeFieldItem> RewardsQueue;

	UPROPERTY()
	UDataTable* MergeItemsDataTable;
};
