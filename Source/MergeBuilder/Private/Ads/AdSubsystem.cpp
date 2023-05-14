// Fill out your copyright notice in the Description page of Project Settings.


#include "Ads/AdSubsystem.h"

#include "MBGameInstance.h"
#include "MBUtilityFunctionLibrary.h"
#include "ShopSubsystem.h"
#include "CitySystem/CityBuilderSubsystem.h"
#include "QuestSystem/MBQuestSubsystem.h"

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
	case EAdPlacementType::SkipTimerGenerator:
		{
			auto CityBuilderSubsystem = GetGameInstance()->GetSubsystem<UCityBuilderSubsystem>();
			int32 ObjectID = FCString::Atoi(*Param);
			CityBuilderSubsystem->HandleSuccessWatchVideoForObject(ObjectID);
		}
		break;
	case EAdPlacementType::SkipTimerQuests:
		{
			auto QuestSubsystem = GetGameInstance()->GetSubsystem<UMBQuestSubsystem>();
			QuestSubsystem->HandleSuccessWatchVideoForQuests();
		}
		break;
	case EAdPlacementType::PurchaseProduct:
		{
			auto ShopSubsystem = Cast<UMBGameInstance>(GetGameInstance())->ShopSubsystem;
			ShopSubsystem->HandleSuccessAdWatchForProduct(Param);
		}
		break;
	}

	OnRewardedAdSuccessHandled.Broadcast();
}

void UAdSubsystem::HandleAdClosed()
{
	OnRewardedAdClosed.Broadcast();
}

void UAdSubsystem::ShowRewardedVideoSkipTimerGenerator(int32 CityObjectId)
{
	FString StringId = FString::FromInt(CityObjectId);
	AdProxy->ShowRewardedVideo(StringId, EAdPlacementType::SkipTimerGenerator);
}

void UAdSubsystem::ShowRewardedVideoSkipTimerQuests()
{
	AdProxy->ShowRewardedVideo("", EAdPlacementType::SkipTimerQuests);
}

void UAdSubsystem::ShowRewardedVideoPurchaseItem(const FString& ProductId)
{
	AdProxy->ShowRewardedVideo(ProductId, EAdPlacementType::PurchaseProduct);
}

