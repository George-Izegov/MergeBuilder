// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestSystem/MBQuestSubsystem.h"

#include "JsonObjectConverter.h"
#include "MBUtilityFunctionLibrary.h"
#include "TimeSubsystem.h"
#include "CitySystem/CityBuilderSubsystem.h"
#include "Kismet/KismetMathLibrary.h"

UMBQuestSubsystem::UMBQuestSubsystem()
{
}

void UMBQuestSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	FTimerDelegate Delegate = FTimerDelegate::CreateLambda([this]() {
		auto TimeSystem = GetGameInstance()->GetSubsystem<UTimeSubsystem>();
		OnGetTimeDelegateHandle = TimeSystem->OnTimeSuccessRequested.AddUObject(this, &UMBQuestSubsystem::InitQuests);
	});

	GetWorld()->GetTimerManager().SetTimerForNextTick(Delegate);
}

void UMBQuestSubsystem::Deinitialize()
{
	Super::Deinitialize();

	SaveQuests();
}

void UMBQuestSubsystem::SaveQuests()
{
	if (!IsInitialized)
		return;
	
	TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();

	TArray<TSharedPtr<FJsonValue>> QuestsArray;

	for (const auto& Quest : Quests)
	{
		TSharedPtr<FJsonObject> JsonQuest = FJsonObjectConverter::UStructToJsonObject<FQuestData>(Quest);
		TSharedPtr<FJsonValueObject> QuestValue = MakeShared<FJsonValueObject>(JsonQuest);

		QuestsArray.Add(QuestValue);
	}

	JsonObject->SetArrayField("Quests", QuestsArray);
	JsonObject->SetStringField("DateTo", DateTo.ToIso8601());

	FString StringData;
	UMBUtilityFunctionLibrary::JsonObjectToString(JsonObject, StringData);

	UMBUtilityFunctionLibrary::SaveToStorage("Quests", StringData);
}

void UMBQuestSubsystem::InitQuests()
{
	auto TimeSystem = GetGameInstance()->GetSubsystem<UTimeSubsystem>();
	
	FString SavedData;
	if (UMBUtilityFunctionLibrary::ReadFromStorage("Quests", SavedData))
	{
		ParseQuests(SavedData);
	}

	if (TimeSystem->GetUTCNow() > DateTo)
	{
		GenerateNewQuests();
	}
	
	IsInitialized = true;
}

void UMBQuestSubsystem::ParseQuests(const FString& JsonString)
{
	TSharedPtr<FJsonObject> JsonObject;

	if (!UMBUtilityFunctionLibrary::StringToJsonObject(JsonString, JsonObject))
		return;

	Quests.Empty();
	
	const TArray<TSharedPtr<FJsonValue>>* QuestsArray;
	if (JsonObject->TryGetArrayField("Quests", QuestsArray))
	{
		for (const auto& QuestValue : *QuestsArray)
		{
			FQuestData Quest;
			if (!FJsonObjectConverter::JsonObjectToUStruct<FQuestData>(QuestValue->AsObject().ToSharedRef(), &Quest))
				continue;
		
			Quests.Add(Quest);
		}
	}

	FDateTime::ParseIso8601(*(JsonObject->GetStringField("DateTo")), DateTo);
}

void UMBQuestSubsystem::GenerateNewQuests()
{
	Quests.Empty();
	
	int32 QuestsNum = GenerateQuestCount();

	FQuestData RecommendedQuest;
	if (GetRecommendedQuest(RecommendedQuest))
	{
		Quests.Add(RecommendedQuest);
		QuestsNum--;
	}

	for (int32 i = 0; i < QuestsNum; ++i)
	{
		FQuestData NewQuest;

		NewQuest.QuestID = GetTypeHash(&NewQuest);

		NewQuest.QuestType = GenerateTypeForQuest();

		switch (NewQuest.QuestType)
		{
			case EQuestType::MergeItems:
				{
					GenerateRequiredMergeItemsForQuest(NewQuest.RequiredItems);
					GenerateRewardForMergeItems(NewQuest.RequiredItems, NewQuest.RewardItems, NewQuest.RewardExperience);
					break;
				}
			case EQuestType::CityObjects:
				{
					GenerateRequiredCityObjectForQuest(NewQuest.RequiredObjectName, NewQuest.RequiredObjectAmount);
					GenerateRewardForCityObject(NewQuest.RequiredObjectName, NewQuest.RequiredObjectAmount, NewQuest.RewardItems, NewQuest.RewardExperience);
					break;
				}
		}

		Quests.Add(NewQuest);
	}

	auto TimeSystem = GetGameInstance()->GetSubsystem<UTimeSubsystem>();
	
	DateTo = TimeSystem->GetUTCNow() + FTimespan::FromHours(RefreshHours);
}

