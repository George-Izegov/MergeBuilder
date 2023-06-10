// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MBCoreTypes.h"
#include "Kismet/BlueprintPlatformLibrary.h"
#include "Utilities/ShopSubsystem.h"
#include "Tutorial/MBTutorialSubsystem.h"
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

	UFUNCTION(BlueprintPure)
	static UMBTutorialSubsystem* GetTutorialSubsystem();

	UFUNCTION(BlueprintPure)
	static UShopSubsystem* GetShopSubsystem();

protected:

	UFUNCTION()
	void SaveAllData();

	UFUNCTION()
	void CheckAllDataLoaded();

	virtual void Shutdown() override;

public:

	UPROPERTY(BlueprintAssignable)
	FNoParamsSignatureDyn OnGameLoaded;
	
	UPROPERTY(BlueprintReadOnly)
	UShopSubsystem* ShopSubsystem;

	UPROPERTY(BlueprintReadOnly)
	UMBTutorialSubsystem* TutorialSubsystem;
};
