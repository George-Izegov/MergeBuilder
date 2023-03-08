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

			CityObjects.Add(Item);
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
		TSharedPtr<FJsonObject> CityJsonObject = FJsonObjectConverter::UStructToJsonObject<FCityObject>(Object);
		TSharedPtr<FJsonValue> ObjectValue = MakeShared<FJsonValueObject>(CityJsonObject);

		CityJsonArray.Add(ObjectValue);
	}

	JsonObject->SetArrayField("cityObjects", CityJsonArray);

	FString StringData;
	UMBUtilityFunctionLibrary::JsonObjectToString(JsonObject, StringData);

	UMBUtilityFunctionLibrary::SaveToStorage("City", StringData);
}
