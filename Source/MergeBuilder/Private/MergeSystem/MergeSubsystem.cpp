// Fill out your copyright notice in the Description page of Project Settings.


#include "MergeSystem/MergeSubsystem.h"
#include "MBUtilityFunctionLibrary.h"
#include "JsonObjectConverter.h"

UMergeSubsystem::UMergeSubsystem()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> ItemsDataTable(TEXT("DataTable'/Game/Development/DataTables/MergeItems.MergeItems'"));
	if (ItemsDataTable.Succeeded())
	{
		MergeItemsDataTable = ItemsDataTable.Object;
	}
	check(MergeItemsDataTable);

	static ConstructorHelpers::FObjectFinder<UDataTable> StartField(TEXT("DataTable'/Game/Development/DataTables/StartMergeField.StartMergeField'"));
	if (StartField.Succeeded())
	{
		StartFieldDataTable = StartField.Object;
	}
	check(StartFieldDataTable);
}

void UMergeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	TArray<FMergeFieldItem> ZeroRow;
	ZeroRow.SetNumZeroed(MergeFieldSize.X);

	MergeField.Init(ZeroRow, MergeFieldSize.Y);

	FString SavedData;
	if (UMBUtilityFunctionLibrary::ReadFromStorage("Inventory", SavedData))
	{
		ParseField(SavedData);
	}
	else
	{
		InitFieldFromStartTable();
	}
}

void UMergeSubsystem::Deinitialize()
{
	SaveField();
}

void UMergeSubsystem::InitFieldFromStartTable()
{
	for (int32 i = 0; i < MergeFieldSize.Y; i++)
	{
		auto FieldRow = StartFieldDataTable->FindRow<FMergeItemsField>(FName(FString::FromInt(i)), "");
		if (!FieldRow)
			continue;

		for (int32 j = 0; j < MergeFieldSize.X; j++)
		{
			if (FieldRow->ItemsRow.Num() <= j)
				break;

			const auto& Item = FieldRow->ItemsRow[j];

			if (Item.Type == EMergeItemType::None)
				continue;

			MergeField[i][j] = Item;
		}
	}
}

void UMergeSubsystem::ParseField(const FString& JsonString)
{
	TSharedPtr<FJsonObject> JsonObject;
	
	if (!UMBUtilityFunctionLibrary::StringToJsonObject(JsonString, JsonObject))
		return;

	const TSharedPtr<FJsonObject>* FieldObject;

	if (JsonObject.Get()->TryGetObjectField("field", FieldObject))
	{
		for (int32 i = 0; i < MergeFieldSize.Y; i++)
		{
			const TSharedPtr<FJsonObject>* RowObject;
			if (!FieldObject->Get()->TryGetObjectField(FString::FromInt(i), RowObject))
				continue;

			for (int32 j = 0; j < MergeFieldSize.X; j++)
			{
				const TSharedPtr<FJsonObject>* ItemObject;
				if (!RowObject->Get()->TryGetObjectField(FString::FromInt(j), ItemObject))
					continue;

				FMergeFieldItem Item;

				FJsonObjectConverter::JsonObjectToUStruct<FMergeFieldItem>(ItemObject->ToSharedRef(), &Item);

				MergeField[i][j] = Item;
			}
		}
	}

	const TArray<TSharedPtr<FJsonValue>>* RewardsArray;
	if (JsonObject.Get()->TryGetArrayField("rewards", RewardsArray))
	{
		for (const auto& Value : *RewardsArray)
		{
			TSharedPtr<FJsonObject> RewardObject = Value->AsObject();

			FMergeFieldItem Item;

			FJsonObjectConverter::JsonObjectToUStruct<FMergeFieldItem>(RewardObject.ToSharedRef(), &Item);

			RewardsQueue.Add(Item);
		}
	}
}

void UMergeSubsystem::SaveField()
{
	TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();

	TSharedPtr<FJsonObject> FieldObject = MakeShared<FJsonObject>();

	for (int32 i = 0; i < MergeFieldSize.Y; i++)
	{
		TSharedPtr<FJsonObject> RowObject = MakeShared<FJsonObject>();

		for (int32 j = 0; j < MergeFieldSize.X; j++)
		{
			const FMergeFieldItem& Item = MergeField[i][j];

			if (Item.Type == EMergeItemType::None)
				continue;

			TSharedPtr<FJsonObject> ItemObject;

			ItemObject = FJsonObjectConverter::UStructToJsonObject<FMergeFieldItem>(Item);

			RowObject->SetObjectField(FString::FromInt(j), ItemObject);
		}

		FieldObject->SetObjectField(FString::FromInt(i), RowObject);
	}

	JsonObject->SetObjectField("field", FieldObject);

	TArray<TSharedPtr<FJsonValue>> RewardValues;

	for (const auto& Reward : RewardsQueue)
	{
		TSharedPtr<FJsonObject> RewardObject = FJsonObjectConverter::UStructToJsonObject<FMergeFieldItem>(Reward);
		TSharedPtr<FJsonValueObject> RewardValue = MakeShared<FJsonValueObject>(RewardObject);

		RewardValues.Add(RewardValue);
	}

	JsonObject->SetArrayField("rewards", RewardValues);

	FString StringData;
	UMBUtilityFunctionLibrary::JsonObjectToString(JsonObject, StringData);

	UMBUtilityFunctionLibrary::SaveToStorage("Inventory", StringData);
}

