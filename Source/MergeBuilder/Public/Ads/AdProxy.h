// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MBCoreTypes.h"
#include "UObject/NoExportTypes.h"
#include "AdProxy.generated.h"

/**
 * 
 */
UCLASS()
class MERGEBUILDER_API UAdProxy : public UObject
{
	GENERATED_BODY()

public:
	
	virtual void Init(const FString& UserId, bool bAgreeGDPR =  true);
	
	virtual void ShowRewardedVideoSkipTimer(const FString UniqueId);
	
	virtual void ShowRewardedVideoPurchaseItem(const FString& ProductId);

	UFUNCTION()
	virtual void ShowRewardedVideoCallback(int32 EventType);

	UFUNCTION(BlueprintCallable)
	bool IsAdStarted() const { return AdStarted; }

	UPROPERTY(BlueprintAssignable)
	FNoParamsSignatureDyn OnRewardedAdSuccessWatched;

	UPROPERTY(BlueprintAssignable)
	FNoParamsSignatureDyn OnRewardedAdClosed;

protected:

	FString AnalyticAdPlacement;
	FString NetworkName;

	bool AdStarted = false;
};
