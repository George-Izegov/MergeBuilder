// Fill out your copyright notice in the Description page of Project Settings.


#include "TopDownPawn.h"

#include "MBGameInstance.h"
#include "CitySystem/MBBaseCityObjectActor.h"
#include "CitySystem/MBCityBuilderManager.h"
#include "Kismet/GameplayStatics.h"
#include "CitySystem/MBGroundFieldManager.h"
#include "GameFramework/HUD.h"

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

	if (!UMBGameInstance::GetTutorialSubsystem()->IsTutorialFinished())
		return;
	
	if (FingerIndex == ETouchIndex::Touch1)
	{
		PrevMoveLocation1 = Location;
		
		TArray<FHitResult> HitResults;
		if (GetMultiWorldObjectHitResults(FingerIndex, HitResults))
		{
			auto CityManager = Cast<AMBCityBuilderManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AMBCityBuilderManager::StaticClass()));

			for (auto& HitResult : HitResults)
			{
				if (HitResult.GetActor() == CityManager->GetEditedObject())
				{
					DragItem = true;
					GetInputHitResult(FingerIndex, HitResult);
					PrevDragLocation = HitResult.Location;
					DragDistance = FVector::ZeroVector;
				}
			}
		}
	}

	if (FingerIndex == ETouchIndex::Touch2)
	{
		StartTouch2Location = Location;
		PrevMoveLocation2 = Location;
		DeltaVectorTwoFingersTouch2 = PrevMoveLocation2 - PrevMoveLocation1;
		DeltaVectorTwoFingersTouch1 = PrevMoveLocation1 - PrevMoveLocation2;
		TwoFingersTouch = true;
	}
}

void ATopDownPawn::TouchRelease(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	AMBBasePawn::TouchRelease(FingerIndex, Location);
	
	if (FingerIndex == ETouchIndex::Touch1)
	{
		if (DragItem)
		{
			auto CityManager = Cast<AMBCityBuilderManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AMBCityBuilderManager::StaticClass()));
			CityManager->HandleDragRelease();
		}
		
		DragItem = false;
		TwoFingersTouch = false;
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
	float PinchSpeed = 0.7f;
	float RotationSpeed = 0.7f;

	AddSpringArmLength(-DeltaDistance / GSystemResolution.ResX * PinchSpeed);

	FRotator DeltaRotation = FRotator::ZeroRotator;
	DeltaRotation.Yaw = -1 * FMath::RadiansToDegrees(DeltaAngle) * RotationSpeed;
	AddActorWorldRotation(DeltaRotation);
}

