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
	
	void Init(const FString& UserID, bool bAgreeGDPR = true);
	
	UFUNCTION(BlueprintCallable)
	void ShowRewardedVideoSkipTimer(const int32 SlotId);

	UFUNCTION(BlueprintCallable)
	void ShowRewardedVideoPurchaseItem(const FString& ProductId);

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UIronSourceAdProxyClient> NetworkClass = UIronSourceAdProxyClient::StaticClass();

	UPROPERTY(BlueprintReadOnly)
	UAdProxy* AdProxy;
};
