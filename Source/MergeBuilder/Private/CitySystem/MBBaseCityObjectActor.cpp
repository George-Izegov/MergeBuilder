// Fill out your copyright notice in the Description page of Project Settings.


#include "CitySystem/MBBaseCityObjectActor.h"

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

