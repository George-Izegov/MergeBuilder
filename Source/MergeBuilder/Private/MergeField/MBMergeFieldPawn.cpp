// Fill out your copyright notice in the Description page of Project Settings.


#include "MergeField/MBMergeFieldPawn.h"
#include "Kismet/GameplayStatics.h"
#include "MergeField/MBMergeFieldManager.h"

// Sets default values
AMBMergeFieldPawn::AMBMergeFieldPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMBMergeFieldPawn::BeginPlay()
{
	Super::BeginPlay();
	
	check(FieldManager);
}

void AMBMergeFieldPawn::TouchPress(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (FingerIndex != ETouchIndex::Type::Touch1)
		return;

	AMBBasePawn::TouchPress(FingerIndex, Location);

	FHitResult HitResult;
	if (GetInputHitResult(FingerIndex, HitResult))
	{
		if (FieldManager->IsLocationOnField(HitResult.Location))
		{
			FIntPoint Index;
			FieldManager->GetIndexForLocation(HitResult.Location, Index);

			FieldManager->HandleStartTouchOnIndex(Index);
		}
	}
}

void AMBMergeFieldPawn::TouchRelease(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (FingerIndex != ETouchIndex::Type::Touch1)
		return;

	AMBBasePawn::TouchRelease(FingerIndex, Location);

	InDrag = false;

	FHitResult HitResult;
	if (GetInputHitResult(FingerIndex, HitResult))
	{
		FIntPoint Index;
		FieldManager->GetIndexForLocation(HitResult.Location, Index);

		FieldManager->HandleReleaseTouchOnIndex(Index);
	}
}

void AMBMergeFieldPawn::TouchMove(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (FingerIndex != ETouchIndex::Type::Touch1)
		return;

	AMBBasePawn::TouchMove(FingerIndex, Location);

	FHitResult HitResult;
	if (!GetInputHitResult(FingerIndex, HitResult))
		return;

	if (!InDrag)
	{
		if (!FieldManager->IsLocationOnField(HitResult.Location))
			return;
		
		if (FVector::Distance(Location, StartTouchLocation) >= DragDeltaInputShift)
		{
			InDrag = true;
			FieldManager->StartDrag();
		}
		else
		{
			return;
		}
	}
	
	FieldManager->HandleDrag(HitResult.Location);
}

void AMBMergeFieldPawn::OnClick(const FVector Location)
{
	AMBBasePawn::OnClick(Location);

	FHitResult HitResult;
	if (GetInputHitResult(ETouchIndex::Type::Touch1, HitResult))
	{
		if (HitResult.Actor->ActorHasTag(FName("RewardActor")))
		{
			FieldManager->HandleClickOnReward();
		}
		else
		{
			if (FieldManager->IsLocationOnField(HitResult.Location))
			{
				FIntPoint Index;
				FieldManager->GetIndexForLocation(HitResult.Location, Index);

				FieldManager->HandleClickOnIndex(Index);
			}
		}
	}
}

// Called every frame
void AMBMergeFieldPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMBMergeFieldPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AMBMergeFieldPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	FieldManager->InitializeField();
}

