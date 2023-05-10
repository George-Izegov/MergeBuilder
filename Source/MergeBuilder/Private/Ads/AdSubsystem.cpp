// Fill out your copyright notice in the Description page of Project Settings.


#include "Ads/AdSubsystem.h"

#include "MBGameInstance.h"
#include "MBUtilityFunctionLibrary.h"
#include "ShopSubsystem.h"

void UAdSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	FString DeviceID = UMBUtilityFunctionLibrary::GetDeviceID();

	Init(DeviceID);
}

void UAdSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UAdSubsystem::Init(const FString& UserID, bool bAgreeGDPR)
{
	AdProxy = NewObject<UAdProxy>(this, NetworkClass);

	AdProxy->Init(UserID, bAgreeGDPR);

	AdProxy->OnRewardedAdSuccessWatched.AddDynamic(this, &UAdSubsystem::HandleSuccessAdWatched);
	AdProxy->OnRewardedAdClosed.AddDynamic(this, &UAdSubsystem::HandleAdClosed);
}

void UAdSubsystem::HandleSuccessAdWatched(EAdPlacementType PlacementType, const FString& Param)
{
	switch (PlacementType)
	{
	case EAdPlacementType::SkipTimer:
		
		break;
	case EAdPlacementType::PurchaseProduct:
		auto ShopSubsystem = Cast<UMBGameInstance>(GetGameInstance())->ShopSubsystem;
		ShopSubsystem->HandleSuccessAdWatchForProduct(Param);
		break;
	}

	OnRewardedAdSuccessHandled.Broadcast();
}

void UAdSubsystem::HandleAdClosed()
{
	OnRewardedAdClosed.Broadcast();
}

void UAdSubsystem::ShowRewardedVideoSkipTimer(const int32 SlotId)
{
	FString UniqueId = "";
	AdProxy->ShowRewardedVideoSkipTimer(UniqueId);
}

void UAdSubsystem::ShowRewardedVideoPurchaseItem(const FString& ProductId)
{
	AdProxy->ShowRewardedVideoPurchaseItem(ProductId);
}

