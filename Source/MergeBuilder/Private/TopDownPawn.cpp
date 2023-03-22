// Fill out your copyright notice in the Description page of Project Settings.


#include "TopDownPawn.h"
#include "CitySystem/MBBaseCityObjectActor.h"
#include "CitySystem/MBCityBuilderManager.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATopDownPawn::ATopDownPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootSphere = CreateDefaultSubobject<USphereComponent>(FName("RootSphere"));
	SetRootComponent(RootSphere);
}

// Called when the game starts or when spawned
void ATopDownPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATopDownPawn::TouchPress(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	AMBBasePawn::TouchPress(FingerIndex, Location);

	if (FingerIndex == ETouchIndex::Touch1)
	{
		FHitResult HitResult;
		if (GetWorldObjectHitResult(FingerIndex, HitResult))
		{
			if (HitResult.Actor->GetClass()->IsChildOf<AMBBaseCityObjectActor>())
			{
				auto CityManager = Cast<AMBCityBuilderManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AMBCityBuilderManager::StaticClass()));
				if (HitResult.Actor.Get() == CityManager->GetEditedObject())
				{
					DragItem = true;
					GetInputHitResult(FingerIndex, HitResult);
					PrevDragLocation = HitResult.Location;
				}
			}
		}
	}

	if (FingerIndex == ETouchIndex::Touch2)
	{
		DeltaVectorTwoFingers = Location - StartTouchLocation;
		TwoFingersTouch = true;
	}
}

void ATopDownPawn::TouchRelease(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	AMBBasePawn::TouchRelease(FingerIndex, Location);

	if (FingerIndex == ETouchIndex::Touch1)
	{
		InMovement = false;
		DragItem = false;
	}

	if (FingerIndex == ETouchIndex::Touch2)
	{
		TwoFingersTouch = false;
	}
}

void ATopDownPawn::TouchDouble(const ETouchIndex::Type FingerIndex, const FVector Location)
{
}

void ATopDownPawn::HandleGesture(float DeltaDistance, float DeltaAngle)
{
	AddSpringArmLength(DeltaDistance / GSystemResolution.ResX);

	FRotator DeltaRotation = FRotator::ZeroRotator;
	DeltaRotation.Yaw = FMath::RadiansToDegrees(DeltaAngle);
	AddActorWorldRotation(DeltaRotation);
}

void ATopDownPawn::TouchMove(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	AMBBasePawn::TouchMove(FingerIndex, Location);

	if (FingerIndex == ETouchIndex::Touch1)
	{
		if (DragItem)
		{
			auto CityManager = Cast<AMBCityBuilderManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AMBCityBuilderManager::StaticClass()));
			FHitResult HitResult;
			if (GetInputHitResult(FingerIndex, HitResult))
			{
				CityManager->MoveEditedObject(HitResult.Location - PrevDragLocation);
				PrevDragLocation = HitResult.Location;
			}
		}
		else
		{
			if (!TwoFingersTouch)
			{
				FVector DeltaLocation = Location - StartTouchLocation;
				DeltaLocation.Z = 0.0f;
				DeltaLocation = DeltaLocation.RotateAngleAxis(GetActorRotation().Yaw, FVector::UpVector);

				switch (MovementType)
				{
				case 0:
				{
					InMovement = true;
					LastMovementTouchLocation = Location;
					break;
				}
				case 1:
				{
					RootSphere->AddForce(DeltaLocation * MovementSpeed[MovementType]);
					StartTouchLocation = Location;
					break;
				}
				}
			}
		}
	}

	if (FingerIndex == ETouchIndex::Touch2)
	{
		FVector CurrentDeltaVector = Location - StartTouchLocation;

		float DeltaDistance = CurrentDeltaVector.Size() - DeltaVectorTwoFingers.Size();

		FVector PrevNormalized = DeltaVectorTwoFingers.GetSafeNormal();
		FVector CurNormalized = CurrentDeltaVector.GetSafeNormal();

		float DeltaAngle = CurNormalized.HeadingAngle() - PrevNormalized.HeadingAngle();
		
		if (FMath::Sign(CurNormalized.HeadingAngle() * PrevNormalized.HeadingAngle()) < 0)
		{
			DeltaAngle = DeltaAngle + 2.0f * PI * FMath::Sign(PrevNormalized.HeadingAngle());
		}

		GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Orange, FString::Printf(TEXT("Gesture Dist: %.1f Angle: %.3f"), DeltaDistance, DeltaAngle));
		
		HandleGesture(DeltaDistance, DeltaAngle);

		DeltaVectorTwoFingers = CurrentDeltaVector;
	}
	
}

void ATopDownPawn::OnClick(const FVector Location)
{
	AMBBasePawn::OnClick(Location);

	auto CityManager = Cast<AMBCityBuilderManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AMBCityBuilderManager::StaticClass()));
	FHitResult HitResult;
	if (GetWorldObjectHitResult(ETouchIndex::Touch1, HitResult))
	{
		auto CityObject = Cast<AMBBaseCityObjectActor>(HitResult.Actor);
		if (CityObject)
		{
			CityManager->HandleObjectClick(CityObject);
		}
	}
	else
	{
		OnVoidClick.Broadcast();
	}
}

// Called every frame
void ATopDownPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (InMovement)
	{
		FVector DeltaLocation = LastMovementTouchLocation - StartTouchLocation;
		DeltaLocation.Z = 0.0f;
		DeltaLocation = DeltaLocation.RotateAngleAxis(GetActorRotation().Yaw, FVector::UpVector);
		RootSphere->AddForce(DeltaLocation * MovementSpeed[MovementType]);
	}

}

// Called to bind functionality to input
void ATopDownPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindTouch(EInputEvent::IE_DoubleClick, this, &ATopDownPawn::TouchDouble);
}

bool ATopDownPawn::GetWorldObjectHitResult(const ETouchIndex::Type FingerIndex, FHitResult& HitResult)
{
	APlayerController* PC = GetGameInstance()->GetFirstLocalPlayerController();
	float TouchX, TouchY;
	bool OnTouch;

	PC->GetInputTouchState(FingerIndex, TouchX, TouchY, OnTouch);

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	// Input Object type
	ObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery8);

	// get hit under finger
	return PC->GetHitResultAtScreenPosition(FVector2D(TouchX, TouchY), ObjectTypes, false, HitResult);
}
