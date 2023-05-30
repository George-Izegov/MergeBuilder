// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseButtonWidget.generated.h"

/**
 * 
 */
UCLASS()
class MERGEBUILDER_API UBaseButtonWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintPure)
	bool IsButtonInteractable() const;

	UFUNCTION(BlueprintCallable)
	static void SetTutorialButton(UBaseButtonWidget* NewTutorialButton);
};
