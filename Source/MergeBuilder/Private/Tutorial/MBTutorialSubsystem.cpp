// Fill out your copyright notice in the Description page of Project Settings.


#include "Tutorial/MBTutorialSubsystem.h"

#include "MBGameInstance.h"
#include "MBUtilityFunctionLibrary.h"
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
			// TODO:: Add analytic of this
		}
	}
	else
	{
		TutorialStep = 0;
		bIsTutorialFinished = false;
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
	TutorialStep++;

	SaveProgress();
}

void UMBTutorialSubsystem::FinishTutorial()
{
	bIsTutorialFinished = true;

	SaveProgress();

	SetTutorialObject(nullptr);

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

static UObject* TutorialObject;

void UMBTutorialSubsystem::SetTutorialObject(UObject* NewTutorialObject)
{
	TutorialObject = NewTutorialObject;
}

UObject* UMBTutorialSubsystem::GetTutorialObject()
{
	return TutorialObject;
}