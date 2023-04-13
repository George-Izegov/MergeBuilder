// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CitySystem/MBBaseGroundTileActor.h"
#include "CitySystem/MBGroundSubsystem.h"
#include "CitySystem/MBPossibleGroundActor.h"
#include "MBGroundFieldManager.generated.h"

UCLASS()
class MERGEBUILDER_API AMBGroundFieldManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMBGroundFieldManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void InitializeGround();

	void GetTileLocationForIndex(const FIntPoint& Index, FVector& Location);

	void GetTileRotation(const FIntPoint& Index, const TArray<FMBGroundTile>& Neighbors, EGroundTileType Type, FRotator& Rotation);

	EGroundTileType GetGroundTileTypeByNeighbors(const FIntPoint& Index, const TArray<FMBGroundTile>& Neighbors);

	UFUNCTION(BlueprintCallable)
	void SpawnAllPossibleGroundTiles();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AMBBaseGroundTileActor> GroundTileClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AMBPossibleGroundActor> PossibleGroundTileClass;
};
