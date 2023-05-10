// Fill out your copyright notice in the Description page of Project Settings.


#include "Ads/AdSubsystem.h"
#include "MBUtilityFunctionLibrary.h"

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

