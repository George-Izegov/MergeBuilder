// Fill out your copyright notice in the Description page of Project Settings.


#include "User/AccountSubsystem.h"
#include "MBUtilityFunctionLibrary.h"
#include "TimeSubsystem.h"

UAccountSubsystem::UAccountSubsystem()
{
	LevelRewards.Empty();
	FMergeFieldItem Reward;
	Reward.Type = EMergeItemType::SoftCoinBox;
	LevelRewards.Add(Reward);
	Reward.Type = EMergeItemType::PremCoinBox;
	LevelRewards.Add(Reward);
	Reward.Type = EMergeItemType::EnergyBox;
	LevelRewards.Add(Reward);
}

void UAccountSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	FTimerDelegate Delegate = FTimerDelegate::CreateLambda([this]() {
		auto TimeSystem = GetGameInstance()->GetSubsystem<UTimeSubsystem>();
		OnGetTimeDelegateHandle = TimeSystem->OnTimeSuccessRequested.AddUObject(this, &UAccountSubsystem::InitAccount);
	});

	GetWorld()->GetTimerManager().SetTimerForNextTick(Delegate);
}

void UAccountSubsystem::Deinitialize()
{
	SaveAccount();
}

void UAccountSubsystem::SaveAccount()
{
	if (!IsInitialized)
		return;
	
	TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();

	JsonObject->SetNumberField("level", Level);
	JsonObject->SetNumberField("exp", Experience);
	JsonObject->SetNumberField("softCoins", SoftCoins);
	JsonObject->SetNumberField("premCoins", PremCoins);
	JsonObject->SetNumberField("energy", Energy);
	JsonObject->SetNumberField("maxEnergy", MaxEnergy);
	JsonObject->SetBoolField("infiniteEnergy", InfiniteEnergy);
	if (Energy < MaxEnergy)
	{
		float RemainRestoreTime = GetWorld()->GetTimerManager().GetTimerRemaining(EnergyRestoreTimerHandle);
		JsonObject->SetNumberField("remainRestoreEnergySeconds", RemainRestoreTime);
	}

	auto TimeSystem = GetGameInstance()->GetSubsystem<UTimeSubsystem>();
	FDateTime SaveTime = TimeSystem->IsTimeValid() ? TimeSystem->GetUTCNow() : LastSaveTime;
	JsonObject->SetStringField("saveTime", SaveTime.ToIso8601());

	FString StringData;
	UMBUtilityFunctionLibrary::JsonObjectToString(JsonObject, StringData);

	UMBUtilityFunctionLibrary::SaveToStorage("Account", StringData);
}

void UAccountSubsystem::ParseAccount(const FString& JsonString)
{
	TSharedPtr<FJsonObject> JsonObject;

	if (!UMBUtilityFunctionLibrary::StringToJsonObject(JsonString, JsonObject))
		return;

	Level = JsonObject->GetNumberField("level");
	Experience = JsonObject->GetNumberField("exp");
	SoftCoins = JsonObject->GetNumberField("softCoins");
	PremCoins = JsonObject->GetNumberField("premCoins");
	MaxEnergy = JsonObject->GetNumberField("maxEnergy");
	InfiniteEnergy = JsonObject->GetBoolField("infiniteEnergy");
	int32 OldEnergy = JsonObject->GetNumberField("Energy");

	FString SaveTimeString;
	JsonObject->TryGetStringField("saveTime", SaveTimeString);
	FDateTime::ParseIso8601(*SaveTimeString, LastSaveTime);

	double RemainTimeSeconds = SecondsToRestoreEnergy;
	JsonObject->TryGetNumberField("remainRestoreEnergySeconds", RemainTimeSeconds);

	InitEnergy(OldEnergy, LastSaveTime, RemainTimeSeconds);
}

void UAccountSubsystem::InitAccount()
{
	FString SavedData;
	if (UMBUtilityFunctionLibrary::ReadFromStorage("Account", SavedData))
	{
		ParseAccount(SavedData);
	}
	else
	{
		Energy = MaxEnergy;
	}

	MaxExperience = GetMaxExperienceForLevel(Level);

	IsInitialized = true;
}

void UAccountSubsystem::InitEnergy(int32 OldEnergy, FDateTime OldTime, float OldRemainTime)
{
	auto TimeSystem = GetGameInstance()->GetSubsystem<UTimeSubsystem>();

	if (OldEnergy >= MaxEnergy)
	{
		Energy = OldEnergy;
		return;
	}

	float PassedTime = (TimeSystem->GetUTCNow() - OldTime).GetTotalSeconds();
	PassedTime += (SecondsToRestoreEnergy - OldRemainTime);
	int32 RestoredEnergy = FMath::TruncToInt(PassedTime / SecondsToRestoreEnergy);
	int32 NewEnergy = OldEnergy + RestoredEnergy;

	if (NewEnergy >= MaxEnergy)
	{
		Energy = MaxEnergy;
		return;
	}
	else
	{
		Energy = NewEnergy;
		
		float RemainTimeToRestore = SecondsToRestoreEnergy - (PassedTime - RestoredEnergy * SecondsToRestoreEnergy);
		StartRestoreEnergy(RemainTimeToRestore);
	}
}

