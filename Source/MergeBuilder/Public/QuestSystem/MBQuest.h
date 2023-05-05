// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MergeItemData.h"
#include "MBQuest.generated.h"

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
	FString QuestID;

	UPROPERTY(BlueprintReadWrite)
	EQuestType QuestType;

	UPROPERTY(BlueprintReadWrite)
	TArray<FRequiredItem> RequiredItems;

	UPROPERTY(BlueprintReadWrite)
	FName RequiredObjectName = NAME_None;

	UPROPERTY(BlueprintReadWrite)
	int32 RequiredObjectAmount = 0;

	UPROPERTY(BlueprintReadWrite)
	int32 RequiredObjectProgress = 0;

	UPROPERTY(BlueprintReadWrite)
	int32 RewardExperience = 0;

	UPROPERTY(BlueprintReadWrite)
	TArray<FMergeFieldItem> RewardItems;

	bool operator==(const FQuestData& Other) const
	{
		return QuestID.Equals(Other.QuestID);
	}
};
