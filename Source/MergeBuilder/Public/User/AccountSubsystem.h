// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DataTable.h"
#include "AccountSubsystem.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class MERGEBUILDER_API UAccountSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	UAccountSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

protected:

	UPROPERTY(BlueprintReadOnly)
	int32 Level = 1;

	UPROPERTY(BlueprintReadOnly)
	int32 Experience = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 MaxExperience = INT32_MAX;

	UPROPERTY(BlueprintReadOnly)
	int32 SoftCoins = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 PremCoins = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 Energy;

	UPROPERTY(BlueprintReadOnly)
	int32 MaxEnergy = 100;

	UPROPERTY(BlueprintReadOnly)
	int32 SecondsToRestoreEnergy = 90;

	UPROPERTY(BlueprintReadOnly)
	bool InfiniteEnergy = false;

	UPROPERTY()
	FTimerHandle EnergyRestoreTimerHandle;

	UPROPERTY()
	FDateTime LastSaveTime;

public:

	inline int32 GetLevel() { return Level; }
	inline int32 GetExperience() { return Experience; }
	inline int32 GetSoftCoins() { return SoftCoins; }
	inline int32 GetPremCoins() { return PremCoins; }
	inline int32 GetEnergy() { return Energy; }

	void AddExperience(int32 DeltaExperience);
	void AddSoftCoins(int32 DeltaCoins);
	void AddPremCoins(int32 DeltaCoins);
	void AddEnergy(int32 DeltaEnergy);

	void SaveAccount();

	bool HasEnoughEnergy(int32 EnergyToSpend);
	bool SpendEnergy(int32 EnergyToSpend);

	void SpendSoftCoins(int32 CoinsToSpend);
	void SpendPremCoins(int32 CoinsToSpend);

	UFUNCTION(BlueprintCallable)
		bool GetRemainTimeToRestoreEnergy(int32& RemainTimeMinutes, int32& RemainTimeSeconds);

protected:

	void ParseAccount(const FString& JsonString);

	UFUNCTION()
	void InitAccount();

	void InitEnergy(int32 OldEnergy, FDateTime OldTime, float OldRemainTime);
	int32 GetMaxExperienceForLevel(int32 InLevel);
	// -1 for NextRestoreTime means default restore time
	void StartRestoreEnergy(float NextRestoreTime = -1.0f);

	void RestoreEnergy();

	void LevelUp();

private:

	FDelegateHandle OnGetTimeDelegateHandle;
};
