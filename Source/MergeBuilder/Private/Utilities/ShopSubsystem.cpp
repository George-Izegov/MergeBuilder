// Fill out your copyright notice in the Description page of Project Settings.


#include "Utilities/ShopSubsystem.h"

#include "Kismet/GameplayStatics.h"
#include "User/AccountSubsystem.h"

UShopSubsystem::UShopSubsystem()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> ItemsDataTable(TEXT("DataTable'/Game/Development/DataTables/ShopItems.ShopItems'"));
	if (ItemsDataTable.Succeeded())
	{
		ProductsDataTable = ItemsDataTable.Object;
	}
	check(ProductsDataTable);
}

void UShopSubsystem::Init()
{
	RequestStoreProductsInfo();
}

bool UShopSubsystem::MakeInternalPurchase(const FString& ProductID)
{
	auto Row = ProductsDataTable->FindRow<FProduct>(FName(ProductID),"");

	if (!Row)
	{
		UE_LOG(LogTemp, Error, TEXT("UShopSubsystem::MakeInternalPurchase() - ProductID %s not found"), *ProductID);
		return false;
	}

	auto GI = UGameplayStatics::GetGameInstance(GetWorld());
	auto AccountSubsystem = GI->GetSubsystem<UAccountSubsystem>();

	switch (Row->PriceType)
	{
	case EConsumableParamType::PremCoin:
		if (AccountSubsystem->GetPremCoins() < Row->Price)
		{
			UE_LOG(LogTemp, Warning, TEXT("UShopSubsystem::MakeInternalPurchase() - Not enough PremCoins"));
			return false;
		}
		AccountSubsystem->SpendPremCoins(Row->Price);
		break;
	case EConsumableParamType::SoftCoin:
		if (AccountSubsystem->GetSoftCoins() < Row->Price)
		{
			UE_LOG(LogTemp, Warning, TEXT("UShopSubsystem::MakeInternalPurchase() - Not enough SoftCoins"));
			return false;
		}
		AccountSubsystem->SpendSoftCoins(Row->Price);
		break;
	}

	GiveRewardOfProduct(ProductID);
	
	return true;
}

void UShopSubsystem::GiveRewardOfProduct(const FString& ProductID)
{
	auto Row = ProductsDataTable->FindRow<FProduct>(FName(ProductID),"");

	if (!Row)
	{
		UE_LOG(LogTemp, Error, TEXT("UShopSubsystem::GiveRewardOfProduct() - ProductID %s not found"), *ProductID);
		return;
	}

	auto GI = UGameplayStatics::GetGameInstance(GetWorld());
	auto AccountSubsystem = GI->GetSubsystem<UAccountSubsystem>();

	switch(Row->RewardType)
	{
	case EConsumableParamType::None:
		break;
	case EConsumableParamType::Energy:
		AccountSubsystem->AddEnergy(Row->RewardAmount);
		break;
	case EConsumableParamType::SoftCoin:
		AccountSubsystem->AddSoftCoins(Row->RewardAmount);
		break;
	case EConsumableParamType::PremCoin:
		AccountSubsystem->AddPremCoins(Row->RewardAmount);
		break;
	}
}


void UShopSubsystem::MakeExternalStorePurchase(const FString& ProductID)
{
#if PLATFORM_IOS
	MakeAppStorePurchase(ProductID);
#endif

#if PLATFORM_ANDROID
	MakeGooglePlayPurchase(ProductID);
#endif
}
