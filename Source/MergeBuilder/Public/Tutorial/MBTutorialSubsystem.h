// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MBTutorialSubsystem.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class MERGEBUILDER_API UMBTutorialSubsystem : public UObject
{
	GENERATED_BODY()

	friend class UMBGameInstance;
	
public:

	UMBTutorialSubsystem();

	~UMBTutorialSubsystem();
	
	void Init();

	UFUNCTION(BlueprintPure)
	bool IsTutorialFinished() const;

	UFUNCTION(BlueprintCallable)
	static void SetTutorialObject(UObject* NewTutorialObject);

	UFUNCTION(BlueprintPure)
	static UObject* GetTutorialObject();

protected:

	bool ParseProgress(const FString& JsonString);

	void SaveProgress();

	UFUNCTION()
	void StartTutorial();

	UFUNCTION(BlueprintImplementableEvent)
	void StartTutorialStep();

	UFUNCTION(BlueprintCallable)
	void NextTutorialStep();

	void FinishTutorial();

	UFUNCTION(BlueprintPure)
	APlayerController* GetPlayerController();

protected:

	UPROPERTY(BlueprintReadOnly)
	int32 TutorialStep = 0;
	
	bool bIsTutorialFinished = true;
};
