// Fill out your copyright notice in the Description page of Project Settings.


#include "MergeField/MBMergeFieldManager.h"
#include "MergeSubsystem.h"
#include "MBUtilityFunctionLibrary.h"
#include "User/AccountSubsystem.h"

// Sets default values
AMBMergeFieldManager::AMBMergeFieldManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TArray<AMBBaseMergeItemActor*> ZeroRow;
	ZeroRow.SetNumZeroed(MergeFieldSize.X);
	FieldItems.Init(ZeroRow, MergeFieldSize.Y);
}

// Called when the game starts or when spawned
void AMBMergeFieldManager::BeginPlay()
{
	Super::BeginPlay();

	auto MergeSystem = GetGameInstance()->GetSubsystem<UMergeSubsystem>();
	MergeSystem->OnGetReward.AddDynamic(this, &AMBMergeFieldManager::InitRewardItem);
}

// Called every frame
void AMBMergeFieldManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMBMergeFieldManager::InitializeField()
{
	DestroyAllItems();

	auto MergeSystem = GetGameInstance()->GetSubsystem<UMergeSubsystem>();

	check(MergeSystem);
	check(MergeItemsDataTable);
	check(SpawnItemsClass);

	for (int32 y = 0; y < MergeFieldSize.Y; y++)
	{
		for (int32 x = 0; x < MergeFieldSize.X; x++)
		{
			FIntPoint ID = FIntPoint(x, y);
			FMergeFieldItem Item;
			if (!MergeSystem->GetItemAt(ID, Item))
				continue;

			SpawnItemAtIndex(Item, ID);
		}
	}

	InitRewardItem();

}

void AMBMergeFieldManager::InitRewardItem()
{
	auto MergeSystem = GetGameInstance()->GetSubsystem<UMergeSubsystem>();

	FMergeFieldItem RewardItem;
	if (MergeSystem->GetFirstReward(RewardItem))
	{
		FString RowName = UMBUtilityFunctionLibrary::EnumToString("EMergeItemType", (int32)RewardItem.Type);

		const FMergeItemChainRow* RowStruct = MergeItemsDataTable->FindRow<FMergeItemChainRow>(FName(RowName), "AMBMergeFieldManager::InitRewardItem()");

		RewardActor->Initialize(RewardItem, RowStruct->ItemsChain[RewardItem.Level - 1], FIntPoint(-1, -1));
		RewardActor->SetActorHiddenInGame(false);
	}
	else
	{
		RewardActor->SetActorHiddenInGame(true);
	}
}

void AMBMergeFieldManager::OpenInBoxItems(const TArray<FIntPoint>& Indexes)
{
	auto MergeSubsystem = GetGameInstance()->GetSubsystem<UMergeSubsystem>();

	for (const FIntPoint& Index : Indexes)
	{
		FMergeFieldItem Item;
		MergeSubsystem->OpenInBoxItem(Index, Item);

		auto ItemAtIndex = FieldItems[Index.Y][Index.X];
		ItemAtIndex->Destroy();

		auto SpawnedItemActor = SpawnItemAtIndex(Item, Index);
	}
}

void AMBMergeFieldManager::SellItem(AMBBaseMergeItemActor* ItemToSell)
{
	auto AccountSubsystem = GetGameInstance()->GetSubsystem<UAccountSubsystem>();
	AccountSubsystem->AddSoftCoins(ItemToSell->TableData.SellPrice);
	
	DestroyItem(ItemToSell->FieldIndex);
	DeselectCurrentIndex();

	auto MergeSubsystem = GetGameInstance()->GetSubsystem<UMergeSubsystem>();
	MergeSubsystem->SaveField();
}

AMBBaseMergeItemActor* AMBMergeFieldManager::SpawnItemAtIndex(const FMergeFieldItem& Item, const FIntPoint& Index)
{
	FVector Location;
	GetLocationForIndex(Index, Location);

	FTransform SpawnTransform = FTransform::Identity;
	SpawnTransform.SetLocation(Location);

	auto SpawnedItem = GetWorld()->SpawnActor<AMBBaseMergeItemActor>(SpawnItemsClass, SpawnTransform);

	FString RowName = UMBUtilityFunctionLibrary::EnumToString("EMergeItemType", (int32)Item.Type);

	const FMergeItemChainRow* RowStruct = MergeItemsDataTable->FindRow<FMergeItemChainRow>(FName(RowName), "AMBMergeFieldManager::SpawnItemAtIndex()");

	if (!RowStruct)
	{
		UE_LOG(LogTemp, Error, TEXT("AMBMergeFieldManager::SpawnItemAtIndex() - No item with type %s in table"), *RowName);
		return nullptr;
	}

	if (RowStruct->ItemsChain.Num() < Item.Level)
	{
		UE_LOG(LogTemp, Error, TEXT("AMBMergeFieldManager::SpawnItemAtIndex() - Item with type %s has no level %d"), *RowName, Item.Level);
		return nullptr;
	}

	FMergeItemData ItemData = RowStruct->ItemsChain[Item.Level - 1];

	SpawnedItem->Initialize(Item, ItemData, Index);

	FieldItems[Index.Y][Index.X] = SpawnedItem;

	return SpawnedItem;
}

