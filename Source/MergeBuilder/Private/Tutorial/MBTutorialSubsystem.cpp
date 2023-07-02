// Fill out your copyright notice in the Description page of Project Settings.


#include "Tutorial/MBTutorialSubsystem.h"

#include "MBGameInstance.h"
#include "MBUtilityFunctionLibrary.h"
#include "Analytics/FGAnalytics.h"
#include "CitySystem/CityBuilderSubsystem.h"
#include "CitySystem/CityObjectsData.h"
#include "Kismet/GameplayStatics.h"

UMBTutorialSubsystem::UMBTutorialSubsystem()
{
}

UMBTutorialSubsystem::~UMBTutorialSubsystem()
{
	SetTutorialObject(nullptr);
}

void UMBTutorialSubsystem::Init()
{
	FString SavedData;
	if (UMBUtilityFunctionLibrary::ReadFromStorage("TutorialProgress", SavedData))
	{
		ParseProgress(SavedData);
		if (TutorialStep > 0 && !bIsTutorialFinished)
		{
			bIsTutorialFinished = true;
			UFGAnalytics::LogEvent("tutorial_skip");
		}
	}
	else
	{
		TutorialStep = 0;
		bIsTutorialFinished = false;
		UFGAnalytics::LogEvent("tutorial_begin");
	}

	if (!bIsTutorialFinished)
	{
		auto GI = Cast<UMBGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
		GI->OnGameLoaded.AddDynamic(this, &UMBTutorialSubsystem::StartTutorial);
	}
}

bool UMBTutorialSubsystem::IsTutorialFinished() const
{
	return bIsTutorialFinished;
}

bool UMBTutorialSubsystem::ParseProgress(const FString& JsonString)
{
	TSharedPtr<FJsonObject> JsonObject;

	if (!UMBUtilityFunctionLibrary::StringToJsonObject(JsonString, JsonObject))
		return false;

	JsonObject->TryGetNumberField("TutorialStep", TutorialStep);
	JsonObject->TryGetBoolField("IsTutorialFinished", bIsTutorialFinished);

	return true;
}

void UMBTutorialSubsystem::SaveProgress()
{
	TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();

	JsonObject->SetNumberField("TutorialStep", TutorialStep);
	JsonObject->SetBoolField("IsTutorialFinished", bIsTutorialFinished);
	
	FString StringData;
	UMBUtilityFunctionLibrary::JsonObjectToString(JsonObject, StringData);

	UMBUtilityFunctionLibrary::SaveToStorage("TutorialProgress", StringData);
}

void UMBTutorialSubsystem::StartTutorial()
{
	if (bIsTutorialFinished)
		return;

	StartTutorialStep();

	MakeTutorialPrices();
}

void UMBTutorialSubsystem::NextTutorialStep()
{
	if (TutorialStep > 3)
	{
		FinishTutorial();
		return;
	}

	StartTutorialStep();
}

void UMBTutorialSubsystem::IncrementTutorialStep()
{
	UFGAnalytics::LogEvent("tutorial_step_" + FString::FromInt(TutorialStep));
	TutorialStep++;

	SaveProgress();
}

void UMBTutorialSubsystem::FinishTutorial()
{
	bIsTutorialFinished = true;

	SaveProgress();

	SetTutorialObject(nullptr);

	RevertTutorialPrices();

	UFGAnalytics::LogEvent("tutorial_complete");

	FTimerHandle TimerHandle;
	FTimerDelegate Delegate = FTimerDelegate::CreateLambda([]()
	{
		CollectGarbage(RF_NoFlags);
	});
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, Delegate, 0.1f, false);
}

APlayerController* UMBTutorialSubsystem::GetPlayerController()
{
	return UGameplayStatics::GetPlayerController(GetWorld(), 0);
}

void UMBTutorialSubsystem::MakeTutorialPrices()
{
	auto GI = Cast<UMBGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	auto CityBuilderSubsystem = GI->GetSubsystem<UCityBuilderSubsystem>();

	// Mine price
	FCityObjectData* RowStruct = CityBuilderSubsystem->CityObjectsDataTable->FindRow<FCityObjectData>(FName("Mine"), "");
	RowStruct->RequiredItems[0].RequiredNum = 1;
	RowStruct->RequiredItems[0].Item.Level = 3;
}

void UMBTutorialSubsystem::RevertTutorialPrices()
{
	auto GI = Cast<UMBGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	auto CityBuilderSubsystem = GI->GetSubsystem<UCityBuilderSubsystem>();

	// Mine price
	FCityObjectData* RowStruct = CityBuilderSubsystem->CityObjectsDataTable->FindRow<FCityObjectData>(FName("Mine"), "");
	RowStruct->RequiredItems[0].RequiredNum = 3;
	RowStruct->RequiredItems[0].Item.Level = 6;
}

static UObject* TutorialObject;

void UMBTutorialSubsystem::SetTutorialObject(UObject* NewTutorialObject)
{
	TutorialObject = NewTutorialObject;
}

UObject* UMBTutorialSubsystem::GetTutorialObject()
{
	return TutorialObject;
}