// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MBQuest.h"
#include "CitySystem/CityObjectsData.h"
#include "MBQuestSubsystem.generated.h"

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

	UFUNCTION(BlueprintCallable)
	bool GetQuestByID(const FString& QuestID, FQuestData& OutQuest);

	UFUNCTION(BlueprintCallable)
	bool CheckQuestRequirements(const FQuestData& Quest);

	UFUNCTION(BlueprintCallable)
	void CompleteQuest(const FString& QuestID);

	TArray<FString> GetAllQuestIDs();

	UFUNCTION(BlueprintCallable)
	void UpdateQuests();

	UFUNCTION(BlueprintCallable)
	void GenerateNewQuestsForPrem();

	void HandleSuccessWatchVideoForQuests();
	
protected:

	void InitQuests();

	void ParseQuests(const FString& JsonString);
	
	void GenerateNewQuests();

	bool GetRecommendedQuest(FQuestData& RecommendedQuest);

	void MakeQuestID(FQuestData& Quest);

	EQuestType GenerateTypeForQuest();

	int32 GenerateQuestCount();

	void GenerateRequiredMergeItemsForQuest(TArray<FRequiredItem>& RequiredItems);
	void GenerateRequiredCityObjectForQuest(FName& RequiredObjectName, int32& RequiredObjectAmount);

	void GenerateRewardForMergeItems(const TArray<FRequiredItem>& RequiredItems, TArray<FMergeFieldItem>& RewardItems, int32& RewardExperience);
	
	void GenerateRewardForCityObject(const FName& RequiredObjectName, int32 ObjectAmount, TArray<FMergeFieldItem>& RewardItems, int32& RewardExperience);

	void GenerateRewardForHardness(int32 QuestHardness, TArray<FMergeFieldItem>& RewardItems, int32& RewardExperience);
	
	void GenerateMergeItemForQuest(EMergeItemType ItemType, FRequiredItem& OutItem);

	int32 CalculateHardnessOfRequiredObjects(const TArray<FRequiredItem>& RequiredItems);

	UFUNCTION()
	void UpdateCityObjectBuildQuests(FName NewBuildObject);

	void CheckRefreshQuestsTimer();

	void GetPossibleItemTypes(TArray<EMergeItemType>& OutItemTypes);

	void GetQuestObjects(TMap<FName, FCityObjectData*>& OutObjects);

	void GenerateNewQuest(FQuestData& Quest);

	bool HasQuestWithSuchRequirements(const FQuestData& Quest);
	
private:

	FDelegateHandle OnGetTimeDelegateHandle;

	FTimerHandle QuestRefreshTimerHandle;

protected:

	bool IsInitialized = false;

	TArray<FQuestData> Quests;

	UPROPERTY(BlueprintReadOnly)
	int32 RefreshHours = 4;
	
	UPROPERTY(BlueprintReadOnly)
	FDateTime DateTo;

	UPROPERTY(BlueprintReadOnly)
	int32 QuestRefreshPremPrice = 30;

	UPROPERTY(BlueprintReadOnly)
	int32 AdSkipMinutes = 60;

	bool GenerateNewQuestAfterComplete = true;
};
