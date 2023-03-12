// Fill out your copyright notice in the Description page of Project Settings.


#include "CitySystem/MBCityBuilderManager.h"
#include "CitySystem/CityBuilderSubsystem.h"
#include "CitySystem/MBBaseCityObjectActor.h"
#include "TopDownPawn.h"
#include "Kismet/GameplayStatics.h"

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

void AMBCityBuilderManager::SpawnNewObject(const FName& ObjectName)
{
	auto CityBuilderSubsystem = GetGameInstance()->GetSubsystem<UCityBuilderSubsystem>();

	const FCityObjectData* RowStruct = CityBuilderSubsystem->CityObjectsDataTable->FindRow<FCityObjectData>(ObjectName, "AMBCityBuilderManager::SpawnNewObject()");

	if (!RowStruct)
	{
		UE_LOG(LogTemp, Warning, TEXT("AMBCityBuilderManager::SpawnNewObject() - Failed to find row with name %s"), *ObjectName.ToString());
		return;
	}

	UClass* ObjectActorClass = RowStruct->ObjectClass.LoadSynchronous();

	FVector SpawnLocation;
	GetInitialSpawnLocation(SpawnLocation);

	FTransform SpawnTransform = FTransform::Identity;
	SpawnTransform.SetLocation(SpawnLocation);

	auto SpawnedObject = GetWorld()->SpawnActor<AMBBaseCityObjectActor>(ObjectActorClass, SpawnTransform);
	FCityObject ObjectStruct;
	ObjectStruct.ObjectName = ObjectName;
	ObjectStruct.Location = SpawnLocation;

	SpawnedObject->Initialize(ObjectStruct, RowStruct);

	SetEditedObject(SpawnedObject);
}

void AMBCityBuilderManager::GetInitialSpawnLocation(FVector& Location)
{
	APlayerController* PC = GetGameInstance()->GetFirstLocalPlayerController();
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	// Input Object type
	ObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery7);

	FVector2D ScreenCenter = FVector2D(GSystemResolution.ResX / 2.0f, (GSystemResolution.ResY / 2.0f - GSystemResolution.ResY*0.15f));
	FHitResult HitResult;
	if (PC->GetHitResultAtScreenPosition(ScreenCenter, ObjectTypes, false, HitResult))
	{
		Location = HitResult.Location;
	}
	else
	{
		Location = PC->GetPawn()->GetActorLocation();
		UE_LOG(LogTemp, Warning, TEXT("AMBCityBuilderManager::GetInitialSpawnLocation - no hit result"));
	}

	Location.Z = 100.0f;
}

void AMBCityBuilderManager::SetEditedObject(AMBBaseCityObjectActor* Object)
{
	bool IsAcceptable = Object->CheckLocation();
	Object->SetEditMaterial(IsAcceptable);

	EditedObject = Object;
}

void AMBCityBuilderManager::AcceptEditObject()
{
	auto CityBuilderSubsystem = GetGameInstance()->GetSubsystem<UCityBuilderSubsystem>();

	EditedObject->CityObjectData.Location = EditedObject->GetActorLocation();
	EditedObject->CityObjectData.Rotation = EditedObject->GetActorRotation().Yaw;
	
	if (EditedObject->CityObjectData.ObjectID == -1)
	{
		CityBuilderSubsystem->AddNewObject(EditedObject->CityObjectData);
	}
	else
	{
		CityBuilderSubsystem->EditObject(EditedObject->CityObjectData);
	}

	EditedObject->SetDefaultMaterial();
	EditedObject = nullptr;
}

void AMBCityBuilderManager::RemoveCityObject(AMBBaseCityObjectActor* ObjectToRemove)
{
	auto CityBuilderSubsystem = GetGameInstance()->GetSubsystem<UCityBuilderSubsystem>();
	CityBuilderSubsystem->RemoveObject(ObjectToRemove->CityObjectData);

	ObjectToRemove->Destroy();
}

// Called every frame
void AMBCityBuilderManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

const AMBBaseCityObjectActor* AMBCityBuilderManager::GetEditedObject()
{
	return EditedObject;
}

void AMBCityBuilderManager::MoveEditedObject(const FVector& DeltaLocation)
{
	check(EditedObject);

	EditedObject->AddActorWorldOffset(FVector(DeltaLocation.X, DeltaLocation.Y, 0.0f));

	bool IsAcceptable = EditedObject->CheckLocation();
	EditedObject->SetEditMaterial(IsAcceptable);
}

void AMBCityBuilderManager::RotateEditedObject(int32 Direction)
{
	check(EditedObject);

	FRotator DeltaRotation = FRotator::ZeroRotator;
	DeltaRotation.Yaw = Direction * 30.0f;
	EditedObject->AddActorWorldRotation(DeltaRotation);

	bool IsAcceptable = EditedObject->CheckLocation();
	EditedObject->SetEditMaterial(IsAcceptable);
}

void AMBCityBuilderManager::CancelEditionObject()
{
	check(EditedObject);

	if (EditedObject->CityObjectData.ObjectID == -1)
	{
		EditedObject->Destroy();
	}
	else
	{
		FTransform PreEditedTransform;
		PreEditedTransform.SetLocation(EditedObject->CityObjectData.Location);
		FRotator Rotation = FRotator::ZeroRotator;
		Rotation.Yaw = EditedObject->CityObjectData.Rotation;
		PreEditedTransform.SetRotation(Rotation.Quaternion());
		PreEditedTransform.SetScale3D(FVector(EditedObject->CityObjectData.Scale));

		EditedObject->SetActorTransform(PreEditedTransform);
		EditedObject->SetDefaultMaterial();
	}

	EditedObject = nullptr;
}

void AMBCityBuilderManager::HandleObjectClick(AMBBaseCityObjectActor* CityObject)
{
	OnObjectClicked.Broadcast(CityObject);
}