// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MBCoreTypes.h"
#include "TimeSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class MERGEBUILDER_API UTimeSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	UTimeSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable)
	const FDateTime& GetUTCNow();

protected:

	UFUNCTION()
	void RequestTime();

	void RecalculateTime();

public:

	FNoParamsSignature OnTimeSuccessRequested;

protected:

	bool TimeValid = false;

	UPROPERTY()
	FDateTime TimeUTC;

	FTimerHandle RecalculationTimer;

	TMap<FString, FString> TimeAPI_URLs;
	
	TMap<FString, FString> RemainURLs;

public:

	bool IsTimeValid() { return TimeValid; };
};
