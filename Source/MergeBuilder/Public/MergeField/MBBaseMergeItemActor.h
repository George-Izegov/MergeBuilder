// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PaperSpriteComponent.h"
#include "MergeSubsystem.h"
#include "MBBaseMergeItemActor.generated.h"

UCLASS()
class MERGEBUILDER_API AMBBaseMergeItemActor : public AActor
{
	GENERATED_BODY()

	friend class AMBMergeFieldManager;

public:	
	// Sets default values for this actor's properties
	AMBBaseMergeItemActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
	void InitVisual();

	UFUNCTION(BlueprintImplementableEvent)
	void MoveToLocation(const FVector& Location);

	void HandleInteraction();

	void GenerateNewItem();
	void AddConsumableValue();

	UFUNCTION(BlueprintImplementableEvent)
		void PlayAppearingAnimation();

public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Initialize(const FMergeFieldItem& InBaseData, const FMergeItemData& InTableData, const FIntPoint& Index);

	FIntPoint GetFieldIndex() { return FieldIndex; }
	FMergeFieldItem GetBaseData() { return BaseData; }

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USceneComponent* Root;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UPaperSpriteComponent* Sprite;

	UPROPERTY(BlueprintReadOnly)
	FMergeFieldItem BaseData;

	UPROPERTY(BlueprintReadOnly)
	FMergeItemData TableData;

	FIntPoint FieldIndex;
};
