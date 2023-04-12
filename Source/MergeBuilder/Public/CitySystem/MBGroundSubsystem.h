// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MBGroundSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FMBGroundTile
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool IsVoid = true;
	
	FIntPoint Index;

	bool operator ==(const FMBGroundTile& Other) const
	{
		return Index == Other.Index;
	}
	
};
/**
 * 
 */
UCLASS()
class MERGEBUILDER_API UMBGroundSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	UMBGroundSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	void SaveGround();

	// return void tiles near not void
	void GetAllPossibleGroundTiles(TArray<FMBGroundTile>& OutGroundTiles);

	// return not void tiles near index
	void GetNeighborTilesForIndex(const FIntPoint& Index, TArray<FMBGroundTile>& OutGroundTiles);

	void AddNewTile(const FIntPoint& Index);

	FIntPoint GetFieldSize() const { return GroundFieldSize; }

	bool GetGroundTile(const FIntPoint& Index, FMBGroundTile& OutGroundTile);

protected:

	void InitGroundField();

	void InitStartGroundField();
	
	void ParseGround(const FString& JsonString);

	TArray<TArray<FMBGroundTile>> GroundField;

	// must be even
	FIntPoint GroundFieldSize;
};
