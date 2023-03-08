// Fill out your copyright notice in the Description page of Project Settings.


#include "CitySystem/MBCityBuilderManager.h"
#include "CitySystem/CityBuilderSubsystem.h"
#include "CitySystem/MBBaseCityObjectActor.h"

// Sets default values
AMBCityBuilderManager::AMBCityBuilderManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMBCityBuilderManager::BeginPlay()
{
	Super::BeginPlay();
	
	InitializeCity();
}

void AMBCityBuilderManager::InitializeCity()
{
	auto CityBuilderSubsystem = GetGameInstance()->GetSubsystem<UCityBuilderSubsystem>();

	TArray<FCityObject> CityObjects;
	CityBuilderSubsystem->GetCityObjects(CityObjects);

	for (const auto& Object : CityObjects)
	{
		const FCityObjectData* RowStruct = CityBuilderSubsystem->CityObjectsDataTable->FindRow<FCityObjectData>(Object.ObjectName, "AMBCityBuilderManager::InitializeCity()");

		if (!RowStruct)
		{
			UE_LOG(LogTemp, Warning, TEXT("AMBCityBuilderManager::InitializeCity() - Failed to find row with name %s"), *Object.ObjectName.ToString());
			continue;
		}

		UClass* ObjectActorClass = RowStruct->ObjectClass.LoadSynchronous();

		FTransform SpawnTransform = FTransform::Identity;
		SpawnTransform.SetLocation(Object.Location);
		FRotator Rotation = FRotator::ZeroRotator;
		Rotation.Yaw = Object.Rotation;
		SpawnTransform.SetRotation(Rotation.Quaternion());
		SpawnTransform.SetScale3D(FVector(Object.Scale));

		auto SpawnedObject = GetWorld()->SpawnActor<AMBBaseCityObjectActor>(ObjectActorClass, SpawnTransform);

		SpawnedObject->Initialize(Object, RowStruct);
	}
}

// Called every frame
void AMBCityBuilderManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