void AMBMergeFieldManager::DestroyAllItems()
{
	for (auto& Row : FieldItems)
	{
		for (auto& Item : Row)
		{
			if (!IsValid(Item))
				continue;

			Item->Destroy();
			Item = nullptr;
		}
	}

	if (SelectionActor)
	{
		SelectionActor->Destroy();
		SelectionActor = nullptr;
	}
}

bool AMBMergeFieldManager::GetLocationForIndex(const FIntPoint& Index, FVector& OutLocation)
{
	FVector ZeroTileLocation = FVector(MergeFieldSize.X - 1, MergeFieldSize.Y - 1, 0) * (TileSize) / 2.0f;

	OutLocation = ZeroTileLocation - (FVector(Index.X, Index.Y, 0) * TileSize);

	OutLocation.Z = LocationFieldZ;

	return true;
}

bool AMBMergeFieldManager::GetIndexForLocation(const FVector& Location, FIntPoint& OutIndex)
{
	FVector ZeroTileLocation = FVector(MergeFieldSize.X - 1, MergeFieldSize.Y - 1, 0) * (TileSize) / 2.0f;
	ZeroTileLocation += (FVector(TileSize, TileSize, 0.0f) / 2.0f);

	FVector ZeroOffset = ZeroTileLocation - Location;
	
	OutIndex = FIntPoint(FMath::TruncToInt(ZeroOffset.X / TileSize), FMath::TruncToInt(ZeroOffset.Y / TileSize));

	OutIndex = FIntPoint(FMath::Clamp(OutIndex.X, 0, MergeFieldSize.X - 1), FMath::Clamp(OutIndex.Y, 0, MergeFieldSize.Y - 1));
	
	return true;
}

bool AMBMergeFieldManager::IsLocationOnField(const FVector& Location)
{
	FVector ZeroTileLocation, LastTileLocation;
	GetLocationForIndex(FIntPoint(0,0), ZeroTileLocation);
	GetLocationForIndex(FIntPoint(MergeFieldSize.X - 1,MergeFieldSize.Y - 1), LastTileLocation);

	ZeroTileLocation += (FVector(TileSize, TileSize, 0.0f) / 2.0f);
	LastTileLocation -= (FVector(TileSize, TileSize, 0.0f) / 2.0f);

	if (Location.X > ZeroTileLocation.X)
		return false;

	if (Location.Y > ZeroTileLocation.Y)
		return false;

	if (Location.X < LastTileLocation.X)
		return false;

	if (Location.Y < LastTileLocation.Y)
		return false;

	return true;
}

void AMBMergeFieldManager::HandleStartTouchOnIndex(const FIntPoint& Index)
{
	TouchStartItem = GetItemAtIndex(Index);
}

