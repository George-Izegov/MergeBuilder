// Fill out your copyright notice in the Description page of Project Settings.


#include "CitySystem/MBPossibleGroundActor.h"

// Sets default values
AMBPossibleGroundActor::AMBPossibleGroundActor()
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
void AMBPossibleGroundActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMBPossibleGroundActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMBPossibleGroundActor::Init(const FIntPoint& Index)
{
	TileIndex = Index;
}

