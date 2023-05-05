// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MBCoreTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MergeItemData.h"
#include "MergeSubsystem.generated.h"

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

	void SaveField();

	bool GetAllItemsInBoxAround(const FIntPoint& Index, TArray<FIntPoint>& OutItemIndexes);

	void OpenInBoxItem(const FIntPoint& Index, FMergeFieldItem& OutItem);

protected:

	void InitFieldFromStartTable();

	void ParseField(const FString& JsonString);

	void SetItemAt(const FIntPoint& Index, const FMergeFieldItem& Item);

	bool TryMergeItems(const FMergeFieldItem& Item, const FIntPoint& MergeIndex, FMergeFieldItem& MergedItem);

	void GetAllIndexVariants(int32 IndexSum, TArray<FIntPoint>& Variants);

	TArray<TArray<FMergeFieldItem>> MergeField;

	TArray<FMergeFieldItem> RewardsQueue;
	
public:
	UPROPERTY()
	UDataTable* MergeItemsDataTable = nullptr;

	UPROPERTY()
	UDataTable* StartFieldDataTable = nullptr;

	UPROPERTY(BlueprintAssignable)
	FNoParamsSignatureDyn OnGetReward;
};
