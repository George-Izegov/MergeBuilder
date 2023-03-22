// Fill out your copyright notice in the Description page of Project Settings.


#include "MBBasePawn.h"

// Sets default values
AMBBasePawn::AMBBasePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMBBasePawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMBBasePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMBBasePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AMBBasePawn::TouchPress);
	PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &AMBBasePawn::TouchRelease);
	PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AMBBasePawn::TouchMove);
}

void AMBBasePawn::TouchPress(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (FingerIndex == ETouchIndex::Touch1)
	{
		StartTouchLocation = Location;
		OnPressedTime = FDateTime::Now();
	}
}

void AMBBasePawn::TouchRelease(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (FingerIndex != ETouchIndex::Touch1)
		return;

	if ((FDateTime::Now() - OnPressedTime).GetTotalMilliseconds() <= 200)
		OnClick(StartTouchLocation);
}

void AMBBasePawn::TouchMove(const ETouchIndex::Type FingerIndex, const FVector Location)
{
}

void AMBBasePawn::OnClick(const FVector Location)
{
}

bool AMBBasePawn::GetInputHitResult(const ETouchIndex::Type FingerIndex, FHitResult& HitResult)
{
	APlayerController* PC = GetGameInstance()->GetFirstLocalPlayerController();
	float TouchX, TouchY;
	bool OnTouch;

	PC->GetInputTouchState(FingerIndex, TouchX, TouchY, OnTouch);

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	// Input Object type
	ObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery7);

	// get hit under finger
	return PC->GetHitResultAtScreenPosition(FVector2D(TouchX, TouchY), ObjectTypes, false, HitResult);
}

