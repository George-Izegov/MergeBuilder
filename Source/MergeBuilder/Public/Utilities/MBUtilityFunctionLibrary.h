// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MergeSubsystem.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MBUtilityFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class MERGEBUILDER_API UMBUtilityFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	static bool ReadFromStorage(const FString& StorageName, FString& OutData);
	UFUNCTION(BlueprintCallable)
	static void SaveToStorage(const FString& StorageName, const FString& Data);

	static bool StringToJsonObject(const FString& JsonString, TSharedPtr<FJsonObject>& OutObject);
	static void JsonObjectToString(TSharedPtr<FJsonObject> JsonObject, FString& OutString);

	UFUNCTION(BlueprintCallable)
		static const FString EnumToString(const FString& Enum, int32 EnumValue);
	UFUNCTION(BlueprintCallable)
		static const int32 StringToEnum(const FString& Enum, const FString& EnumString);

	UFUNCTION(BlueprintPure)
	static void GetMergeItemData(const FMergeFieldItem& Item, FMergeItemData& OutData);

	UFUNCTION(BlueprintPure)
	static bool IsMergeItemMaxLevel(const FMergeFieldItem& Item);

	UFUNCTION(BlueprintCallable)
	static int32 GetSkipTimerPrice(const FTimespan& TotalTime, const FTimespan& RemainTime, int32 TotalPrice);

	UFUNCTION(BlueprintCallable)
	static FString GetDeviceID();
};

template <typename T>
void Shuffle(TArray<T> &arr)
{
	for (int i = arr.Num() - 1; i >= 0; --i)
	{
		int j = FMath::Rand() % (i + 1);
		if (i != j) arr.Swap(i, j);
	}
}