int32 UAccountSubsystem::GetMaxExperienceForLevel(int32 InLevel)
{
	return InLevel * 50;
}

void UAccountSubsystem::StartRestoreEnergy(float NextRestoreTime)
{
	if (Energy >= MaxEnergy)
		return;

	FTimerDelegate Delegate = FTimerDelegate::CreateUObject(this, &UAccountSubsystem::RestoreEnergy);
	GetWorld()->GetTimerManager().SetTimer(EnergyRestoreTimerHandle, Delegate, SecondsToRestoreEnergy, true, NextRestoreTime);
}

void UAccountSubsystem::RestoreEnergy()
{
	if (Energy < MaxEnergy)
	{
		Energy++;
	}

	if (Energy >= MaxEnergy)
	{
		GetWorld()->GetTimerManager().ClearTimer(EnergyRestoreTimerHandle);
	}
}

bool UAccountSubsystem::HasEnoughEnergy(int32 EnergyToSpend)
{
	if (InfiniteEnergy)
		return true;

	return Energy >= EnergyToSpend;
}

bool UAccountSubsystem::SpendEnergy(int32 EnergyToSpend)
{
	if (EnergyToSpend <= 0)
		return true;

	if (!HasEnoughEnergy(EnergyToSpend))
		return false;

	if (InfiniteEnergy)
		return true;

	Energy -= EnergyToSpend;

	if (!GetWorld()->GetTimerManager().IsTimerActive(EnergyRestoreTimerHandle))
		StartRestoreEnergy();

	return true;
}

void UAccountSubsystem::SpendSoftCoins(int32 CoinsToSpend)
{
	if (CoinsToSpend <= 0)
		return;

	CoinsToSpend = FMath::Min(CoinsToSpend, SoftCoins);

	SoftCoins -= CoinsToSpend;
}

void UAccountSubsystem::SpendPremCoins(int32 CoinsToSpend)
{
	if (CoinsToSpend <= 0)
		return;

	CoinsToSpend = FMath::Min(CoinsToSpend, PremCoins);

	PremCoins -= CoinsToSpend;
}

bool UAccountSubsystem::GetRemainTimeToRestoreEnergy(int32& RemainTimeMinutes, int32& RemainTimeSeconds)
{
	if (!GetWorld()->GetTimerManager().IsTimerActive(EnergyRestoreTimerHandle))
		return false;

	float TotalRemainSeconds = GetWorld()->GetTimerManager().GetTimerRemaining(EnergyRestoreTimerHandle);

	RemainTimeMinutes = TotalRemainSeconds / 60;
	RemainTimeSeconds = FMath::CeilToInt(TotalRemainSeconds - RemainTimeMinutes * 60);

	return true;
}

void UAccountSubsystem::AddExperience(int32 DeltaExperience)
{
	int32 RemainDeltaExp = DeltaExperience;
	while (true)
	{
		int32 RemainExperienceToLvlUp = MaxExperience - Experience;
		if (RemainExperienceToLvlUp > RemainDeltaExp)
		{
			Experience += RemainDeltaExp;
			break;
		}

		LevelUp();
		RemainDeltaExp -= RemainExperienceToLvlUp;
	}
}

void UAccountSubsystem::AddSoftCoins(int32 DeltaCoins)
{
	if (DeltaCoins <= 0)
		return;

	SoftCoins += DeltaCoins;
}

void UAccountSubsystem::AddPremCoins(int32 DeltaCoins)
{
	if (DeltaCoins <= 0)
		return;
	
	PremCoins += DeltaCoins;
}

void UAccountSubsystem::AddEnergy(int32 DeltaEnergy)
{
	Energy += DeltaEnergy;

	if (Energy >= MaxEnergy)
	{
		GetWorld()->GetTimerManager().ClearTimer(EnergyRestoreTimerHandle);
	}
}

void UAccountSubsystem::LevelUp()
{
	Level++;
	Experience = 0;
	MaxExperience = GetMaxExperienceForLevel(Level);

	auto MergeSystem = GetGameInstance()->GetSubsystem<UMergeSubsystem>();
	for (auto& Reward : LevelRewards)
	{
		MergeSystem->AddNewReward(Reward);
	}

	OnGetNewLevel.Broadcast();
}