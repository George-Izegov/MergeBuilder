// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MBCoreTypes.h"
#include "Blueprint/UserWidget.h"
#include "TutorialStepWidget.generated.h"

/**
 * 
 */
UCLASS()
class MERGEBUILDER_API UTutorialStepWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void StartStep();

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FNoParamsSignatureDyn OnFinished;
};
