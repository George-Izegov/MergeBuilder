// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MBBaseMergeItemActor.h"
#include "MBCoreTypes.h"
#include "MBMergeFieldManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemActorAction, AMBBaseMergeItemActor*, ItemActor);
UCLASS()
class MERGEBUILDER_API AMBMergeFieldManager : public AActor
{
	GENERATED_BODY()

	friend class AMBMergeFieldPawn;
	
public:	
	// Sets default values for this actor's properties
	AMBMergeFieldManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void DestroyAllItems();

	AMBBaseMergeItemActor* SpawnItemAtIndex(const FMergeFieldItem& Item, const FIntPoint& Index);

	void InitializeField();

	void HandleStartTouchOnIndex(const FIntPoint& Index);
	void HandleReleaseTouchOnIndex(const FIntPoint& Index);
	void HandleDrag(const FVector& FieldLocation);
	void HandleClickOnIndex(const FIntPoint& Index);
	void HandleClickOnReward();

	void StartDrag();

	void PlaceItemAtIndex(const FIntPoint& Index, AMBBaseMergeItemActor* Item);

	void SelectIndex(const FIntPoint& Index);
	void DeselectCurrentIndex();

	void InitRewardItem();

	void OpenInBoxItems(const TArray<FIntPoint>& Indexes);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void DestroyItem(const FIntPoint& Index);

	UFUNCTION(BlueprintCallable)
	bool GetLocationForIndex(const FIntPoint& Index, FVector& OutLocation);

	UFUNCTION(BlueprintCallable)
	bool GetIndexForLocation(const FVector& Location, FIntPoint& OutIndex);

	AMBBaseMergeItemActor* GetItemAtIndex(const FIntPoint& Index);

	bool GenerateNewItemFromAnother(AMBBaseMergeItemActor* SourceItem);

	AMBBaseMergeItemActor* GenerateNewItemFromLocation(const FIntPoint& SourceIndex, const FMergeFieldItem& NewItem);
	AMBBaseMergeItemActor* GenerateNewItemFromLocation(const FIntPoint& SourceIndex, const FVector& SourceLocation, const FMergeFieldItem& NewItem);

protected:

	TArray<TArray<AMBBaseMergeItemActor*>> FieldItems;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UDataTable* MergeItemsDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TSubclassOf<AMBBaseMergeItemActor> SpawnItemsClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TSubclassOf<AActor> SelectionActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		AMBBaseMergeItemActor* RewardActor;

	AMBBaseMergeItemActor* TouchStartItem;

	FIntPoint SelectedIndex;

	UPROPERTY(BlueprintReadOnly)
		AActor* SelectionActor;

	UPROPERTY(EditAnywhere)
	float TileSize = 64.0f;
	UPROPERTY(EditAnywhere)
	float LocationFieldZ = 2.0f;

	bool InDrag = false;

public:

	UPROPERTY(BlueprintAssignable)
	FOnItemActorAction OnItemSelected;
	UPROPERTY(BlueprintAssignable)
	FNoParamsSignatureDyn OnItemDeselected;

	UPROPERTY(BlueprintAssignable)
	FOnItemActorAction OnItemUpdated;
};
