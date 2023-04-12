// Fill out your copyright notice in the Description page of Project Settings.


#include "CitySystem/MBGroundFieldManager.h"

// Sets default values
AMBGroundFieldManager::AMBGroundFieldManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMBGroundFieldManager::BeginPlay()
{
	Super::BeginPlay();

	InitializeGround();
}

void AMBGroundFieldManager::InitializeGround()
{
	auto GroundFieldSubsystem = GetGameInstance()->GetSubsystem<UMBGroundSubsystem>();
	for (int32 i = 0; i < GroundFieldSubsystem->GetFieldSize().Y; ++i)
	{
		for (int32 j = 0; j < GroundFieldSubsystem->GetFieldSize().X; ++j)
		{
			FMBGroundTile Tile;
			if (!GroundFieldSubsystem->GetGroundTile(FIntPoint(j, i), Tile))
				continue;

			FTransform Transform = FTransform::Identity;

			FVector Location;
			GetTileLocationForIndex(Tile.Index, Location);
			Transform.SetLocation(Location);

			TArray<FMBGroundTile> Neighbors;
			GroundFieldSubsystem->GetNeighborTilesForIndex(Tile.Index, Neighbors);

			EGroundTileType Type = GetGroundTileTypeByNeighbors(Tile.Index, Neighbors);
			FRotator Rotation;
			GetTileRotation(Tile.Index, Neighbors, Type, Rotation);
			Transform.SetRotation(Rotation.Quaternion());
			
			auto GroundTileActor = GetWorld()->SpawnActor<AMBBaseGroundTileActor>(GroundTileClass, Transform);

			GroundTileActor->InitMeshByType(Type);
		}
	}
}

void AMBGroundFieldManager::GetTileLocationForIndex(const FIntPoint& Index, FVector& Location)
{
	auto GroundFieldSubsystem = GetGameInstance()->GetSubsystem<UMBGroundSubsystem>();
	
	Location.Z = 100.0f;

	Location.X = (Index.X - GroundFieldSubsystem->GetFieldSize().X / 2.0f) * 3000.0f;
	Location.Y = (Index.Y - GroundFieldSubsystem->GetFieldSize().Y / 2.0f) * 3000.0f;
}

void AMBGroundFieldManager::GetTileRotation(const FIntPoint& Index, const TArray<FMBGroundTile>& Neighbors,
	EGroundTileType Type, FRotator& Rotation)
{
	Rotation = FRotator::ZeroRotator;
}

EGroundTileType AMBGroundFieldManager::GetGroundTileTypeByNeighbors(const FIntPoint& Index, const TArray<FMBGroundTile>& Neighbors)
{
	return EGroundTileType::BaseSquare;
}

// Called every frame
void AMBGroundFieldManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

