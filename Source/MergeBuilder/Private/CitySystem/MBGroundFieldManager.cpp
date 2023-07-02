// Fill out your copyright notice in the Description page of Project Settings.


#include "CitySystem/MBGroundFieldManager.h"

#include "Analytics/FGAnalytics.h"
#include "Kismet/GameplayStatics.h"
#include "User/AccountSubsystem.h"

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

			SpawnGroundTile(Tile);
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

	switch (Type)
	{
	case EGroundTileType::BaseSquare:
		break;
	case EGroundTileType::OneSided:
		{
			FIntPoint DeltaSum = FIntPoint(0,0);
			for (auto& Neighbor : Neighbors)
			{
				DeltaSum += Neighbor.Index - Index;
			}

			if (DeltaSum == FIntPoint(-1, 0))
			{
				Rotation.Yaw = 0.0f;
			}
			else if (DeltaSum == FIntPoint(0, -1))
			{
				Rotation.Yaw = 90.0f;
			}
			else if (DeltaSum == FIntPoint(1, 0))
			{
				Rotation.Yaw = 180.0f;
			}
			else if (DeltaSum == FIntPoint(0, 1))
			{
				Rotation.Yaw = 270.0f;
			}
			break;
		}
	case EGroundTileType::TwoSidedCorner:
		{
			FIntPoint NeighborDelta1 = Neighbors[0].Index - Index;
			FIntPoint NeighborDelta2 = Neighbors[1].Index - Index;

			FIntPoint DeltaSum = NeighborDelta1 + NeighborDelta2;

			if (DeltaSum == FIntPoint(1,1))
			{
				Rotation.Yaw = 270.0f;
			}
			else if (DeltaSum == FIntPoint(-1,1))
			{
				Rotation.Yaw = 0.0f;
			}
			else if(DeltaSum == FIntPoint(-1, -1))
			{
				Rotation.Yaw = 90.0f;
			}
			else if (DeltaSum == FIntPoint(1, -1))
			{
				Rotation.Yaw = 180.0f;
			}
			break;
		}
	case EGroundTileType::TwoSidedEdge:
		{
			FIntPoint NeighborDelta1 = Neighbors[0].Index - Index;

			if (NeighborDelta1.X == 0)
			{
				Rotation.Yaw = 0.0f;
			}
			else
			{
				Rotation.Yaw = 90.0f;
			}
			break;
		}
	case EGroundTileType::ThreeSided:
		{
			FIntPoint NeighborDelta = Neighbors[0].Index - Index;

			if (NeighborDelta == FIntPoint(0,1))
			{
				Rotation.Yaw = 0.0f;
			}
			else if (NeighborDelta == FIntPoint(1,0))
			{
				Rotation.Yaw = 270.0f;
			}
			else if(NeighborDelta == FIntPoint(0, -1))
			{
				Rotation.Yaw = 180.0f;
			}
			else if (NeighborDelta == FIntPoint(-1, 0))
			{
				Rotation.Yaw = 90.0f;
			}
			break;
		}
	}
}

EGroundTileType AMBGroundFieldManager::GetGroundTileTypeByNeighbors(const FIntPoint& Index, const TArray<FMBGroundTile>& Neighbors)
{
	switch (Neighbors.Num())
	{
	case 1:
		return EGroundTileType::ThreeSided;
	case 2:
		{
			if (Neighbors[0].Index.X == Neighbors[1].Index.X || Neighbors[0].Index.Y == Neighbors[1].Index.Y)
				return EGroundTileType::TwoSidedEdge;
			
			return EGroundTileType::TwoSidedCorner;
		}
	case 3:
		return EGroundTileType::OneSided;
	case 4:
		return EGroundTileType::BaseSquare;
	}
	
	return EGroundTileType::BaseSquare;
}

void AMBGroundFieldManager::SpawnAllPossibleGroundTiles()
{
	auto GroundFieldSubsystem = GetGameInstance()->GetSubsystem<UMBGroundSubsystem>();

	TArray<FMBGroundTile> AllPossibleGroundTiles;
	GroundFieldSubsystem->GetAllPossibleGroundTiles(AllPossibleGroundTiles);

	for (auto& GroundTile : AllPossibleGroundTiles)
	{
		FTransform Transform = FTransform::Identity;
		FVector Location;
		GetTileLocationForIndex(GroundTile.Index, Location);
		Transform.SetLocation(Location);

		auto PossibleTileActor = GetWorld()->SpawnActor<AMBPossibleGroundActor>(PossibleGroundTileClass, Transform);
		
		PossibleTileActor->Init(GroundTile.Index);
	}
}