void ATopDownPawn::TouchMove(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	AMBBasePawn::TouchMove(FingerIndex, Location);
	
	if (!UMBGameInstance::GetTutorialSubsystem()->IsTutorialFinished())
		return;
	
	if (FingerIndex == ETouchIndex::Touch1 && FirstTouchStarted)
	{
		if (DragItem)
		{
			auto CityManager = Cast<AMBCityBuilderManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AMBCityBuilderManager::StaticClass()));
			FHitResult HitResult;
			if (GetInputHitResult(FingerIndex, HitResult))
			{
				DragDistance += HitResult.Location - PrevDragLocation;

				int32 Grid = CityManager->BuildGrid;
				FVector DeltaMovement = FVector(FMath::TruncToInt(DragDistance.X / Grid) * Grid, FMath::TruncToInt(DragDistance.Y / Grid) * Grid, 0.0f);
				CityManager->MoveEditedObject(DeltaMovement);
				DragDistance -= DeltaMovement;
				PrevDragLocation = HitResult.Location;
			}
		}
		else
		{
			if (!TwoFingersTouch)
			{
				FVector DeltaLocation = Location - PrevMoveLocation1;
				DeltaLocation.Z = 0.0f;
				DeltaLocation = DeltaLocation.RotateAngleAxis(GetActorRotation().Yaw, FVector::UpVector);
				
				float Force = GetSpringArmLength() / 2500.0f * MovementSpeed;
				RootSphere->AddForce(DeltaLocation * Force);
			}
		}
	}

	if (TwoFingersTouch)
	{
		if (FingerIndex == ETouchIndex::Touch1 || FingerIndex == ETouchIndex::Touch2)
		{
			FVector CurrentDeltaVector;
			FVector PrevDeltaVector;
			
			if (FingerIndex == ETouchIndex::Touch1)
			{
				CurrentDeltaVector = Location - PrevMoveLocation2;
				PrevDeltaVector = DeltaVectorTwoFingersTouch1;
			}
			else if (FingerIndex == ETouchIndex::Touch2)
			{
				CurrentDeltaVector = Location - PrevMoveLocation1;
				PrevDeltaVector = DeltaVectorTwoFingersTouch2;
			}

			float DeltaDistance = CurrentDeltaVector.Size() - PrevDeltaVector.Size();

			FVector PrevNormalized = PrevDeltaVector.GetSafeNormal();
			FVector CurNormalized = CurrentDeltaVector.GetSafeNormal();

			float DeltaAngle = CurNormalized.HeadingAngle() - PrevNormalized.HeadingAngle();

			if (FMath::Sign(CurNormalized.HeadingAngle() * PrevNormalized.HeadingAngle()) < 0 && FMath::Abs(DeltaAngle) > PI)
			{
				DeltaAngle = DeltaAngle + 2.0f * PI * FMath::Sign(PrevNormalized.HeadingAngle());
			}

			HandleGesture(DeltaDistance, DeltaAngle);

			if (FingerIndex == ETouchIndex::Touch1)
			{
				DeltaVectorTwoFingersTouch1 = CurrentDeltaVector;
			}
			else if (FingerIndex == ETouchIndex::Touch2)
			{
				DeltaVectorTwoFingersTouch2 = CurrentDeltaVector;
			}
		}
	}

	if (FingerIndex == ETouchIndex::Touch1)
	{
		PrevMoveLocation1 = Location;
	}
	else if (FingerIndex == ETouchIndex::Touch2)
	{
		PrevMoveLocation2 = Location;
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
		if (IsActorClickable(CityObject))
		{
			CityManager->HandleObjectClick(CityObject);
		}
	}
	else
	{
		if (UMBGameInstance::GetTutorialSubsystem()->IsTutorialFinished())
		{
			OnVoidClick.Broadcast();
		}
	}
}

// Called every frame
void ATopDownPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	auto GroundFieldSubsystem = GetGameInstance()->GetSubsystem<UMBGroundSubsystem>();

	FVector MinBoundingLocation, MaxBoundingLocation;

	if (GroundFieldSubsystem)
	{
		GroundFieldSubsystem->GetBoundingSquare(MinBoundingLocation, MaxBoundingLocation);

		FVector BoundLocation = GetActorLocation();

		BoundLocation.X = FMath::Clamp(BoundLocation.X, MinBoundingLocation.X, MaxBoundingLocation.X);
		BoundLocation.Y = FMath::Clamp(BoundLocation.Y, MinBoundingLocation.Y, MaxBoundingLocation.Y);

		if (!BoundLocation.Equals(GetActorLocation()))
		{
			SetActorLocation(BoundLocation);
		}
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

bool ATopDownPawn::GetMultiWorldObjectHitResults(const ETouchIndex::Type FingerIndex, TArray<FHitResult>& HitResults)
{
	APlayerController* PC = GetGameInstance()->GetFirstLocalPlayerController();
	float TouchX, TouchY;
	bool OnTouch;

	PC->GetInputTouchState(FingerIndex, TouchX, TouchY, OnTouch);

	FVector2D ScreenPosition = FVector2D(TouchX, TouchY);

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	// Input Object type
	ObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery8);
	
	// Early out if we clicked on a HUD hitbox
	if (PC->GetHUD() != NULL && PC->GetHUD()->GetHitBoxAtCoordinates(ScreenPosition, true))
	{
		return false;
	}
	
	FVector WorldOrigin;
	FVector WorldDirection;
	if (UGameplayStatics::DeprojectScreenToWorld(PC, ScreenPosition, WorldOrigin, WorldDirection) == true)
	{
		FCollisionObjectQueryParams const ObjParam(ObjectTypes);
		return GetWorld()->LineTraceMultiByObjectType(HitResults, WorldOrigin, WorldOrigin + WorldDirection * PC->HitResultTraceDistance, ObjParam, FCollisionQueryParams(SCENE_QUERY_STAT(ClickableTrace), false));
	}

	return false;
}
