// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MBBasePawn.h"
#include "Components/SphereComponent.h"
#include "Utilities/MBCoreTypes.h"
#include "TopDownPawn.generated.h"

UCLASS()
class MERGEBUILDER_API ATopDownPawn : public AMBBasePawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATopDownPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void TouchPress(const ETouchIndex::Type FingerIndex, const FVector Location) override;
	virtual void TouchRelease(const ETouchIndex::Type FingerIndex, const FVector Location) override;
	virtual void TouchMove(const ETouchIndex::Type FingerIndex, const FVector Location) override;
	virtual void OnClick(const FVector Location) override;

	void TouchDouble(const ETouchIndex::Type FingerIndex, const FVector Location);

	UFUNCTION(BlueprintCallable)
	void HandleGesture(float DeltaDistance, float DeltaAngle);

	UFUNCTION(BlueprintImplementableEvent)
		void AddSpringArmLength(float Delta);

	UFUNCTION(BlueprintImplementableEvent)
		float GetSpringArmLength();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	bool GetWorldObjectHitResult(const ETouchIndex::Type FingerIndex, FHitResult& HitResult);

protected:

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		USphereComponent* RootSphere;

	UPROPERTY(EditAnywhere)
		float MovementSpeed = 15;
	
	bool DragItem = false;

	FVector PrevDragLocation;

	UPROPERTY(BlueprintAssignable)
	FNoParamsSignatureDyn OnVoidClick;

	FVector StartTouch2Location;
	FVector DeltaVectorTwoFingersTouch2;
	FVector DeltaVectorTwoFingersTouch1;
	bool TwoFingersTouch = false;
};
