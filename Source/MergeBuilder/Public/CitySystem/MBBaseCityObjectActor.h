// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CityBuilderSubsystem.h"
#include "Components/StaticMeshComponent.h"
#include "MBBaseCityObjectActor.generated.h"

UCLASS()
class MERGEBUILDER_API AMBBaseCityObjectActor : public AActor
{
	GENERATED_BODY()

		friend class AMBCityBuilderManager;
	
public:	
	// Sets default values for this actor's properties
	AMBBaseCityObjectActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Initialize(const FCityObject& ObjectStruct, const FCityObjectData*& ObjectTableData);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	bool CheckLocation();

	UFUNCTION(BlueprintImplementableEvent)
	void SetEditMaterial(bool IsAcceptable);

	UFUNCTION(BlueprintImplementableEvent)
	void SetDefaultMaterial();

protected:

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		USceneComponent* Root;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		UStaticMeshComponent* BaseMesh;

	UPROPERTY(BlueprintReadOnly)
		FCityObject CityObjectData;
};
