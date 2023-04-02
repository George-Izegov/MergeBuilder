// Fill out your copyright notice in the Description page of Project Settings.


#include "CitySystem/MBBaseCityObjectActor.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
AMBBaseCityObjectActor::AMBBaseCityObjectActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(FName("Root"));
	SetRootComponent(Root);

	auto AttachmentRules = FAttachmentTransformRules(EAttachmentRule::KeepRelative, false);
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Base Mesh"));
	BaseMesh->AttachToComponent(Root, AttachmentRules);
}

// Called when the game starts or when spawned
void AMBBaseCityObjectActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMBBaseCityObjectActor::Initialize(const FCityObject& ObjectStruct, const FCityObjectData*& ObjectTableData)
{
	CityObjectData = ObjectStruct;
}

// Called every frame
void AMBBaseCityObjectActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AMBBaseCityObjectActor::CheckLocation()
{
	TArray<AActor*> OverlappingActors;
	BaseMesh->GetOverlappingActors(OverlappingActors, AMBBaseCityObjectActor::StaticClass());

	return OverlappingActors.Num() == 0;
}

void AMBBaseCityObjectActor::TrySnapToClosestObject()
{
	if (!CanSnap)
		return;
	
	TArray<AActor*> CityObjects;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMBBaseCityObjectActor::StaticClass(), CityObjects);

	float MaxDistanceToCheck = 3000.0f;
	TArray<AActor*> ClosestObjects;
	for (AActor* Object : CityObjects)
	{
		if (Object == this)
			continue;
		float Distance = FVector::Dist(Object->GetActorLocation(), GetActorLocation());

		if (Distance > MaxDistanceToCheck)
			continue;
		
		auto CityObject = Cast<AMBBaseCityObjectActor>(Object);

		if (!CityObject->CanSnap)
			continue;
		
		ClosestObjects.Add(Object);
	}

	if (ClosestObjects.Num() == 0)
		return;

	TArray<UActorComponent*> SceneComponents;
	for (auto Object : ClosestObjects)
	{
		TArray<UActorComponent*> Components;
		Object->GetComponents(USceneComponent::StaticClass(), Components, true);

		SceneComponents.Insert(Components, SceneComponents.Num());
	}

	TArray<USceneComponent*> OtherSnapComponents;
	for (UActorComponent* SceneComponent : SceneComponents)
	{
		if (USceneComponent* SnapComponent = Cast<USceneComponent>(SceneComponent))
		{
			if (SnapComponent->ComponentHasTag("Snap"))
				OtherSnapComponents.Add(SnapComponent);
		}
	}

	if (OtherSnapComponents.Num() == 0)
		return;

	SceneComponents.Empty();
	GetComponents(USceneComponent::StaticClass(), SceneComponents, true);
	
	TArray<USceneComponent*> SelfSnapComponents;
	for (UActorComponent* SceneComponent : SceneComponents)
	{
		if (USceneComponent* SnapComponent = Cast<USceneComponent>(SceneComponent))
		{
			if (SnapComponent->ComponentHasTag("Snap"))
				SelfSnapComponents.Add(SnapComponent);
		}
	}

	if (SelfSnapComponents.Num() == 0)
		return;

	// closest snap delta distance
	FVector ClosestSnapDelta = FVector(FLT_MAX);

	for (USceneComponent* SelfSnapComponent : SelfSnapComponents)
	{
		for (USceneComponent* OtherSnapComponent : OtherSnapComponents)
		{
			FVector CurDelta = OtherSnapComponent->GetComponentLocation() - SelfSnapComponent->GetComponentLocation();

			if (CurDelta.Size() < ClosestSnapDelta.Size())
				ClosestSnapDelta = CurDelta;
		}
	}

	AddActorWorldOffset(ClosestSnapDelta);

	SetEditMaterial(CheckLocation());
}

