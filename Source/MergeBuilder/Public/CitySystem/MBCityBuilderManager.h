// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MBCityBuilderManager.generated.h"

UCLASS()
class MERGEBUILDER_API AMBCityBuilderManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMBCityBuilderManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void InitializeCity();

	UFUNCTION(BlueprintCallable)
	void SpawnNewObject(const FName& ObjectName);

	void GetInitialSpawnLocation(FVector& Location);

	UFUNCTION(BlueprintCallable)
	void SetEditedObject(AMBBaseCityObjectActor* Object);
	UFUNCTION(BlueprintCallable)
	void AcceptEditObject();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	const AMBBaseCityObjectActor* GetEditedObject();

	void MoveEditedObject(const FVector& DeltaLocation);
	UFUNCTION(BlueprintCallable)
	void RotateEditedObject(int32 Direction);

protected:

	UPROPERTY(BlueprintReadOnly)
	AMBBaseCityObjectActor* EditedObject;

};
