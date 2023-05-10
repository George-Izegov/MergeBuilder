// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MBCoreTypes.h"
#include "UObject/NoExportTypes.h"
#include "AdProxy.generated.h"

UENUM(BlueprintType)
enum class EAdPlacementType : uint8
{
	PurchaseProduct,
	SkipTimer
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetAdResult, EAdPlacementType, AdPlacementType, const FString&, Param);
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
	FOnGetAdResult OnRewardedAdSuccessWatched;

	UPROPERTY(BlueprintAssignable)
	FNoParamsSignatureDyn OnRewardedAdClosed;

protected:

	EAdPlacementType CurrentAdPlacement;
	FString CurrentParam;
	
	FString NetworkName;

	bool AdStarted = false;
};
