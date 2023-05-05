// Fill out your copyright notice in the Description page of Project Settings.


#include "CitySystem/CityBuilderSubsystem.h"
#include "MBUtilityFunctionLibrary.h"
#include "JsonObjectConverter.h"
#include "TimeSubsystem.h"
#include "User/AccountSubsystem.h"

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

	AddExperienceForNewObject(NewObject.ObjectName);
	CalculateCurrentPopulationAndRatings();
}

void UCityBuilderSubsystem::EditObject(const FCityObject& EditedObject)
{
	CityObjects[EditedObject.ObjectID] = EditedObject;
}

void UCityBuilderSubsystem::RemoveObject(const FCityObject& ObjectToRemove)
{
	CityObjects[ObjectToRemove.ObjectID].ObjectName = NAME_None;

	CalculateCurrentPopulationAndRatings();
}

void UCityBuilderSubsystem::CollectFromObject(FCityObject& Object)
{
	auto TimeSubsystem = GetGameInstance()->GetSubsystem<UTimeSubsystem>();
	auto MergeSubsystem = GetGameInstance()->GetSubsystem<UMergeSubsystem>();
	
	check(CityObjects[Object.ObjectID].RestoreTime < TimeSubsystem->GetUTCNow());

	const FCityObjectData* RowStruct = CityObjectsDataTable->FindRow<FCityObjectData>(Object.ObjectName, "");

	MergeSubsystem->AddNewReward(RowStruct->GeneratorSettings.GeneratedBox);

	FTimespan RestoreDuration = FTimespan::FromSeconds(RowStruct->GeneratorSettings.MinutesToRestore * 60);
	CityObjects[Object.ObjectID].RestoreTime = TimeSubsystem->GetUTCNow() + RestoreDuration;
	Object = CityObjects[Object.ObjectID];
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
	if (!UMBUtilityFunctionLibrary::ReadFromStorage("City", SavedData))
	{
		FString StartCityJsonPath = FPaths::ProjectContentDir() + "/Jsons/StartCity.json";
		FFileHelper::LoadFileToString(SavedData, *StartCityJsonPath);
		
		//  TODO: move this logic to tutorial subsystem
		FCityObjectData* RowStruct = CityObjectsDataTable->FindRow<FCityObjectData>(FName("Mine"), "");
		RowStruct->RequiredItems[0].RequiredNum = 1;
	}

	ParseCity(SavedData);
	CalculateCurrentPopulationAndRatings();
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

void UCityBuilderSubsystem::CalculateCurrentPopulationAndRatings()
{
	int32 TotalPopulation = 1;
	int32 TotalEmployed = 0;
	FCityRatings NewRatings;

	for (const auto& Object : CityObjects)
	{
		if (Object.ObjectName == NAME_None)
			continue;

		const FCityObjectData* RowStruct = CityObjectsDataTable->FindRow<FCityObjectData>(Object.ObjectName, "");

		if (!RowStruct)
			continue;

		TotalPopulation += RowStruct->AdditionalPopulation;
		TotalEmployed += RowStruct->GeneratorSettings.RequiredEmployees;
		NewRatings += RowStruct->AdditionalRatings;
	}

	Population = TotalPopulation;
	EmployedPopulation = TotalEmployed;
	CityRating = NewRatings;
}

void UCityBuilderSubsystem::GetTopRatingsForLevel(int32 Level, FCityRatings& TopRatings)
{
	int32 Grade = Level;

	TopRatings.Comfort = 10 * Grade;
	TopRatings.Greening = 10 * Grade;
	TopRatings.Infrastructure = 10 * Grade;
}

float UCityBuilderSubsystem::GetAverageRating()
{
	UAccountSubsystem* AccountSubsystem = GetGameInstance()->GetSubsystem<UAccountSubsystem>();

	FCityRatings TopRatings;
	GetTopRatingsForLevel(AccountSubsystem->GetLevel(), TopRatings);

	float ComfortPercentage = FMath::Clamp((float)CityRating.Comfort / TopRatings.Comfort, 0.0f, 1.0f);
	float GreeningPercentage = FMath::Clamp((float)CityRating.Greening / TopRatings.Greening, 0.0f, 1.0f);
	float InfrastructurePercentage = FMath::Clamp((float)CityRating.Infrastructure / TopRatings.Infrastructure, 0.0f, 1.0f);

	return (ComfortPercentage + GreeningPercentage + InfrastructurePercentage) / 3;
}

void UCityBuilderSubsystem::GetObjectsChain(const FName& ObjectName, TArray<FCityObjectData>& OutObjects, int32& CurrentIndex)
{
	FName PreviousObjectName = NAME_None;
	FName CurrentObjectName = ObjectName;
	do
	{
		TMap<FName, uint8*> RowsMap = CityObjectsDataTable->GetRowMap();

		PreviousObjectName = NAME_None;
		for (const auto& Row : RowsMap)
		{
			auto RowData = (FCityObjectData*)Row.Value;
			
			if (RowData->NextLevelObjectName == CurrentObjectName)
			{
				PreviousObjectName = Row.Key;
				CurrentObjectName = Row.Key;
				OutObjects.Insert(*RowData, 0);
				break;
			}
		}
	} while (PreviousObjectName != NAME_None);

	CurrentObjectName = ObjectName;
	CurrentIndex = OutObjects.Num();
	while(true)
	{
		auto Row = CityObjectsDataTable->FindRow<FCityObjectData>(CurrentObjectName, "");

		if (!Row)
			break;

		OutObjects.Add(*Row);
		CurrentObjectName = Row->NextLevelObjectName;
	} 
}

bool UCityBuilderSubsystem::HasGenerator(const FName& ObjectName)
{
	for (const auto& Object : CityObjects)
	{
		if (Object.ObjectName == ObjectName)
			return true;

		if (Object.ObjectName == FName(ObjectName.ToString() + "2"))
			return true;

		if (Object.ObjectName == FName(ObjectName.ToString() + "3"))
			return true;
	}

	return false;
}

void UCityBuilderSubsystem::GetQuestObjects(TMap<FName, FCityObjectData*>& OutObjects)
{
	TMap<FName, uint8*> RowsMap = CityObjectsDataTable->GetRowMap();

	for (const auto& Row : RowsMap)
	{
		auto RowData = (FCityObjectData*)Row.Value;

		if (RowData->FitForQuest)
			OutObjects.Add(Row.Key, RowData);
	}
}

void UCityBuilderSubsystem::AddExperienceForNewObject(const FName& NewObjectName)
{
	const FCityObjectData* RowStruct = CityObjectsDataTable->FindRow<FCityObjectData>(NewObjectName, "");

	if (!RowStruct)
		return;

	int32 Experience = 0;

	switch (RowStruct->CoinsType)
	{
	case EConsumableParamType::SoftCoin:
		Experience = RowStruct->CostInCoins / 10;
		break;
	case EConsumableParamType::PremCoin:
		Experience = RowStruct->CostInCoins * 5;
		break;
	}

	Experience = FMath::Max(1, Experience);

	auto AccountSubsystem = GetGameInstance()->GetSubsystem<UAccountSubsystem>();
	AccountSubsystem->AddExperience(Experience);
}

bool UCityBuilderSubsystem::CheckRequierementsForBuildObject(const FName& ObjectName)
{
	auto MergeSubsystem = GetGameInstance()->GetSubsystem<UMergeSubsystem>();
	auto AccountSubsystem = GetGameInstance()->GetSubsystem<UAccountSubsystem>();

	const FCityObjectData* RowStruct = CityObjectsDataTable->FindRow<FCityObjectData>(ObjectName, "");
	check(RowStruct);

	int32 UnemployedPopulation = FMath::Max(0, Population - EmployedPopulation);
	if (RowStruct->GeneratorSettings.RequiredEmployees > UnemployedPopulation)
		return false;

	for (const auto& Item : RowStruct->RequiredItems)
	{
		int32 TotalCount = MergeSubsystem->GetItemTotalCount(Item.Item);

		if (TotalCount < Item.RequiredNum)
			return false;
	}

	switch (RowStruct->CoinsType)
	{
	case EConsumableParamType::SoftCoin:
		if (AccountSubsystem->GetSoftCoins() < RowStruct->CostInCoins)
			return false;
		break;
	case EConsumableParamType::PremCoin:
		if (AccountSubsystem->GetPremCoins() < RowStruct->CostInCoins)
			return false;
		break;
	}

	return true;
}


void UCityBuilderSubsystem::SpendResourcesForBuildObject(const FName& ObjectName)
{
	auto MergeSubsystem = GetGameInstance()->GetSubsystem<UMergeSubsystem>();
	auto AccountSubsystem = GetGameInstance()->GetSubsystem<UAccountSubsystem>();

	const FCityObjectData* RowStruct = CityObjectsDataTable->FindRow<FCityObjectData>(ObjectName, "");
	check(RowStruct);

	for (const auto& Item : RowStruct->RequiredItems)
	{
		MergeSubsystem->SpendItems(Item.Item, Item.RequiredNum);
	}

	switch (RowStruct->CoinsType)
	{
	case EConsumableParamType::SoftCoin:
		AccountSubsystem->SpendSoftCoins(RowStruct->CostInCoins);
		break;
	case EConsumableParamType::PremCoin:
		AccountSubsystem->SpendPremCoins(RowStruct->CostInCoins);
		break;
	}
}