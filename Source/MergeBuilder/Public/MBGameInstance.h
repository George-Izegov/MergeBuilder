// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintPlatformLibrary.h"
#include "MBGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class MERGEBUILDER_API UMBGameInstance : public UPlatformGameInstance
{
	GENERATED_BODY()
	
public:

	virtual void Init() override;

protected:

	UFUNCTION()
	void SaveAllData();
};
