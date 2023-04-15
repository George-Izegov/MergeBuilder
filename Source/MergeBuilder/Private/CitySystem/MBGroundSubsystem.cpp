// Fill out your copyright notice in the Description page of Project Settings.


#include "CitySystem/MBGroundSubsystem.h"

#include "JsonObjectConverter.h"
#include "MBUtilityFunctionLibrary.h"

UMBGroundSubsystem::UMBGroundSubsystem()
{
	GroundFieldSize = FIntPoint(10, 10);

	static ConstructorHelpers::FObjectFinder<UDataTable> ItemsDataTable(TEXT("DataTable'/Game/Development/DataTables/GroundTilesInfo.GroundTilesInfo'"));
	if (ItemsDataTable.Succeeded())
	{
		PossibleGroundTilesDataTable = ItemsDataTable.Object;
	}

	check(PossibleGroundTilesDataTable);
}

void UMBGroundSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	InitGroundField();

	CalculateBoundingSquare();
}

void UMBGroundSubsystem::Deinitialize()
{
	Super::Deinitialize();

	SaveGround();
}

void UMBGroundSubsystem::SaveGround()
{
	TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();

	TSharedPtr<FJsonObject> FieldObject = MakeShared<FJsonObject>();

	for (int32 i = -GroundFieldSize.Y/2; i < GroundFieldSize.Y/2; i++)
	{
		TSharedPtr<FJsonObject> RowObject = MakeShared<FJsonObject>();

		for (int32 j = -GroundFieldSize.X/2; j < GroundFieldSize.X/2; j++)
		{
			FMBGroundTile Tile = GroundField[i + GroundFieldSize.Y/2][j + GroundFieldSize.X/2];
			if (Tile.IsVoid)
				continue;

			TSharedPtr<FJsonObject> ItemObject;

			ItemObject = FJsonObjectConverter::UStructToJsonObject<FMBGroundTile>(Tile);

			RowObject->SetObjectField(FString::FromInt(j), ItemObject);
		}

		FieldObject->SetObjectField(FString::FromInt(i), RowObject);
	}

	JsonObject->SetObjectField("field", FieldObject);

	FString StringData;
	UMBUtilityFunctionLibrary::JsonObjectToString(JsonObject, StringData);

	UMBUtilityFunctionLibrary::SaveToStorage("GroundField", StringData);
}

void UMBGroundSubsystem::GetAllPossibleGroundTiles(TArray<FMBGroundTile>& OutGroundTiles)
{
	// get all void tiles near not void tiles
	for (const auto& Row : GroundField)
	{
		for (const auto& Tile : Row)
		{
			if (Tile.IsVoid)
				continue;

			for (int32 i = -1; i < 2; i++)
			{
				for (int32 j = -1; j < 2; j++)
				{
					if (FMath::Abs(i) + FMath::Abs(j) != 1)
						continue;

					FIntPoint TileIndex = Tile.Index + FIntPoint(i, j);

					if (TileIndex.X < 0 || TileIndex.Y < 0 || TileIndex.X >= GroundFieldSize.X || TileIndex.Y >= GroundFieldSize.Y)
						continue;

					FMBGroundTile NearTile = GroundField[TileIndex.Y][TileIndex.X];

					if (!NearTile.IsVoid)
						continue;

					OutGroundTiles.AddUnique(NearTile);
				}
			}
		}
	}
}

void UMBGroundSubsystem::GetNeighborTilesForIndex(const FIntPoint& Index, TArray<FMBGroundTile>& OutGroundTiles)
{
	for (int32 i = -1; i < 2; i++)
	{
		for (int32 j = -1; j < 2; j++)
		{
			if (FMath::Abs(i) + FMath::Abs(j) != 1)
				continue;

			FIntPoint TileIndex = Index + FIntPoint(i, j);

			FMBGroundTile NearTile;
			if (!GetGroundTile(TileIndex, NearTile))
				continue;

			OutGroundTiles.AddUnique(NearTile);
		}
	}
}

void UMBGroundSubsystem::AddNewTile(const FIntPoint& Index)
{
	if (Index.X < 0 || Index.Y < 0 || Index.X >= GroundFieldSize.X || Index.Y >= GroundFieldSize.Y)
		return;

	FMBGroundTile NewTile;
	NewTile.IsVoid = false;
	NewTile.Index = Index;

	GroundField[Index.Y][Index.X] = NewTile;

	SaveGround();

	CalculateBoundingSquare();
}

bool UMBGroundSubsystem::GetGroundTile(const FIntPoint& Index, FMBGroundTile& OutGroundTile)
{
	if (Index.X < 0 || Index.Y < 0 || Index.X >= GroundFieldSize.X || Index.Y >= GroundFieldSize.Y)
		return false;

	OutGroundTile = GroundField[Index.Y][Index.X];

	return !OutGroundTile.IsVoid;
}

