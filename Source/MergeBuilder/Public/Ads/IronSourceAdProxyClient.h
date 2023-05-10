// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IPsIronSource.h"
#include "PsIronSourceProxy.h"
#include "Ads/AdProxy.h"
#include "IronSourceAdProxyClient.generated.h"

/**
 *
 */
UCLASS()
class MERGEBUILDER_API UIronSourceAdProxyClient : public UAdProxy
{
	GENERATED_BODY()

public:
	UIronSourceAdProxyClient();

	virtual void Init(const FString& UserId, bool bAgreeGDPR = true) override;

	virtual void ShowRewardedVideoSkipTimer(const FString UniqueId) override;

	virtual void ShowRewardedVideoPurchaseItem(const FString& ProductId) override;

	virtual void ShowRewardedVideoCallback(int32 EventType) override;

private:

	UPROPERTY()
	UPsIronSourceProxy* IronSourceSDKProxy;
};
