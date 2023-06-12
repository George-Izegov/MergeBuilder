// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MBBasePawn.generated.h"

UCLASS()
class MERGEBUILDER_API AMBBasePawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMBBasePawn();
	~AMBBasePawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void TouchPress(const ETouchIndex::Type FingerIndex, const FVector Location);
	virtual void TouchRelease(const ETouchIndex::Type FingerIndex, const FVector Location);
	virtual void TouchMove(const ETouchIndex::Type FingerIndex, const FVector Location);
	virtual void OnClick(const FVector Location);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	bool GetInputHitResult(const ETouchIndex::Type FingerIndex, FHitResult& HitResult);

	static bool IsActorClickable(AActor* ActorToClick);
protected:

	FVector StartTouchLocation;
	FDateTime OnPressedTime;
	bool FirstTouchStarted = false;
};