bool UMergeSubsystem::GetAllItemsInBoxAround(const FIntPoint& Index, TArray<FIntPoint>& OutItemIndexes)
{
	if (Index.X < 0 || Index.Y < 0 || Index.X >= MergeFieldSize.X || Index.Y >= MergeFieldSize.Y)
		return false;

	// for all indexes around
	for (int32 i = Index.X - 1; i <= Index.X + 1; i++)
	{
		for (int32 j = Index.Y - 1; j <= Index.Y + 1; j++)
		{
			if (i == Index.X && j == Index.Y)
				continue;

			if (i < 0 || j < 0 || i >= MergeFieldSize.X || j >= MergeFieldSize.Y)
				continue;

			if (MergeField[j][i].Type == EMergeItemType::None)
				continue;

			if (MergeField[j][i].IsInBox)
			{
				OutItemIndexes.Add(FIntPoint(i, j));
			}
		}
	}

	return OutItemIndexes.Num() > 0;
}

void UMergeSubsystem::OpenInBoxItem(const FIntPoint& Index, FMergeFieldItem& OutItem)
{
	if (Index.X < 0 || Index.Y < 0 || Index.X >= MergeFieldSize.X || Index.Y >= MergeFieldSize.Y)
	{
		UE_LOG(LogTemp, Error, TEXT("Index out of range"));
		return;
	}
	
	OutItem = MergeField[Index.Y][Index.X];

	OutItem.IsDusty = true;
	OutItem.IsInBox = false;

	MergeField[Index.Y][Index.X] = OutItem;
}

bool UMergeSubsystem::GetItemAt(const FIntPoint& Index, FMergeFieldItem& OutItem)
{
	if (Index.X < 0 || Index.X >= MergeFieldSize.X)
		return false;

	if (Index.Y < 0 || Index.Y >= MergeFieldSize.Y)
		return false;

	OutItem = MergeField[Index.Y][Index.X];

	if (OutItem.Type == EMergeItemType::None)
		return false;

	return true;
}

void UMergeSubsystem::SetItemAt(const FIntPoint& Index, const FMergeFieldItem& Item)
{
	if (Index.X < 0 || Index.X >= MergeFieldSize.X)
		return;

	if (Index.Y < 0 || Index.Y >= MergeFieldSize.Y)
		return;

	MergeField[Index.Y][Index.X] = Item;
}

bool UMergeSubsystem::TryMergeItems(const FMergeFieldItem& Item, const FIntPoint& MergeIndex, FMergeFieldItem& MergedItem)
{
	FMergeFieldItem MergeItem;
	if (!GetItemAt(MergeIndex, MergeItem))
		return false;

	// if this is different items
	if (Item != MergeItem)
		return false;

	FString RowName = UMBUtilityFunctionLibrary::EnumToString("EMergeItemType", (int32)Item.Type);

	const FMergeItemChainRow* RowStruct = MergeItemsDataTable->FindRow<FMergeItemChainRow>(FName(RowName), "UMergeSubsystem::TryMergeItems()");

	if (!RowStruct)
	{
		UE_LOG(LogTemp, Error, TEXT("UMergeSubsystem::TryMergeItems() - No item with type %s in table"), *RowName);
		return false;
	}

	// if this is a last level item
	if (RowStruct->ItemsChain.Num() <= Item.Level)
		return false;

	MergedItem.Level = Item.Level + 1;
	MergedItem.Type = Item.Type;

	MergeField[MergeIndex.Y][MergeIndex.X] = MergedItem;

	return true;
}

bool UMergeSubsystem::GetClosestFreeIndex(const FIntPoint& Index, FIntPoint& ClosestFreeIndex)
{
	FMergeFieldItem Item;
	if (!GetItemAt(Index, Item))
	{
		ClosestFreeIndex = Index;
		return true;
	}

	for (int32 i = 1; i <= (MergeFieldSize.X + MergeFieldSize.Y - 2); i++)
	{
		TArray<FIntPoint> Variants;
		GetAllIndexVariants(i, Variants);

		for (const auto& Variant : Variants)
		{
			FIntPoint IndexToCheck = Index + Variant;

			if (IndexToCheck.X < 0 || IndexToCheck.X >= MergeFieldSize.X)
				continue;

			if (IndexToCheck.Y < 0 || IndexToCheck.Y >= MergeFieldSize.Y)
				continue;

			if (GetItemAt(IndexToCheck, Item))
				continue;

			ClosestFreeIndex = IndexToCheck;
			return true;
		}
	}
	return false;
}

