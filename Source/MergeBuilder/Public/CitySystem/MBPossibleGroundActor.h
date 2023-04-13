// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MBPossibleGroundActor.generated.h"

UCLASS()
class MERGEBUILDER_API AMBPossibleGroundActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMBPossibleGroundActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Init(const FIntPoint& Index);

protected:

	UPROPERTY(BlueprintReadOnly)
	FIntPoint TileIndex;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	USceneComponent* Root;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UStaticMeshComponent* BaseMesh;
};