void AMBGroundFieldManager::RemoveAllPossibleGroundTiles()
{
	TArray<AActor*> PossibleGroundTiles;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMBPossibleGroundActor::StaticClass(), PossibleGroundTiles);

	for (auto& PossibleTile : PossibleGroundTiles)
	{
		PossibleTile->Destroy();
	}
}

AMBBaseGroundTileActor* AMBGroundFieldManager::SpawnGroundTile(const FMBGroundTile& GroundTile)
{
	auto GroundFieldSubsystem = GetGameInstance()->GetSubsystem<UMBGroundSubsystem>();
	
	FTransform Transform = FTransform::Identity;

	FVector Location;
	GetTileLocationForIndex(GroundTile.Index, Location);
	Transform.SetLocation(Location);

	TArray<FMBGroundTile> Neighbors;
	GroundFieldSubsystem->GetNeighborTilesForIndex(GroundTile.Index, Neighbors);

	EGroundTileType Type = GetGroundTileTypeByNeighbors(GroundTile.Index, Neighbors);
	FRotator Rotation;
	GetTileRotation(GroundTile.Index, Neighbors, Type, Rotation);
	Transform.SetRotation(Rotation.Quaternion());
			
	auto GroundTileActor = GetWorld()->SpawnActor<AMBBaseGroundTileActor>(GroundTileClass, Transform);

	GroundTileActor->InitMeshByType(Type);
	GroundTileActor->SetIndex(GroundTile.Index);

	return GroundTileActor;
}

AMBBaseGroundTileActor* AMBGroundFieldManager::GetTileActorByIndex(const FIntPoint& Index)
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMBBaseGroundTileActor::StaticClass(), Actors);

	for (auto& Actor : Actors)
	{
		auto Tile = Cast<AMBBaseGroundTileActor>(Actor);

		if (Tile && Tile->GetIndex() == Index)
		{
			return Tile;
		}
	}

	return nullptr;
}

void AMBGroundFieldManager::BuyGroundTile(const FIntPoint& Index)
{
	auto GroundFieldSubsystem = GetGameInstance()->GetSubsystem<UMBGroundSubsystem>();
	auto AccountSubsystem = GetGameInstance()->GetSubsystem<UAccountSubsystem>();
	
	FMBPossibleGroundTileInfo TileInfo;
	GroundFieldSubsystem->GetGroundTileInfo(Index, TileInfo);

	switch (TileInfo.CoinsType)
	{
	case EConsumableParamType::SoftCoin:
		AccountSubsystem->SpendSoftCoins(TileInfo.CostInCoins);
		break;
	case EConsumableParamType::PremCoin:
		AccountSubsystem->SpendPremCoins(TileInfo.CostInCoins);
		break;
	}

	GroundFieldSubsystem->AddNewTile(Index);
	
	RemoveAllPossibleGroundTiles();

	FMBGroundTile GroundTile;
	GroundTile.Index = Index;
	
	SpawnGroundTile(GroundTile);

	TArray<FMBGroundTile> Neighbors;
	GroundFieldSubsystem->GetNeighborTilesForIndex(Index, Neighbors);

	for (auto& Neighbor : Neighbors)
	{
		auto TileActor = GetTileActorByIndex(Neighbor.Index);

		TArray<FMBGroundTile> TileNeighbors;
		GroundFieldSubsystem->GetNeighborTilesForIndex(Neighbor.Index, TileNeighbors);

		EGroundTileType Type = GetGroundTileTypeByNeighbors(Neighbor.Index, TileNeighbors);
		FRotator Rotation;
		GetTileRotation(Neighbor.Index, TileNeighbors, Type, Rotation);

		TileActor->SetActorRotation(Rotation);
		TileActor->InitMeshByType(Type);
	}

	SpawnAllPossibleGroundTiles();

	UFGAnalytics::LogEvent("buy_ground_tile");
}

// Called every frame
void AMBGroundFieldManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

