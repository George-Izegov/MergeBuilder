// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "MergeItemData.h"
#include "InAppPurchaseQueryCallbackProxy2.h"
#include "ShopSubsystem.generated.h"

UENUM(BlueprintType)
enum class EShopCategory : uint8
{
	PremCoins,
	SoftCoins,
	Energy
};

USTRUCT(BlueprintType)
struct FProduct : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FString ProductID;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	EShopCategory Category;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool IsStoreProduct = false;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool IsFreeForAd = false;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	EConsumableParamType PriceType = EConsumableParamType::PremCoin;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 Price = 0;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	EConsumableParamType RewardType = EConsumableParamType::None;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 RewardAmount = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPurchaseResult, const FString&, ProductID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStoreProductsReceived);
/**
 * 
 */
UCLASS(Blueprintable)
class MERGEBUILDER_API UShopSubsystem : public UObject
{
	GENERATED_BODY()

public:

	UShopSubsystem();

	void Init();

	UFUNCTION(BlueprintCallable)
	void GetStorePriceText(const FString& ProductID, FText& PriceText);

	bool GetGooglePlayOfferInfo(const FString& ProductID, FOnlineProxyStoreOffer& OfferInfo);
protected:

	UFUNCTION(BlueprintImplementableEvent)
	void RequestStoreProductsInfo();

	UFUNCTION(BlueprintCallable)
	bool MakeInternalPurchase(const FString& ProductID);

	UFUNCTION(BlueprintCallable)
	void MakeExternalStorePurchase(const FString& ProductID);

	UFUNCTION(BlueprintImplementableEvent)
	void MakeGooglePlayPurchase(const FString& ProductID);

	UFUNCTION(BlueprintImplementableEvent)
	void MakeAppStorePurchase(const FString& ProductID);

	UFUNCTION(BlueprintCallable)
	void GiveRewardOfProduct(const FString& ProductID);

public:

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnPurchaseResult OnPurchaseSuccess;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnPurchaseResult OnPurchaseFailed;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnStoreProductsReceived OnStoreProductsReceivedSuccess;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnStoreProductsReceived OnStoreProductsReceivedFailed;

protected:

	UPROPERTY(BlueprintReadWrite)
	FString ProcessedStoreProductID;

	UPROPERTY(BlueprintReadOnly)
	UDataTable* ProductsDataTable = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TArray<FOnlineProxyStoreOffer> GooglePlayOffers;
};
