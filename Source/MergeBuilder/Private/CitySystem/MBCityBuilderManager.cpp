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

	auto CityBuilderSubsystem = GetGameInstance()->GetSubsystem<UCityBuilderSubsystem>();

	CityBuilderSubsystem->OnQuestsUpdated.AddDynamic(this, &AMBCityBuilderManager::UpdateQuestsForObjects);
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

AMBBaseCityObjectActor* AMBCityBuilderManager::SpawnNewObject(const FName& ObjectName)
{
	auto CityBuilderSubsystem = GetGameInstance()->GetSubsystem<UCityBuilderSubsystem>();

	const FCityObjectData* RowStruct = CityBuilderSubsystem->CityObjectsDataTable->FindRow<FCityObjectData>(ObjectName, "AMBCityBuilderManager::SpawnNewObject()");

	if (!RowStruct)
	{
		UE_LOG(LogTemp, Warning, TEXT("AMBCityBuilderManager::SpawnNewObject() - Failed to find row with name %s"), *ObjectName.ToString());
		return nullptr;
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

	return SpawnedObject;
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

	Location = FVector(FMath::TruncToInt(Location.X / BuildGrid) * BuildGrid, FMath::TruncToInt(Location.Y / BuildGrid) * BuildGrid, 100.0f);
}

void AMBCityBuilderManager::SetEditedObject(AMBBaseCityObjectActor* Object)
{
	ECityObjectLocationState State = Object->CheckLocation();
	Object->SetEditMaterial(State);

	EditedObject = Object;
}

void AMBCityBuilderManager::AcceptEditObject()
{
	auto CityBuilderSubsystem = GetGameInstance()->GetSubsystem<UCityBuilderSubsystem>();

	EditedObject->CityObjectData.Location = EditedObject->GetActorLocation();
	EditedObject->CityObjectData.Rotation = EditedObject->GetActorRotation().Yaw;
	
	if (EditedObject->CityObjectData.ObjectID == -1)
	{
		CityBuilderSubsystem->SpendResourcesForBuildObject(EditedObject->CityObjectData.ObjectName);
		CityBuilderSubsystem->AddNewObject(EditedObject->CityObjectData);
	}
	else
	{
		CityBuilderSubsystem->EditObject(EditedObject->CityObjectData);
	}

	if (MergedObject1)
	{
		RemoveCityObject(MergedObject1);
		MergedObject1 = nullptr;
	}
	
	if (MergedObject2)
	{
		RemoveCityObject(MergedObject2);
		MergedObject2 = nullptr;
	}

	EditedObject->Deselect();
	EditedObject = nullptr;
}

void AMBCityBuilderManager::RemoveCityObject(AMBBaseCityObjectActor* ObjectToRemove)
{
	auto CityBuilderSubsystem = GetGameInstance()->GetSubsystem<UCityBuilderSubsystem>();
	CityBuilderSubsystem->RemoveObject(ObjectToRemove->CityObjectData);

	ObjectToRemove->Destroy();
}

void AMBCityBuilderManager::CollectRewardFromCityObject(AMBBaseCityObjectActor* CityObject)
{
	auto CityBuilderSubsystem = GetGameInstance()->GetSubsystem<UCityBuilderSubsystem>();
	CityBuilderSubsystem->CollectFromObject(CityObject->CityObjectData);
}

void AMBCityBuilderManager::MergeObjects(AMBBaseCityObjectActor* Object1, AMBBaseCityObjectActor* Object2)
{
	check(Object1);
	check(Object2);
	
	auto CityBuilderSubsystem = GetGameInstance()->GetSubsystem<UCityBuilderSubsystem>();
	const FCityObjectData* RowStruct = CityBuilderSubsystem->CityObjectsDataTable->FindRow<FCityObjectData>(Object1->CityObjectData.ObjectName, "");

	FName NextLevelObjectName = RowStruct->NextLevelObjectName;

	Object1->SetActorHiddenInGame(true);
	Object2->SetActorHiddenInGame(true);
	Object1->SetActorEnableCollision(false);
	Object2->SetActorEnableCollision(false);

	MergedObject1 = Object1;
	MergedObject2 = Object2;

	auto NextLevelObject = SpawnNewObject(NextLevelObjectName);
	NextLevelObject->SetActorLocation(Object2->GetActorLocation());
	NextLevelObject->SetActorRotation(Object2->GetActorRotation());
}

void AMBCityBuilderManager::UpdateQuestsForObjects(TArray<int32> ObjectIDs)
{
	TArray<AActor*> ObjectsToUpdate;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMBBaseCityObjectActor::StaticClass(), ObjectsToUpdate);

	for (auto Actor : ObjectsToUpdate)
	{
		AMBBaseCityObjectActor* CityObject = Cast<AMBBaseCityObjectActor>(Actor);

		if (!ObjectIDs.Contains(CityObject->CityObjectData.ObjectID))
		{
			continue;
		}

		CityObject->UpdateQuest();
	}
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

void AMBCityBuilderManager::HandleDragRelease()
{
	ECityObjectLocationState State = EditedObject->CheckLocation();
	if (State == ECityObjectLocationState::MergeReady)
	{
		TArray<AActor*> OverlappingActors;
		EditedObject->GetOverlappingActors(OverlappingActors, AMBBaseCityObjectActor::StaticClass());

		AMBBaseCityObjectActor* ObjectToMerge = nullptr;
		for (auto Actor : OverlappingActors)
		{
			if (Actor->GetClass() == EditedObject->GetClass())
			{
				ObjectToMerge = Cast<AMBBaseCityObjectActor>(Actor);
				break;
			}
		}

		EditedObject->Deselect();
		MergeObjects(EditedObject, ObjectToMerge);
	}
}

void AMBCityBuilderManager::MoveEditedObject(const FVector& DeltaLocation)
{
	check(EditedObject);

	EditedObject->AddActorWorldOffset(FVector(DeltaLocation.X, DeltaLocation.Y, 0.0f));

	ECityObjectLocationState State = EditedObject->CheckLocation();
	EditedObject->SetEditMaterial(State);
}

void AMBCityBuilderManager::RotateEditedObject(int32 Direction)
{
	check(EditedObject);

	FRotator DeltaRotation = FRotator::ZeroRotator;
	DeltaRotation.Yaw = Direction * 45.0f;
	EditedObject->AddActorWorldRotation(DeltaRotation);

	ECityObjectLocationState State = EditedObject->CheckLocation();
	EditedObject->SetEditMaterial(State);
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
		EditedObject->Deselect();
	}

	if (MergedObject1)
	{
		FTransform PreEditedTransform;
		PreEditedTransform.SetLocation(MergedObject1->CityObjectData.Location);
		FRotator Rotation = FRotator::ZeroRotator;
		Rotation.Yaw = MergedObject1->CityObjectData.Rotation;
		PreEditedTransform.SetRotation(Rotation.Quaternion());
		PreEditedTransform.SetScale3D(FVector(MergedObject1->CityObjectData.Scale));

		MergedObject1->SetActorTransform(PreEditedTransform);
		
		MergedObject1->SetActorHiddenInGame(false);
		MergedObject1->SetActorEnableCollision(true);
		MergedObject1 = nullptr;
	}

	if (MergedObject2)
	{
		MergedObject2->SetActorHiddenInGame(false);
		MergedObject2->SetActorEnableCollision(true);
		MergedObject2 = nullptr;
	}

	EditedObject = nullptr;
}

void AMBCityBuilderManager::HandleObjectClick(AMBBaseCityObjectActor* CityObject)
{
	OnObjectClicked.Broadcast(CityObject);
}