// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MBBaseGroundTileActor.generated.h"

UENUM(BlueprintType)
enum class EGroundTileType : uint8
{
	BaseSquare,
	OneSided,
	TwoSidedCorner,
	TwoSidedEdge,
	ThreeSided
};

UCLASS()
class MERGEBUILDER_API AMBBaseGroundTileActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMBBaseGroundTileActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent)
	void InitMeshByType(EGroundTileType Type);

protected:

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	USceneComponent* Root;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UStaticMeshComponent* BaseMesh;

};
