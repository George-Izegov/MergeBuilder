// Fill out your copyright notice in the Description page of Project Settings.


#include "Tutorial/MBTutorialSubsystem.h"

#include "MBGameInstance.h"
#include "MBUtilityFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

UMBTutorialSubsystem::UMBTutorialSubsystem()
{
}

void UMBTutorialSubsystem::Init()
{
	FString SavedData;
	if (UMBUtilityFunctionLibrary::ReadFromStorage("TutorialProgress", SavedData))
	{
		ParseProgress(SavedData);
	}
	else
	{
		TutorialStep = 0;
		bIsTutorialFinished = false;
	}

	auto GI = Cast<UMBGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	GI->OnGameLoaded.AddDynamic(this, &UMBTutorialSubsystem::StartTutorial);
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
	TutorialStep++;

	if (TutorialStep > 5)
	{
		FinishTutorial();
		return;
	}

	SaveProgress();

	StartTutorialStep();
}

void UMBTutorialSubsystem::FinishTutorial()
{
	bIsTutorialFinished = true;
}