void AMBMergeFieldManager::HandleReleaseTouchOnIndex(const FIntPoint& Index)
{
	if (!TouchStartItem)
		return;

	if (InDrag)
	{
		auto MergeSystem = GetGameInstance()->GetSubsystem<UMergeSubsystem>();

		auto ItemAtIndex = GetItemAtIndex(Index);

		if (ItemAtIndex)
		{
			FMergeFieldItem MergedItem;
			if (MergeSystem->TryMergeItems(TouchStartItem->BaseData, Index, MergedItem))
			{
				bool MergeWithDusty = ItemAtIndex->BaseData.IsDusty;
				FVector MergeItemLocation = (TouchStartItem->GetActorLocation() + ItemAtIndex->GetActorLocation()) / 2.0f;
				TouchStartItem->Destroy();
				ItemAtIndex->Destroy();

				auto MergedItemActor = SpawnItemAtIndex(MergedItem, Index);
				MergedItemActor->SetActorLocation(MergeItemLocation);
				FVector IndexLocation;
				GetLocationForIndex(Index, IndexLocation);
				MergedItemActor->MoveToLocation(IndexLocation);

				// if merge with item that is dusty
				if (MergeWithDusty)
				{
					// check if there are items in the box around
					TArray<FIntPoint> InBoxIndexes;
					if (MergeSystem->GetAllItemsInBoxAround(Index, InBoxIndexes))
					{
						OpenInBoxItems(InBoxIndexes);
					}
				}
			}
			else
			{
				if (ItemAtIndex->BaseData.IsDusty || ItemAtIndex->BaseData.IsInBox)
				{
					// return current item closest location

					FIntPoint ClosestFreeIndex;
					if (!MergeSystem->GetClosestFreeIndex(Index, ClosestFreeIndex))
					{
						check(nullptr);
					}

					PlaceItemAtIndex(ClosestFreeIndex, TouchStartItem);
				}
				else
				{
					// place current item at this location and move other on closest free place

					FIntPoint ClosestFreeIndex;
					if (!MergeSystem->GetClosestFreeIndex(Index, ClosestFreeIndex))
					{
						check(nullptr);
					}

					PlaceItemAtIndex(ClosestFreeIndex, ItemAtIndex);
					PlaceItemAtIndex(Index, TouchStartItem);
				}
			}
		}
		else
		{
			// Place item at the free place

			PlaceItemAtIndex(Index, TouchStartItem);
		}

		SelectIndex(Index);

		MergeSystem->SaveField();
	}

	InDrag = false;
}

void AMBMergeFieldManager::HandleClickOnIndex(const FIntPoint& Index)
{
	if (InDrag)
		return;

	if (Index == SelectedIndex)
	{
		auto ItemAtIndex = GetItemAtIndex(Index);
		if (ItemAtIndex)
		{
			ItemAtIndex->HandleInteraction();

			auto MergeSubsystem = GetGameInstance()->GetSubsystem<UMergeSubsystem>();
			MergeSubsystem->SaveField();
		}
		else
		{
			DeselectCurrentIndex();
		}
	}
	else
	{
		SelectIndex(Index);
	}
}

void AMBMergeFieldManager::HandleClickOnReward()
{
	auto MergeSystem = GetGameInstance()->GetSubsystem<UMergeSubsystem>();

	FMergeFieldItem RewardItem;
	if (!MergeSystem->GetFirstReward(RewardItem))
		return;

	if (!MergeSystem->HasFreePlace())
	{
		// TODO: Add failed sound/anim
		return;
	}

	GenerateNewItemFromLocation(FIntPoint(3, 8), RewardActor->GetActorLocation() + FVector(0, 32.0f, 0), RewardItem);

	MergeSystem->RemoveFirstReward();

	InitRewardItem();

	MergeSystem->SaveField();
}

void AMBMergeFieldManager::StartDrag()
{
	if (!TouchStartItem)
		return;

	if (TouchStartItem->BaseData.IsDusty || TouchStartItem->BaseData.IsInBox)
		return;

	InDrag = true;

	DeselectCurrentIndex();

	FIntPoint DraggedIndex = TouchStartItem->GetFieldIndex();

	FieldItems[DraggedIndex.Y][DraggedIndex.X] = nullptr;
	
	auto MergeSystem = GetGameInstance()->GetSubsystem<UMergeSubsystem>();
	FMergeFieldItem VoidItem = FMergeFieldItem();
	MergeSystem->SetItemAt(DraggedIndex, VoidItem);
}

void AMBMergeFieldManager::HandleDrag(const FVector& FieldLocation)
{
	if (!TouchStartItem)
		return;

	if (!InDrag)
		return;

	FVector ZeroTileLocation;
	GetLocationForIndex(FIntPoint(0,0), ZeroTileLocation);
	FVector NewLocation = FieldLocation;
	NewLocation.Y = FMath::Min(NewLocation.Y, ZeroTileLocation.Y);

	TouchStartItem->SetActorLocation(NewLocation + FVector(0.0f, 0.0f, 15.0f));
}

AMBBaseMergeItemActor* AMBMergeFieldManager::GetItemAtIndex(const FIntPoint& Index)
{
	if (Index.X < 0 || Index.X >= MergeFieldSize.X)
		return nullptr;

	if (Index.Y < 0 || Index.Y >= MergeFieldSize.Y)
		return nullptr;

	return FieldItems[Index.Y][Index.X];
}

