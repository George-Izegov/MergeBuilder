// Fill out your copyright notice in the Description page of Project Settings.


#include "MBBasePlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraActor.h"
#include "MergeField/MBMergeFieldManager.h"

void AMBBasePlayerController::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), ACameraActor::StaticClass(), FName("MergeField"), FoundActors);

	if (FoundActors.Num() != 0)
	{
		MergeFieldCamera = Cast<ACameraActor>(FoundActors[0]);
	}
	else
	{
		check(MergeFieldCamera);
		UE_LOG(LogTemp, Warning, TEXT("AMBBasePlayerController::BeginPlay() - There is no MergeField Camera"))
	}

	FieldManager = Cast<AMBMergeFieldManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AMBMergeFieldManager::StaticClass()));

	check(FieldManager);
}

void AMBBasePlayerController::SwitchToMergeField()
{
	SetViewTargetWithBlend(MergeFieldCamera);

	FieldManager->InitializeField();

}

void AMBBasePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AMBBasePlayerController::TouchPress);
	InputComponent->BindTouch(EInputEvent::IE_Released, this, &AMBBasePlayerController::TouchRelease);
	InputComponent->BindTouch(EInputEvent::IE_DoubleClick, this, &AMBBasePlayerController::TouchDouble);
	InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AMBBasePlayerController::TouchMove);
}

void AMBBasePlayerController::TouchPress(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (FingerIndex != ETouchIndex::Type::Touch1)
		return;

	StartTouchLocation = Location;
	OnPressedTime = FDateTime::Now();

	FHitResult HitResult;
	if (GetInputHitResult(FingerIndex, HitResult))
	{
		FIntPoint Index;
		FieldManager->GetIndexForLocation(HitResult.Location, Index);

		FieldManager->HandleStartTouchOnIndex(Index);
	}
}

void AMBBasePlayerController::TouchRelease(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (FingerIndex != ETouchIndex::Type::Touch1)
		return;

	InDrag = false;

	if ((FDateTime::Now() - OnPressedTime).GetTotalMilliseconds() <= 200)
		OnClick(StartTouchLocation);

	FHitResult HitResult;
	if (GetInputHitResult(FingerIndex, HitResult))
	{
		FIntPoint Index;
		FieldManager->GetIndexForLocation(HitResult.Location, Index);

		FieldManager->HandleReleaseTouchOnIndex(Index);
	}
}

void AMBBasePlayerController::OnClick(const FVector Location)
{
	FHitResult HitResult;
	if (GetInputHitResult(ETouchIndex::Type::Touch1, HitResult))
	{
		if (HitResult.Actor->ActorHasTag(FName("RewardActor")))
		{
			FieldManager->HandleClickOnReward();
		}
		else
		{
			FIntPoint Index;
			FieldManager->GetIndexForLocation(HitResult.Location, Index);

			FieldManager->HandleClickOnIndex(Index);
		}
	}
}

void AMBBasePlayerController::TouchDouble(const ETouchIndex::Type FingerIndex, const FVector Location)
{
}

void AMBBasePlayerController::TouchMove(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (FingerIndex != ETouchIndex::Type::Touch1)
		return;

	if (!InDrag)
	{
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

	FHitResult HitResult;
	if (GetInputHitResult(FingerIndex, HitResult))
	{
		FieldManager->HandleDrag(HitResult.Location);
	}
}

bool AMBBasePlayerController::GetInputHitResult(const ETouchIndex::Type FingerIndex, FHitResult& HitResult)
{
	float TouchX, TouchY;
	bool OnTouch;

	GetInputTouchState(FingerIndex, TouchX, TouchY, OnTouch);

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	// Input Object type
	ObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery7);

	// get hit under finger
	return GetHitResultAtScreenPosition(FVector2D(TouchX, TouchY), ObjectTypes, false, HitResult);
}