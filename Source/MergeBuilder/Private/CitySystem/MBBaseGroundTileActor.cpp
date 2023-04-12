// Fill out your copyright notice in the Description page of Project Settings.


#include "CitySystem/MBBaseGroundTileActor.h"

// Sets default values
AMBBaseGroundTileActor::AMBBaseGroundTileActor()
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
void AMBBaseGroundTileActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMBBaseGroundTileActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

