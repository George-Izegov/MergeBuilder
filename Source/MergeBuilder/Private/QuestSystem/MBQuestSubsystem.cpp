// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestSystem/MBQuestSubsystem.h"

#include "JsonObjectConverter.h"
#include "MBUtilityFunctionLibrary.h"
#include "TimeSubsystem.h"
#include "CitySystem/CityBuilderSubsystem.h"
#include "Kismet/KismetMathLibrary.h"
#include "CitySystem/CityBuilderSubsystem.h"
#include "User/AccountSubsystem.h"

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

bool UMBQuestSubsystem::GetQuestByID(const FString& QuestID, FQuestData& OutQuest)
{
	for (const auto& Quest : Quests)
	{
		if (Quest.QuestID == QuestID)
		{
			OutQuest = Quest;
			return true;
		}
	}
	return false;
}

bool UMBQuestSubsystem::CheckQuestRequirements(const FQuestData& Quest)
{
	switch (Quest.QuestType)
	{
	case EQuestType::CityObjects:
		return Quest.RequiredObjectProgress >= Quest.RequiredObjectAmount;
	case EQuestType::MergeItems:
		for (const auto& Item : Quest.RequiredItems)
		{
			auto MergeSubsystem = GetGameInstance()->GetSubsystem<UMergeSubsystem>();
			int32 ItemsCount = MergeSubsystem->GetItemTotalCount(Item.Item);

			if (ItemsCount < Item.RequiredNum)
				return false;
		}

		return true;
	}

	return false;
}

void UMBQuestSubsystem::CompleteQuest(const FString& QuestID)
{
	FQuestData Quest;
	if (!GetQuestByID(QuestID, Quest))
	{
		check(nullptr);
	}

	if (!CheckQuestRequirements(Quest))
		return;

	auto MergeSubsystem = GetGameInstance()->GetSubsystem<UMergeSubsystem>();

	if (Quest.QuestType == EQuestType::MergeItems)
	{
		for (const auto& Item : Quest.RequiredItems)
		{
			MergeSubsystem->SpendItems(Item.Item, Item.RequiredNum);
		}
	}
	
	for (const auto& RewardItem : Quest.RewardItems)
	{
		MergeSubsystem->AddNewReward(RewardItem);
	}

	auto AccountSubsystem = GetGameInstance()->GetSubsystem<UAccountSubsystem>();

	AccountSubsystem->AddExperience(Quest.RewardExperience);

	Quests.Remove(Quest);

	UpdateQuests();
}

TArray<FString> UMBQuestSubsystem::GetAllQuestIDs()
{
	TArray<FString> QuestIDs;
	for (const auto& Quest : Quests)
	{
		QuestIDs.Add(Quest.QuestID);
	}
	return QuestIDs;
}

