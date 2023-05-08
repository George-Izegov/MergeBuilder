// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MBCoreTypes.h"
#include "Kismet/BlueprintPlatformLibrary.h"
#include "Utilities/ShopSubsystem.h"
#include "MBGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class MERGEBUILDER_API UMBGameInstance : public UPlatformGameInstance
{
	GENERATED_BODY()
	
public:

	UMBGameInstance();
	
	virtual void Init() override;

protected:

	UFUNCTION()
	void SaveAllData();

	UFUNCTION()
	void CheckAllDataLoaded();

public:

	UPROPERTY(BlueprintAssignable)
	FNoParamsSignatureDyn OnGameLoaded;

protected:
	
	UPROPERTY(BlueprintReadOnly)
	UShopSubsystem* ShopSubsystem;
};
