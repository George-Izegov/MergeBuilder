// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DataTable.h"
#include "CityBuilderSubsystem.generated.h"


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

};
USTRUCT(BlueprintType)
struct FCityObjectData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		TSoftClassPtr<class AMBBaseCityObjectActor> ObjectClass;
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
	
protected:

	void ParseCity(const FString& JsonString);

	void SaveCity();

	UPROPERTY()
	UDataTable* CityObjectsDataTable;
};