void UMergeSubsystem::GetAllIndexVariants(int32 IndexSum, TArray<FIntPoint>& Variants)
{
	for (int32 i = -IndexSum; i <= IndexSum; i++)
	{
		for (int32 j = -IndexSum; j <= IndexSum; j++)
		{
			if ((FMath::Abs(i) + FMath::Abs(j)) != IndexSum)
				continue;

			Variants.Add(FIntPoint(j, i));
		}
	}
}

void UMergeSubsystem::GetRandomItemWeight(const TArray<FSpawnItemData>& Items, FSpawnItemData& OutItem)
{
	check(Items.Num() > 0);

	// get all types
	TMap<ESpawnProbability, TArray<FSpawnItemData>> ProbabilityTypes;
	for (const auto& Item : Items)
	{
		if (ProbabilityTypes.Contains(Item.Probability))
		{
			ProbabilityTypes[Item.Probability].Add(Item);
		}
		else
		{
			TArray<FSpawnItemData> ItemArray;
			ItemArray.Add(Item);
			ProbabilityTypes.Add(Item.Probability, ItemArray);
		}
	}

	// get sum of all weights
	int32 WeightSum = 0;
	for (const auto& Probability : ProbabilityTypes)
	{
		WeightSum += GetWeightForProbability(Probability.Key);
	}
	check(WeightSum > 0);

	int32 RandNumber = FMath::RandRange(1, WeightSum);

	int32 PassedWeight = 0;
	
	// get weight randow for probability type
	TArray<FSpawnItemData> SelectedProbabilityItems;
	for (const auto& Probability : ProbabilityTypes)
	{
		PassedWeight += GetWeightForProbability(Probability.Key);
		if (RandNumber <= PassedWeight)
		{
			SelectedProbabilityItems = Probability.Value;
			break;
		}
	}

	// get equal random in probability type group
	int32 RandIndex = FMath::RandRange(0, SelectedProbabilityItems.Num() - 1);

	OutItem = SelectedProbabilityItems[RandIndex];
}

int32 UMergeSubsystem::GetWeightForProbability(ESpawnProbability Probability)
{
	switch (Probability)
	{
	case ESpawnProbability::Rarely:
		return 1;
	case ESpawnProbability::Often:
		return 9;
	}

	return 0;
}

bool UMergeSubsystem::HasFreePlace()
{
	for (const auto& Row : MergeField)
	{
		for (const auto& Item : Row)
		{
			if (Item.Type == EMergeItemType::None)
				return true;
		}
	}

	return false;
}

int32 UMergeSubsystem::DecrementRemainItemsToSpawn(const FIntPoint& Index)
{
	MergeField[Index.Y][Index.X].RemainItemsToSpawn--;

	return MergeField[Index.Y][Index.X].RemainItemsToSpawn;
}

void UMergeSubsystem::InitItem(FMergeFieldItem& OutItem)
{
	FString RowName = UMBUtilityFunctionLibrary::EnumToString("EMergeItemType", (int32)OutItem.Type);
	const FMergeItemChainRow* RowStruct = MergeItemsDataTable->FindRow<FMergeItemChainRow>(FName(RowName), "UMergeSubsystem::InitItem()");
	
	if (!RowStruct)
		return;

	if (OutItem.RemainItemsToSpawn <= 0)
		OutItem.RemainItemsToSpawn = RowStruct->ItemsChain[OutItem.Level - 1].MaxItemsToSpawn;
}

bool UMergeSubsystem::GetFirstReward(FMergeFieldItem& OutItem)
{
	if (RewardsQueue.Num() == 0)
		return false;

	OutItem = RewardsQueue[0];

	return true;
}

void UMergeSubsystem::RemoveFirstReward()
{
	RewardsQueue.RemoveAt(0);
}

void UMergeSubsystem::AddNewReward(const FMergeFieldItem& NewRewardItem)
{
	RewardsQueue.Add(NewRewardItem);

	OnGetReward.Broadcast();
}

int32 UMergeSubsystem::GetItemTotalCount(const FMergeFieldItem& Item)
{
	int32 Count = 0;

	for (const auto& Row : MergeField)
	{
		for (const auto& RowItem : Row)
		{
			if (Item == RowItem)
				Count++;
		}
	}

	return Count;
}

void UMergeSubsystem::SpendItems(const FMergeFieldItem& Item, int32 Count)
{
	if (Count == 0)
		return;

	for (auto& Row : MergeField)
	{
		for (auto& RowItem : Row)
		{
			if (Item == RowItem)
			{
				RowItem.Type = EMergeItemType::None;
				Count--;

				if (Count == 0)
					return;
			}
		}
	}
}