void UMBQuestSubsystem::InitQuests()
{
	FString SavedData;
	if (UMBUtilityFunctionLibrary::ReadFromStorage("Quests", SavedData))
	{
		ParseQuests(SavedData);
	}

	CheckRefreshQuestsTimer();
	
	IsInitialized = true;

	auto CityBuilderSubsystem = GetGameInstance()->GetSubsystem<UCityBuilderSubsystem>();
	
	CityBuilderSubsystem->SetNewQuestsForObjects(GetAllQuestIDs());

	CityBuilderSubsystem->OnBuildNewObject.AddDynamic(this, &UMBQuestSubsystem::UpdateCityObjectBuildQuests);

	GetWorld()->GetTimerManager().SetTimer(QuestRefreshTimerHandle, this, &UMBQuestSubsystem::CheckRefreshQuestsTimer, 1.0f, true);

	auto TimeSubsystem = GetGameInstance()->GetSubsystem<UTimeSubsystem>();
	TimeSubsystem->OnTimeSuccessRequested.Remove(OnGetTimeDelegateHandle);
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

		MakeQuestID(NewQuest);

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
	TArray<EMergeItemType> PossibleItemTypes;

	GetPossibleItemTypes(PossibleItemTypes);

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
	TMap<FName, FCityObjectData*> QuestObjects;
	GetQuestObjects(QuestObjects);

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
	auto CitySubsystem = GetGameInstance()->GetSubsystem<UCityBuilderSubsystem>();

	if (!CitySubsystem->HasGenerator("Mine"))
	{
		RecommendedQuest.QuestType = EQuestType::CityObjects;
		RecommendedQuest.RequiredObjectName = FName("Mine");
		RecommendedQuest.RequiredObjectAmount = 1;

		RecommendedQuest.RewardExperience = 150;

		FMergeFieldItem SoftCoins;
		SoftCoins.Type = EMergeItemType::SoftCoinBox;
		SoftCoins.Level = 1;
		SoftCoins.RemainItemsToSpawn = 20;
		
		RecommendedQuest.RewardItems.Add(SoftCoins);

		FMergeFieldItem PremCoins;
		PremCoins.Type = EMergeItemType::PremCoins;
		PremCoins.Level = 2;

		RecommendedQuest.RewardItems.Add(PremCoins);
		MakeQuestID(RecommendedQuest);
		
		return true;
	}

	if (!CitySubsystem->HasGenerator("LumberPlant"))
	{
		RecommendedQuest.QuestType = EQuestType::CityObjects;
		RecommendedQuest.RequiredObjectName = FName("LumberPlant");
		RecommendedQuest.RequiredObjectAmount = 1;

		RecommendedQuest.RewardExperience = 200;

		FMergeFieldItem SoftCoins;
		SoftCoins.Type = EMergeItemType::SoftCoinBox;
		SoftCoins.Level = 1;
		SoftCoins.RemainItemsToSpawn = 20;
		
		RecommendedQuest.RewardItems.Add(SoftCoins);

		FMergeFieldItem PremCoins;
		PremCoins.Type = EMergeItemType::PremCoins;
		PremCoins.Level = 2;

		RecommendedQuest.RewardItems.Add(PremCoins);
		MakeQuestID(RecommendedQuest);
		
		return true;
	}

	if (!CitySubsystem->HasGenerator("MetalPlant"))
	{
		RecommendedQuest.QuestType = EQuestType::CityObjects;
		RecommendedQuest.RequiredObjectName = FName("MetalPlant");
		RecommendedQuest.RequiredObjectAmount = 1;

		RecommendedQuest.RewardExperience = 250;

		FMergeFieldItem SoftCoins;
		SoftCoins.Type = EMergeItemType::SoftCoinBox;
		SoftCoins.Level = 1;
		SoftCoins.RemainItemsToSpawn = 20;
		
		RecommendedQuest.RewardItems.Add(SoftCoins);

		FMergeFieldItem PremCoins;
		PremCoins.Type = EMergeItemType::PremCoins;
		PremCoins.Level = 3;

		RecommendedQuest.RewardItems.Add(PremCoins);
		MakeQuestID(RecommendedQuest);
		
		return true;
	}
	
	return false;
}

