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

	void TouchPress(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchRelease(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchDouble(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchMove(const ETouchIndex::Type FingerIndex, const FVector Location);
	void OnClick(const FVector Location);

	bool GetInputHitResult(const ETouchIndex::Type FingerIndex, FHitResult& HitResult);

public:

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
		void SwitchToMergeField();

protected:

	UPROPERTY(BlueprintReadOnly)
	class ACameraActor* MergeFieldCamera;
	UPROPERTY(BlueprintReadOnly)
	class AMBMergeFieldManager* FieldManager;

	UPROPERTY(EditAnywhere)
		float DragDeltaInputShift = 10.0f;

	FVector StartTouchLocation;
	FDateTime OnPressedTime;
	bool InDrag = false;

};
