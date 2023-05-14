// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AdProxy.h"
#include "IronSourceAdProxyClient.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AdSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class MERGEBUILDER_API UAdSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;
	
	UFUNCTION(BlueprintCallable)
	void ShowRewardedVideoSkipTimerGenerator(int32 CityObjectId);

	UFUNCTION(BlueprintCallable)
	void ShowRewardedVideoSkipTimerQuests();

	UFUNCTION(BlueprintCallable)
	void ShowRewardedVideoPurchaseItem(const FString& ProductId);

protected:

	void Init(const FString& UserID, bool bAgreeGDPR = true);

	UFUNCTION()
	void HandleSuccessAdWatched(EAdPlacementType PlacementType, const FString& Param);
	UFUNCTION()
	void HandleAdClosed();

public:

	UPROPERTY(BlueprintAssignable)
	FNoParamsSignatureDyn OnRewardedAdSuccessHandled;

	UPROPERTY(BlueprintAssignable)
	FNoParamsSignatureDyn OnRewardedAdClosed;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UIronSourceAdProxyClient> NetworkClass = UIronSourceAdProxyClient::StaticClass();

	UPROPERTY(BlueprintReadOnly)
	UAdProxy* AdProxy;
};
