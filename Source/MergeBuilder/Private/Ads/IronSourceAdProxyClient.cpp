// Fill out your copyright notice in the Description page of Project Settings.


#include "Ads/IronSourceAdProxyClient.h"
#include "PsIronSourceLibrary.h"

UIronSourceAdProxyClient::UIronSourceAdProxyClient()
{
	NetworkName = "IronSource";

	IronSourceSDKProxy = UPsIronSourceLibrary::GetIronSourceProxy();
}

void UIronSourceAdProxyClient::Init(const FString& UserId, bool bAgreeGDPR)
{
	Super::Init(UserId);
	
	IronSourceSDKProxy->SetGDPRConsent(true);

	if (IronSourceSDKProxy->IsInitialized())
	{
		IronSourceSDKProxy->ForceUpdateIronSourceUser(UserId);
	}
	else
	{
		IronSourceSDKProxy->InitIronSource(UserId);
	}

	if (IronSourceSDKProxy)
	{
		IronSourceSDKProxy->VideoStateDelegate.AddUniqueDynamic(this, &UIronSourceAdProxyClient::ShowRewardedVideoCallback);
	}
}

void UIronSourceAdProxyClient::ShowRewardedVideoSkipTimer(const FString UniqueId)
{
	if (IsAdStarted())
		return;

	Super::ShowRewardedVideoSkipTimer(UniqueId);
	IronSourceSDKProxy->ShowRewardedVideo("decreaseTimer", "unique_id", UniqueId);
}

void UIronSourceAdProxyClient::ShowRewardedVideoPurchaseItem(const FString& ProductId)
{
	if (IsAdStarted())
		return;

	Super::ShowRewardedVideoPurchaseItem(ProductId);
	IronSourceSDKProxy->ShowRewardedVideo("shopItem", "product_id", ProductId);
}

void UIronSourceAdProxyClient::ShowRewardedVideoCallback(int32 EventType)
{
	Super::ShowRewardedVideoCallback(EventType);
	
	FString ET = FString::FromInt(EventType);
	UE_LOG(LogTemp, Display, TEXT(" UIronSourceAdProxyClient ShowRewardedVideoCallback: EventType %s"), *ET);
	
	if (EventType == 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, "IronSource Success watched");
		OnRewardedAdSuccessWatched.Broadcast();
	}
}