// Fill out your copyright notice in the Description page of Project Settings.


#include "CitySystem/CityBuilderSubsystem.h"
#include "MBUtilityFunctionLibrary.h"
#include "JsonObjectConverter.h"

UCityBuilderSubsystem::UCityBuilderSubsystem()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> ItemsDataTable(TEXT("DataTable'/Game/Development/DataTables/CityObjects.CityObjects'"));
	if (ItemsDataTable.Succeeded())
	{
		CityObjectsDataTable = ItemsDataTable.Object;
	}

	check(CityObjectsDataTable);
}

void UCityBuilderSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	InitCity();
}

void UCityBuilderSubsystem::Deinitialize()
{
	SaveCity();
}

void UCityBuilderSubsystem::AddNewObject(FCityObject& NewObject)
{
	int32 ID = CityObjects.Add(NewObject);
	CityObjects[ID].ObjectID = ID;
	NewObject.ObjectID = ID;
}

void UCityBuilderSubsystem::EditObject(const FCityObject& EditedObject)
{
	CityObjects[EditedObject.ObjectID] = EditedObject;
}

void UCityBuilderSubsystem::RemoveObject(const FCityObject& ObjectToRemove)
{
	CityObjects[ObjectToRemove.ObjectID].ObjectName = NAME_None;
}

void UCityBuilderSubsystem::ParseCity(const FString& JsonString)
{
	TSharedPtr<FJsonObject> JsonObject;

	if (!UMBUtilityFunctionLibrary::StringToJsonObject(JsonString, JsonObject))
		return;

	const TArray<TSharedPtr<FJsonValue>>* CityObjectsJsonArray;

	if (JsonObject.Get()->TryGetArrayField("cityObjects", CityObjectsJsonArray))
	{
		CityObjects.Empty();
		for (const auto& CityObjectValue : *CityObjectsJsonArray)
		{
			FCityObject Item;
			FJsonObjectConverter::JsonObjectToUStruct<FCityObject>(CityObjectValue->AsObject().ToSharedRef(), &Item);

			int32 ID = CityObjects.Add(Item);
			CityObjects[ID].ObjectID = ID;
		}
	}
}

void UCityBuilderSubsystem::InitCity()
{
	FString SavedData;
	if (UMBUtilityFunctionLibrary::ReadFromStorage("City", SavedData))
	{
		ParseCity(SavedData);
	}
}

void UCityBuilderSubsystem::SaveCity()
{
	TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();

	TArray<TSharedPtr<FJsonValue>> CityJsonArray;

	for (const auto& Object : CityObjects)
	{
		if (Object.ObjectName == NAME_None)
			continue;

		TSharedPtr<FJsonObject> CityJsonObject = FJsonObjectConverter::UStructToJsonObject<FCityObject>(Object);
		TSharedPtr<FJsonValue> ObjectValue = MakeShared<FJsonValueObject>(CityJsonObject);

		CityJsonArray.Add(ObjectValue);
	}

	JsonObject->SetArrayField("cityObjects", CityJsonArray);

	FString StringData;
	UMBUtilityFunctionLibrary::JsonObjectToString(JsonObject, StringData);

	UMBUtilityFunctionLibrary::SaveToStorage("City", StringData);
}
