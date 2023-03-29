// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MBBasePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MERGEBUILDER_API AMBBasePlayerController : public APlayerController
{
	GENERATED_BODY()
protected:

	virtual void SetupInputComponent() override;

public:

	AMBBasePlayerController();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
		void SwitchToMergeField();

	UFUNCTION(BlueprintCallable)
		void SwitchToCity();

protected:

	UPROPERTY(BlueprintReadOnly)
	class AMBMergeFieldPawn* MergeFieldPawn;

	UPROPERTY(BlueprintReadOnly)
	class ATopDownPawn* TopDownPawn;

	TSubclassOf<class UUserWidget> LoadingScreenClass;
	
public:
	
	UPROPERTY()
	UUserWidget* LoadingScreen;
};