EQuestType UMBQuestSubsystem::GenerateTypeForQuest()
{
	bool IsObjectType = UKismetMathLibrary::RandomBoolWithWeight(0.6f);

	if (IsObjectType)
		return EQuestType::CityObjects;

	return EQuestType::MergeItems;
}

int32 UMBQuestSubsystem::GenerateQuestCount()
{
	return 3;
}

void UMBQuestSubsystem::GenerateRequiredMergeItemsForQuest(TArray<FRequiredItem>& RequiredItems)
{
	auto CitySubsystem = GetGameInstance()->GetSubsystem<UCityBuilderSubsystem>();

	TArray<EMergeItemType> PossibleItemTypes;

	PossibleItemTypes.Add(EMergeItemType::Tools);
	
	if (CitySubsystem->HasGenerator(FName("Mine")))
		PossibleItemTypes.Add(EMergeItemType::Stones);

	if (CitySubsystem->HasGenerator(FName("LumberFactory")))
		PossibleItemTypes.Add(EMergeItemType::Lumber);

	if (CitySubsystem->HasGenerator(FName("MetalFactory")))
		PossibleItemTypes.Add(EMergeItemType::Metal);

	int32 RandomIndex = UKismetMathLibrary::RandomIntegerInRange(0, PossibleItemTypes.Num() - 1);
	FRequiredItem FirstItem;
	FirstItem.Item.Type = PossibleItemTypes[RandomIndex];
	GenerateMergeItemForQuest(FirstItem.Item.Type, FirstItem);

	RequiredItems.Add(FirstItem);

	bool SecondItemChance = UKismetMathLibrary::RandomBoolWithWeight(0.1f);

	if (SecondItemChance)
	{
		RandomIndex = UKismetMathLibrary::RandomIntegerInRange(0, PossibleItemTypes.Num() - 1);
		FRequiredItem SecondItem;
		SecondItem.Item.Type = PossibleItemTypes[RandomIndex];
		GenerateMergeItemForQuest(SecondItem.Item.Type, SecondItem);

		if (FirstItem.Item != SecondItem.Item)
			RequiredItems.Add(SecondItem);
	}
}

void UMBQuestSubsystem::GenerateRequiredCityObjectForQuest(FName& RequiredObjectName, int32& RequiredObjectAmount)
{
	auto CitySubsystem = GetGameInstance()->GetSubsystem<UCityBuilderSubsystem>();

	TMap<FName, FCityObjectData*> QuestObjects;
	CitySubsystem->GetQuestObjects(QuestObjects);

	int32 RandomIndex = UKismetMathLibrary::RandomIntegerInRange(0, QuestObjects.Num() - 1);

	TArray<FName> Keys;
	QuestObjects.GetKeys(Keys);

	RequiredObjectName = Keys[RandomIndex];

	if (QuestObjects[RequiredObjectName]->CoinsType != EConsumableParamType::SoftCoin)
	{
		RequiredObjectAmount = 1;
		return;
	}

	int32 MaxAmount = 100 / QuestObjects[RequiredObjectName]->CostInCoins;
	MaxAmount = FMath::Clamp(MaxAmount, 1, 5);
	RequiredObjectAmount = UKismetMathLibrary::RandomIntegerInRange(1, MaxAmount);
}

