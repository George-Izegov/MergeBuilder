// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MBBasePawn.h"
#include "MBMergeFieldPawn.generated.h"

UCLASS()
class MERGEBUILDER_API AMBMergeFieldPawn : public AMBBasePawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMBMergeFieldPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void TouchPress(const ETouchIndex::Type FingerIndex, const FVector Location) override;
	virtual void TouchRelease(const ETouchIndex::Type FingerIndex, const FVector Location) override;
	virtual void TouchMove(const ETouchIndex::Type FingerIndex, const FVector Location) override;
	virtual void OnClick(const FVector Location) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PossessedBy(AController* NewController) override;
protected:

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		class AMBMergeFieldManager* FieldManager;

	UPROPERTY(EditAnywhere)
		float DragDeltaInputShift = 10.0f;

	bool InDrag = false;
};
