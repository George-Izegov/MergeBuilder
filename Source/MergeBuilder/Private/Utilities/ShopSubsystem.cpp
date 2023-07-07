// Fill out your copyright notice in the Description page of Project Settings.


#include "Utilities/ShopSubsystem.h"

#include "JsonObjectConverter.h"
#include "MBUtilityFunctionLibrary.h"
#include "TimeSubsystem.h"
#include "Analytics/FGAnalytics.h"
#include "Analytics/FGAnalyticsParameter.h"
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

	ParseHistory();
}

void UShopSubsystem::GetStorePriceText(const FString& ProductID, FText& PriceText)
{
	PriceText = FText::FromString("---");

#if PLATFORM_IOS

#endif

#if PLATFORM_ANDROID
	FSkuDetailsRecord GooglePlayOffer;
	if (!GetGooglePlayOfferInfo(ProductID, GooglePlayOffer))
		return;

	PriceText = FText::FromString(GooglePlayOffer.Price);
#endif
	
}

bool UShopSubsystem::GetGooglePlayOfferInfo(const FString& ProductID, FSkuDetailsRecord& OfferInfo)
{
	for (const FSkuDetailsRecord& Offer : GooglePlayOffers)
	{
		if (Offer.ProductID == ProductID)
		{
			OfferInfo = Offer;
			return true;
		}
	}

	return false;
}

void UShopSubsystem::ParseHistory()
{
	FString SavedData;
	if (!UMBUtilityFunctionLibrary::ReadFromStorage("ShopHistory", SavedData))
		return;

	TSharedPtr<FJsonObject> JsonObject;

	if (!UMBUtilityFunctionLibrary::StringToJsonObject(SavedData, JsonObject))
		return;
	
	const TArray<TSharedPtr<FJsonValue>>* ValuesArray;
	if (JsonObject->TryGetArrayField("Products", ValuesArray))
	{
		for (const auto& ProductValue : *ValuesArray)
		{
			FPurchaseHistory PurchaseHistory;
			if (!FJsonObjectConverter::JsonObjectToUStruct<FPurchaseHistory>(ProductValue->AsObject().ToSharedRef(), &PurchaseHistory))
				continue;
			
			ProductsHistory.Add(PurchaseHistory);
		}
	}
}

void UShopSubsystem::SaveHistory()
{
	TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();

	TArray<TSharedPtr<FJsonValue>> ValuesArray;

	for (const auto& Product : ProductsHistory)
	{
		TSharedPtr<FJsonObject> ProductObject = FJsonObjectConverter::UStructToJsonObject<FPurchaseHistory>(Product);
		TSharedPtr<FJsonValueObject> ProductValue = MakeShared<FJsonValueObject>(ProductObject);

		ValuesArray.Add(ProductValue);
	}

	JsonObject->SetArrayField("Products", ValuesArray);
	

	FString StringData;
	UMBUtilityFunctionLibrary::JsonObjectToString(JsonObject, StringData);

	UMBUtilityFunctionLibrary::SaveToStorage("ShopHistory", StringData);
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

	DecrementPurchaseLimit(ProductID);

	UFGAnalyticsParameter* Param = NewObject<UFGAnalyticsParameter>();
	Param->SetName("product_id");
	Param->SetString(ProductID);
	UFGAnalytics::LogEventWithParameter("internal_purchase", Param);
	
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

APlayerController* UShopSubsystem::GetPlayerController()
{
	return UGameplayStatics::GetPlayerController(GetWorld(), 0);
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

bool UShopSubsystem::IsProductAvailable(const FString& ProductID)
{
	FPurchaseHistory ProductHistory;
	if (!GetProductHistory(ProductID, ProductHistory))
		return true;

	auto TimeSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UTimeSubsystem>();

	if (ProductHistory.LastPurchaseDate.GetDate() != TimeSubsystem->GetUTCNow().GetDate())
		return true;

	return ProductHistory.PurchaseLimit != 0;
}

bool UShopSubsystem::GetProductHistory(const FString& ProductID, FPurchaseHistory& History)
{
	for (const auto& Product : ProductsHistory)
	{
		if (Product.ProductID == ProductID)
		{
			History = Product;
			return true;
		}
	}

	return false;
}

void UShopSubsystem::DecrementPurchaseLimit(const FString& ProductID)
{
	auto Row = ProductsDataTable->FindRow<FProduct>(FName(ProductID),"");
	
	if (!Row)
		return;

	if (Row->PurchaseLimit <= 0)
		return;
	
	FPurchaseHistory ProductHistory;
	ProductHistory.ProductID = ProductID;
	int32 Index = ProductsHistory.Find(ProductHistory);

	auto TimeSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UTimeSubsystem>();
	
	if (Index != INDEX_NONE)
	{
		if (ProductsHistory[Index].LastPurchaseDate.GetDate() != TimeSubsystem->GetUTCNow().GetDate())
		{
			ProductsHistory[Index].PurchaseLimit = Row->PurchaseLimit;
		}
		
		ProductsHistory[Index].PurchaseLimit--;
		ProductsHistory[Index].PurchaseLimit = FMath::Clamp(ProductsHistory[Index].PurchaseLimit, 0, Row->PurchaseLimit);
		
		ProductsHistory[Index].LastPurchaseDate = TimeSubsystem->GetUTCNow();
	}
	else
	{
		ProductHistory.PurchaseLimit = Row->PurchaseLimit - 1;
		ProductHistory.LastPurchaseDate = TimeSubsystem->GetUTCNow();

		ProductsHistory.Add(ProductHistory);
	}

	SaveHistory();
}

void UShopSubsystem::HandleSuccessAdWatchForProduct(const FString& ProductID)
{
	GiveRewardOfProduct(ProductID);

	DecrementPurchaseLimit(ProductID);
}

