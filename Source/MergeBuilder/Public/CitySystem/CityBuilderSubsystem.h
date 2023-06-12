// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CityObjectsData.h"
#include "QuestSystem/MBQuest.h"
#include "CityBuilderSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdateObjects, TArray<int32>, ObjectIDs);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBuildNewObject, FName, ObjectName);
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

	void GetCityObjectsByType(ECityObjectCategory Type, TArray<int32>& OutObjectIDs);

	void AddNewObject(FCityObject& NewObject);
	void EditObject(const FCityObject& EditedObject);
	void RemoveObject(const FCityObject& ObjectToRemove);

	void CollectFromObject(FCityObject& Object);

	UFUNCTION(BlueprintCallable)
	bool CheckRequierementsForBuildObject(const FName& ObjectName);

	void SpendResourcesForBuildObject(const FName& ObjectName);

	void SaveCity();

	void CalculateCurrentPopulationAndRatings();
	
	UFUNCTION(BlueprintCallable)
	void GetTopRatingsForLevel(int32 Level, FCityRatings& TopRatings);

	UFUNCTION(BlueprintCallable)
	float GetAverageRating();

	UFUNCTION(BlueprintCallable)
	void GetObjectsChain(const FName& ObjectName, TArray<FCityObjectData>& OutObjects, int32& CurrentIndex);

	bool HasGenerator(const FName& ObjectName);

	void SetNewQuestsForObjects(TArray<FString> NewQuests);

	UFUNCTION(BlueprintPure)
	bool GetCityObjectByID(int32 ObjectID, FCityObject& OutObject);

	UFUNCTION(BlueprintCallable)
	void SkipTimerForObject(int32 ObjectID);

	void HandleSuccessWatchVideoForObject(int32 ObjectID);
	
protected:

	void AddExperienceForNewObject(const FName& NewObjectName);

	void ParseCity(const FString& JsonString);

	void InitCity();

	void CreateConsoleVariables();

	UPROPERTY()
	TArray<FCityObject> CityObjects;

	UPROPERTY(BlueprintReadOnly)
	int32 Population = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 EmployedPopulation = 0;

	UPROPERTY(BlueprintReadOnly)
	FCityRatings CityRating;

	UPROPERTY(BlueprintReadOnly)
	int32 SkipTimerPrice = 15;

	UPROPERTY(BlueprintReadOnly)
	int32 AdSkipTimeSeconds = 45;

	UPROPERTY(BlueprintReadOnly)
	int32 TimerBaseDurationInHours = 1;

public:

	UPROPERTY()
	UDataTable* CityObjectsDataTable;

	UPROPERTY(BlueprintAssignable)
	FOnUpdateObjects OnQuestsUpdated;

	UPROPERTY(BlueprintAssignable)
	FOnBuildNewObject OnBuildNewObject;
};
