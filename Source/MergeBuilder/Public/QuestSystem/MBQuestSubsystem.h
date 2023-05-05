// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MergeSubsystem.h"
#include "MBQuestSubsystem.generated.h"

UENUM(BlueprintType)
enum class EQuestType : uint8
{
	MergeItems,
	CityObjects
};

USTRUCT(BlueprintType)
struct FQuestData
{
	GENERATED_BODY()

	UPROPERTY()
	uint32 QuestID = -1;

	UPROPERTY(BlueprintReadWrite)
	EQuestType QuestType;

	UPROPERTY(BlueprintReadWrite)
	TArray<FRequiredItem> RequiredItems;

	UPROPERTY(BlueprintReadWrite)
	FName RequiredObjectName = NAME_None;

	UPROPERTY(BlueprintReadWrite)
	int32 RequiredObjectAmount = 1;

	UPROPERTY(BlueprintReadWrite)
	int32 RewardExperience = 0;

	UPROPERTY(BlueprintReadWrite)
	TArray<FMergeFieldItem> RewardItems;
};
/**
 * 
 */
UCLASS()
class MERGEBUILDER_API UMBQuestSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	
	UMBQuestSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void SaveQuests();
	
protected:

	void InitQuests();

	void ParseQuests(const FString& JsonString);

	void GenerateNewQuests();

	bool GetRecommendedQuest(FQuestData& RecommendedQuest);

	EQuestType GenerateTypeForQuest();

	int32 GenerateQuestCount();

	void GenerateRequiredMergeItemsForQuest(TArray<FRequiredItem>& RequiredItems);
	void GenerateRequiredCityObjectForQuest(FName& RequiredObjectName, int32& RequiredObjectAmount);

	void GenerateRewardForMergeItems(const TArray<FRequiredItem>& RequiredItems, TArray<FMergeFieldItem>& RewardItems, int32& RewardExperience);
	
	void GenerateRewardForCityObject(const FName& RequiredObjectName, int32 ObjectAmount, TArray<FMergeFieldItem>& RewardItems, int32& RewardExperience);

	void GenerateRewardForHardness(int32 QuestHardness, TArray<FMergeFieldItem>& RewardItems, int32& RewardExperience);
	
	void GenerateMergeItemForQuest(EMergeItemType ItemType, FRequiredItem& OutItem);

	int32 CalculateHardnessOfRequiredObjects(const TArray<FRequiredItem>& RequiredItems);
	
private:

	FDelegateHandle OnGetTimeDelegateHandle;

	FTimerHandle QuestRefreshTimerHandle;

protected:

	bool IsInitialized = false;

	TArray<FQuestData> Quests;

	int32 RefreshHours = 4;
	
	UPROPERTY(BlueprintReadOnly)
	FDateTime DateTo;
};
