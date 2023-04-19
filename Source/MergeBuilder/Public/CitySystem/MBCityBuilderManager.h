// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MBCityBuilderManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectClicked, AMBBaseCityObjectActor*, ClickedObject);

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
	AMBBaseCityObjectActor* SpawnNewObject(const FName& ObjectName);

	void GetInitialSpawnLocation(FVector& Location);

	UFUNCTION(BlueprintCallable)
	void SetEditedObject(AMBBaseCityObjectActor* Object);
	UFUNCTION(BlueprintCallable)
	void AcceptEditObject();
	
	UFUNCTION(BlueprintCallable)
	void RemoveCityObject(AMBBaseCityObjectActor* ObjectToRemove);

	UFUNCTION(BlueprintCallable)
		void CollectRewardFromCityObject(AMBBaseCityObjectActor* CityObject);

	void MergeObjects(AMBBaseCityObjectActor* Object1, AMBBaseCityObjectActor* Object2);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	const AMBBaseCityObjectActor* GetEditedObject();

	void HandleDragRelease();
	void MoveEditedObject(const FVector& DeltaLocation);
	UFUNCTION(BlueprintCallable)
	void RotateEditedObject(int32 Direction);

	UFUNCTION(BlueprintCallable)
	void CancelEditionObject();

	void HandleObjectClick(AMBBaseCityObjectActor* CityObject);

protected:

	UPROPERTY(BlueprintReadOnly)
	AMBBaseCityObjectActor* EditedObject;

	UPROPERTY(BlueprintAssignable)
	FOnObjectClicked OnObjectClicked;

	AMBBaseCityObjectActor* MergedObject1 = nullptr;
	AMBBaseCityObjectActor* MergedObject2 = nullptr;
};