void UMBQuestSubsystem::MakeQuestID(FQuestData& Quest)
{
	FString QuestID;

	switch (Quest.QuestType)
	{
	case EQuestType::MergeItems:
		QuestID = "Merge";

		for (const auto& Item : Quest.RequiredItems)
		{
			QuestID += ("_" + UMBUtilityFunctionLibrary::EnumToString("EMergeItemType", (int32)Item.Item.Type));
			QuestID += FString::FromInt(Item.Item.Level);
			QuestID += ("_" + FString::FromInt(Item.RequiredNum));
		}
		
		break;
	case EQuestType::CityObjects:
		QuestID = "Build_";
		QuestID += Quest.RequiredObjectName.ToString();
		QuestID += ("_" + FString::FromInt(Quest.RequiredObjectAmount));
		
		break;
	}

	Quest.QuestID = QuestID;
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

void UMBQuestSubsystem::UpdateCityObjectBuildQuests(FName NewBuildObject)
{
	for (auto& Quest : Quests)
	{
		if (Quest.QuestType != EQuestType::CityObjects)
			continue;

		if (Quest.RequiredObjectName == NewBuildObject)
			Quest.RequiredObjectProgress++;
	}
	
	UpdateQuests();
}

void UMBQuestSubsystem::CheckRefreshQuestsTimer()
{
	auto TimeSystem = GetGameInstance()->GetSubsystem<UTimeSubsystem>();
	
	if (TimeSystem->GetUTCNow() > DateTo)
	{
		GenerateNewQuests();

		UpdateQuests();
	}
}

void UMBQuestSubsystem::GetPossibleItemTypes(TArray<EMergeItemType>& OutItemTypes)
{
	auto CitySubsystem = GetGameInstance()->GetSubsystem<UCityBuilderSubsystem>();

	OutItemTypes.Add(EMergeItemType::Tools);
	
	if (CitySubsystem->HasGenerator(FName("Mine")))
		OutItemTypes.Add(EMergeItemType::Stones);

	if (CitySubsystem->HasGenerator(FName("LumberFactory")))
		OutItemTypes.Add(EMergeItemType::Lumber);

	if (CitySubsystem->HasGenerator(FName("MetalFactory")))
		OutItemTypes.Add(EMergeItemType::Metal);
}

void UMBQuestSubsystem::UpdateQuests()
{
	auto CityBuilderSubsystem = GetGameInstance()->GetSubsystem<UCityBuilderSubsystem>();
	CityBuilderSubsystem->SetNewQuestsForObjects(GetAllQuestIDs());

	SaveQuests();
}

void UMBQuestSubsystem::GenerateNewQuestsForPrem()
{
	auto AccountSubsystem = GetGameInstance()->GetSubsystem<UAccountSubsystem>();
	auto TimeSubsystem = GetGameInstance()->GetSubsystem<UTimeSubsystem>();

	FTimespan TotalTime = FTimespan::FromHours(RefreshHours);
	FTimespan RemainTime = DateTo - TimeSubsystem->GetUTCNow();
	
	int32 Price = UMBUtilityFunctionLibrary::GetSkipTimerPrice(TotalTime, RemainTime, QuestRefreshPremPrice);

	if (AccountSubsystem->GetPremCoins() < Price)
		return;

	AccountSubsystem->SpendPremCoins(Price);

	GenerateNewQuests();

	UpdateQuests();
}

void UMBQuestSubsystem::HandleSuccessWatchVideoForQuests()
{
	FTimespan SkipTime = FTimespan::FromMinutes(AdSkipMinutes);

	DateTo -= SkipTime;

	CheckRefreshQuestsTimer();

	SaveQuests();
}

void UMBQuestSubsystem::GetQuestObjects(TMap<FName, FCityObjectData*>& OutObjects)
{
	auto CityBuilderSubsystem = GetGameInstance()->GetSubsystem<UCityBuilderSubsystem>();
	
	TMap<FName, uint8*> RowsMap = CityBuilderSubsystem->CityObjectsDataTable->GetRowMap();

	TArray<EMergeItemType> PossibleItemTypes;
	GetPossibleItemTypes(PossibleItemTypes);

	for (const auto& Row : RowsMap)
	{
		auto RowData = (FCityObjectData*)Row.Value;

		if (!RowData->FitForQuest)
			continue;

		bool IsPossible = true;
		for (const auto& Item : RowData->RequiredItems)
		{
			if (!PossibleItemTypes.Contains(Item.Item.Type))
			{
				IsPossible = false;
				break;
			}
		}

		if (!IsPossible)
			continue;
		
		OutObjects.Add(Row.Key, RowData);
	}
}
