// Fill out your copyright notice in the Description page of Project Settings.


#include "Ads/IronSourceAdProxyClient.h"

#include "MBUtilityFunctionLibrary.h"
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

void UIronSourceAdProxyClient::ShowRewardedVideo(const FString& RewardedVideoParam, EAdPlacementType PlacementType)
{
	if (IsAdStarted())
		return;

	Super::ShowRewardedVideo(RewardedVideoParam, PlacementType);

	FString PlacementName = UMBUtilityFunctionLibrary::EnumToString("EAdPlacementType", (int32) PlacementType);
	
	IronSourceSDKProxy->ShowRewardedVideo(PlacementName, "param_1", RewardedVideoParam);

#if WITH_EDITOR
	// TEST EDITOR ONLY LOGIC
	
	FTimerDelegate EditorSuccessWatchDelegate = FTimerDelegate::CreateLambda([this]()
	{
		ShowRewardedVideoCallback(0);
	});
	FTimerHandle DelegateHandle;
	GetWorld()->GetTimerManager().SetTimer(DelegateHandle, EditorSuccessWatchDelegate, 3.0f, false);
#endif
}

void UIronSourceAdProxyClient::ShowRewardedVideoCallback(int32 EventType)
{
	Super::ShowRewardedVideoCallback(EventType);
	
	FString ET = FString::FromInt(EventType);
	UE_LOG(LogTemp, Display, TEXT(" UIronSourceAdProxyClient ShowRewardedVideoCallback: EventType %s"), *ET);
	
	if (EventType == 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, "IronSource Success watched");
	}
}