void UMBQuestSubsystem::GenerateRewardForMergeItems(const TArray<FRequiredItem>& RequiredItems,
	TArray<FMergeFieldItem>& RewardItems, int32& RewardExperience)
{
	int32 QuestHardness = CalculateHardnessOfRequiredObjects(RequiredItems);

	GenerateRewardForHardness(QuestHardness, RewardItems, RewardExperience);
}

void UMBQuestSubsystem::GenerateRewardForCityObject(const FName& RequiredObjectName, int32 ObjectAmount,
	TArray<FMergeFieldItem>& RewardItems, int32& RewardExperience)
{
	auto CitySubsystem = GetGameInstance()->GetSubsystem<UCityBuilderSubsystem>();

	const FCityObjectData* RowStruct = CitySubsystem->CityObjectsDataTable->FindRow<FCityObjectData>(RequiredObjectName, "");

	int32 QuestHardness = CalculateHardnessOfRequiredObjects(RowStruct->RequiredItems);

	if (RowStruct->CoinsType != EConsumableParamType::SoftCoin)
	{
		QuestHardness += 100;
	}
	else
	{
		QuestHardness += RowStruct->CostInCoins;
	}
		
	QuestHardness *= ObjectAmount;

	GenerateRewardForHardness(QuestHardness, RewardItems, RewardExperience);
}

void UMBQuestSubsystem::GenerateRewardForHardness(int32 QuestHardness, TArray<FMergeFieldItem>& RewardItems,
	int32& RewardExperience)
{
	FMergeFieldItem SoftCoins;
	SoftCoins.Type = EMergeItemType::SoftCoinBox;
	SoftCoins.Level = 1;
	SoftCoins.RemainItemsToSpawn = 4 + QuestHardness / 10;

	RewardItems.Add(SoftCoins);

	if (QuestHardness > 50)
	{
		FMergeFieldItem PremCoin;
		PremCoin.Type = EMergeItemType::PremCoins;
		PremCoin.Level = 1 + QuestHardness / 100;
		PremCoin.Level = FMath::Min(PremCoin.Level, 3);

		RewardItems.Add(PremCoin);
	}

	RewardExperience = ((QuestHardness / 5) * 5) + 5;
}

void UMBQuestSubsystem::GenerateMergeItemForQuest(EMergeItemType ItemType, FRequiredItem& OutItem)
{
	auto MergeSubsystem = GetGameInstance()->GetSubsystem<UMergeSubsystem>();

	OutItem.Item.Type = ItemType;
	
	FString RowName = UMBUtilityFunctionLibrary::EnumToString("EMergeItemType", (int32)ItemType);

	const FMergeItemChainRow* RowStruct = MergeSubsystem->MergeItemsDataTable->FindRow<FMergeItemChainRow>(FName(RowName), "");
	int32 ItemMaxLevel = RowStruct->ItemsChain.Num();
	
	OutItem.Item.Level = UKismetMathLibrary::RandomIntegerInRange(1, ItemMaxLevel - 1);
	OutItem.RequiredNum = UKismetMathLibrary::RandomIntegerInRange(1, ItemMaxLevel - OutItem.Item.Level);
}

bool UMBQuestSubsystem::GetRecommendedQuest(FQuestData& RecommendedQuest)
{
	return false;
}

int32 UMBQuestSubsystem::CalculateHardnessOfRequiredObjects(const TArray<FRequiredItem>& RequiredItems)
{
	int32 Hardness = 0;

	auto MergeSubsystem = GetGameInstance()->GetSubsystem<UMergeSubsystem>();

	for (const auto& RequiredItem : RequiredItems)
	{
		FString RowName = UMBUtilityFunctionLibrary::EnumToString("EMergeItemType", (int32)RequiredItem.Item.Type);
		const FMergeItemChainRow* RowStruct = MergeSubsystem->MergeItemsDataTable->FindRow<FMergeItemChainRow>(FName(RowName), "");

		int32 ItemCost = FMath::Pow(2.0f, RequiredItem.Item.Level);

		ItemCost *= RequiredItem.RequiredNum;

		ItemCost *= RowStruct->ItemsChain[0].SellPrice;

		Hardness += ItemCost;
	}

	return Hardness;
}

