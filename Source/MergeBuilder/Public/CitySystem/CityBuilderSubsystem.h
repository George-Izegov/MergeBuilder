// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DataTable.h"
#include "MergeSubsystem.h"
#include "CityBuilderSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FGeneratorSettings
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FMergeFieldItem GeneratedBox;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		int32 MinutesToRestore;
};

USTRUCT(BlueprintType)
struct FCityObject
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FName ObjectName = NAME_None;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FVector Location = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		float Rotation = 0.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		float Scale = 1.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FDateTime RestoreTime;

	int32 ObjectID = -1;
};

UENUM(BlueprintType)
enum class ECityObjectCategory : uint8
{
	Other,
	Buildings,
	Plants,
	Infrastructure
};

USTRUCT(BlueprintType)
struct FCityObjectData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		TSoftClassPtr<class AMBBaseCityObjectActor> ObjectClass;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FName NextLevelObjectName;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		ECityObjectCategory Category;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FText LocalizedName;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<FRequiredItem> RequiredItems;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 CostInCoins;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	EConsumableParamType CoinsType = EConsumableParamType::SoftCoin;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		bool IsGenerator = false;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FGeneratorSettings GeneratorSettings;
};
/**
 * 
 */
UCLASS()
class MERGEBUILDER_API UCityBuilderSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	UCityBuilderSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	void GetCityObjects(TArray<FCityObject>& OutObjects) { OutObjects = CityObjects; }

	void AddNewObject(FCityObject& NewObject);
	void EditObject(const FCityObject& EditedObject);
	void RemoveObject(const FCityObject& ObjectToRemove);

	void CollectFromObject(FCityObject& Object);

	UFUNCTION(BlueprintCallable)
	bool CheckRequierementsForBuildObject(const FName& ObjectName);

	void SpendResourcesForBuildObject(const FName& ObjectName);

	void SaveCity();
	
protected:

	void ParseCity(const FString& JsonString);

	void InitCity();

	UPROPERTY()
	TArray<FCityObject> CityObjects;

public:

	UPROPERTY()
	UDataTable* CityObjectsDataTable;
};