void UMBGroundSubsystem::GetGroundTileInfo(const FIntPoint& Index, FMBPossibleGroundTileInfo& OutGroundTileInfo)
{
	FName RowName = FName(FString::FromInt(Index.X) + "_" + FString::FromInt(Index.Y));

	FMBPossibleGroundTileInfo* RowStruct = PossibleGroundTilesDataTable->FindRow<FMBPossibleGroundTileInfo>(RowName, "");

	if (!RowStruct)
	{
		RowStruct = PossibleGroundTilesDataTable->FindRow<FMBPossibleGroundTileInfo>("0_0", "");
		check(RowStruct);
	}

	OutGroundTileInfo = *RowStruct;
}

void UMBGroundSubsystem::GetBoundingSquare(FVector& OutMinBoundingLocation, FVector& OutMaxBoundingLocation)
{
	OutMinBoundingLocation = MinBoundingLocation;
	OutMaxBoundingLocation = MaxBoundingLocation;
}

void UMBGroundSubsystem::CalculateBoundingSquare()
{
	FIntPoint Min = FIntPoint(GroundFieldSize.X/2, GroundFieldSize.Y/2);
	FIntPoint Max = FIntPoint(-GroundFieldSize.X/2, -GroundFieldSize.Y/2);

	for (int32 i = -GroundFieldSize.Y/2; i < GroundFieldSize.Y/2; i++)
	{
		for (int32 j = -GroundFieldSize.X/2; j < GroundFieldSize.X/2; j++)
		{
			FIntPoint TileIndex = FIntPoint(j + GroundFieldSize.X/2, i + GroundFieldSize.Y/2);

			FMBGroundTile Tile;
			if (!GetGroundTile(TileIndex, Tile))
				continue;

			if (Tile.IsVoid)
				continue;

			Min.X = FMath::Min(Min.X, j);
			Min.Y = FMath::Min(Min.Y, i);
			Max.X = FMath::Max(Max.X, j);
			Max.Y = FMath::Max(Max.Y, i);
		}
	}

	MaxBoundingLocation = FVector(Max.X + 1, Max.Y + 1, 0);
	MaxBoundingLocation *= 3000.0f;

	MinBoundingLocation = FVector(Min.X - 1, Min.Y - 1, 0);
	MinBoundingLocation *= 3000.0f;
}

void UMBGroundSubsystem::InitGroundField()
{
	TArray<FMBGroundTile> ZeroRow;
	ZeroRow.SetNumZeroed(GroundFieldSize.X);

	GroundField.Init(ZeroRow, GroundFieldSize.Y);

	for (int32 i = 0; i < GroundFieldSize.Y; i++)
	{
		for (int32 j = 0; j < GroundFieldSize.X; j++)
		{
			GroundField[i][j].IsVoid = true;
			GroundField[i][j].Index = FIntPoint(j, i);
		}
	}
	
	FString SavedData;
	if (UMBUtilityFunctionLibrary::ReadFromStorage("GroundField", SavedData))
	{
		ParseGround(SavedData);
	}
	else
	{
		InitStartGroundField();
	}
}

void UMBGroundSubsystem::InitStartGroundField()
{
	FMBGroundTile NotNullTile;
	NotNullTile.IsVoid = false;

	NotNullTile.Index = FIntPoint(GroundFieldSize.X/2, GroundFieldSize.Y/2);
	GroundField[NotNullTile.Index.Y][NotNullTile.Index.X] = NotNullTile;

	NotNullTile.Index = FIntPoint(GroundFieldSize.X/2, GroundFieldSize.Y/2 - 1);
	GroundField[NotNullTile.Index.Y][NotNullTile.Index.X] = NotNullTile;

	NotNullTile.Index = FIntPoint(GroundFieldSize.X/2 - 1, GroundFieldSize.Y/2);
	GroundField[NotNullTile.Index.Y][NotNullTile.Index.X] = NotNullTile;

	NotNullTile.Index = FIntPoint(GroundFieldSize.X/2 - 1, GroundFieldSize.Y/2 - 1);
	GroundField[NotNullTile.Index.Y][NotNullTile.Index.X] = NotNullTile;
}

void UMBGroundSubsystem::ParseGround(const FString& JsonString)
{
	TSharedPtr<FJsonObject> JsonObject;
	
	if (!UMBUtilityFunctionLibrary::StringToJsonObject(JsonString, JsonObject))
		return;

	const TSharedPtr<FJsonObject>* FieldObject;

	if (JsonObject.Get()->TryGetObjectField("field", FieldObject))
	{
		for (int32 i = -GroundFieldSize.Y/2; i < GroundFieldSize.Y/2; i++)
		{
			const TSharedPtr<FJsonObject>* RowObject;
			if (!FieldObject->Get()->TryGetObjectField(FString::FromInt(i), RowObject))
				continue;

			for (int32 j = -GroundFieldSize.X/2; j < GroundFieldSize.X/2; j++)
			{
				const TSharedPtr<FJsonObject>* ItemObject;
				if (!RowObject->Get()->TryGetObjectField(FString::FromInt(j), ItemObject))
					continue;

				FMBGroundTile Tile;
				FJsonObjectConverter::JsonObjectToUStruct<FMBGroundTile>(ItemObject->ToSharedRef(), &Tile);

				Tile.Index = FIntPoint(j + GroundFieldSize.X/2, i + GroundFieldSize.Y/2);
				GroundField[Tile.Index.Y][Tile.Index.X] = Tile;
			}
		}
	}
}