void AMBMergeFieldManager::PlaceItemAtIndex(const FIntPoint& Index, AMBBaseMergeItemActor* Item)
{
	auto MergeSystem = GetGameInstance()->GetSubsystem<UMergeSubsystem>();

	FVector IndexLocation;
	GetLocationForIndex(Index, IndexLocation);
	Item->MoveToLocation(IndexLocation);

	FieldItems[Index.Y][Index.X] = Item;
	Item->FieldIndex = Index;

	MergeSystem->SetItemAt(Index, Item->BaseData);
}

void AMBMergeFieldManager::SelectIndex(const FIntPoint& Index)
{
	DeselectCurrentIndex();

	auto ActorAtIndex = GetItemAtIndex(Index);
	if (!ActorAtIndex)
		return;

	if (ActorAtIndex->BaseData.IsInBox)
		return;

	SelectedIndex = Index;

	FVector IndexLocation;
	GetLocationForIndex(Index, IndexLocation);
	IndexLocation.Z -= 0.5f;
	FTransform SpawnTransform = FTransform::Identity;
	SpawnTransform.SetLocation(IndexLocation);
	SelectionActor = GetWorld()->SpawnActor<AActor>(SelectionActorClass, SpawnTransform);
	
	OnItemSelected.Broadcast(ActorAtIndex);
}

void AMBMergeFieldManager::DeselectCurrentIndex()
{
	SelectedIndex = FIntPoint(-1, -1);

	if (SelectionActor)
	{
		SelectionActor->Destroy();
		SelectionActor = nullptr;
		OnItemDeselected.Broadcast();
	}
}

void AMBMergeFieldManager::DestroyItem(const FIntPoint& Index)
{
	auto MergeSystem = GetGameInstance()->GetSubsystem<UMergeSubsystem>();
	FMergeFieldItem VoidItem = FMergeFieldItem();
	MergeSystem->SetItemAt(Index, VoidItem);

	auto Item = GetItemAtIndex(Index);

	Item->Destroy();

	FieldItems[Index.Y][Index.X] = nullptr;
}

bool AMBMergeFieldManager::GenerateNewItemFromAnother(AMBBaseMergeItemActor* SourceItem)
{
	auto MergeSystem = GetGameInstance()->GetSubsystem<UMergeSubsystem>();

	if (!MergeSystem->HasFreePlace())
	{
		// TODO: Handle situation if generator has last charge and we can spawn item instead of it
		// TODO: Add failed sound/anim
		return false;
	}

	FSpawnItemData ItemToSpawn;
	MergeSystem->GetRandomItemWeight(SourceItem->TableData.SpawnableItems, ItemToSpawn);
	MergeSystem->InitItem(ItemToSpawn.Item);

	GenerateNewItemFromLocation(SourceItem->FieldIndex, ItemToSpawn.Item);

	int32 RemainItems = MergeSystem->DecrementRemainItemsToSpawn(SourceItem->FieldIndex);
	MergeSystem->GetItemAt(SourceItem->FieldIndex, SourceItem->BaseData);
	
	if (RemainItems <= 0)
	{
		DestroyItem(SourceItem->FieldIndex);
		DeselectCurrentIndex();
	}
	else
	{
		OnItemUpdated.Broadcast(SourceItem);
	}
	
	return true;
}

AMBBaseMergeItemActor* AMBMergeFieldManager::GenerateNewItemFromLocation(const FIntPoint& SourceIndex, const FMergeFieldItem& NewItem)
{
	FVector SourceLocation;
	GetLocationForIndex(SourceIndex, SourceLocation);

	return GenerateNewItemFromLocation(SourceIndex, SourceLocation, NewItem);
}

AMBBaseMergeItemActor* AMBMergeFieldManager::GenerateNewItemFromLocation(const FIntPoint& SourceIndex, const FVector& SourceLocation, const FMergeFieldItem& NewItem)
{
	auto MergeSystem = GetGameInstance()->GetSubsystem<UMergeSubsystem>();
	FMergeFieldItem ItemToSpawn = NewItem;
	MergeSystem->InitItem(ItemToSpawn);

	FIntPoint ClosestIndex;
	MergeSystem->GetClosestFreeIndex(SourceIndex, ClosestIndex);

	auto SpawnedItem = SpawnItemAtIndex(ItemToSpawn, ClosestIndex);

	SpawnedItem->SetActorLocation(SourceLocation);
	FVector DestinationLocation;
	GetLocationForIndex(ClosestIndex, DestinationLocation);
	SpawnedItem->MoveToLocation(DestinationLocation);
	SpawnedItem->PlayAppearingAnimation();

	MergeSystem->SetItemAt(ClosestIndex, ItemToSpawn);

	return SpawnedItem;
}
