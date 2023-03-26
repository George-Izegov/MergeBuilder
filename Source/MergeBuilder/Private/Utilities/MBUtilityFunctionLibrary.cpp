// Fill out your copyright notice in the Description page of Project Settings.


#include "Utilities/MBUtilityFunctionLibrary.h"

#include "Kismet/GameplayStatics.h"
#include "Misc/FileHelper.h"

bool UMBUtilityFunctionLibrary::ReadFromStorage(const FString& StorageName, FString& OutData)
{
	FString Path = FPaths::ProjectSavedDir()+ "UserData/" + StorageName + ".json";
	return FFileHelper::LoadFileToString(OutData, *Path);
}

void UMBUtilityFunctionLibrary::SaveToStorage(const FString& StorageName, const FString& Data)
{
	FString Path = FPaths::ProjectSavedDir() + "UserData/" + StorageName + ".json";
	FFileHelper::SaveStringToFile(Data, *Path);
}

bool UMBUtilityFunctionLibrary::StringToJsonObject(const FString& JsonString, TSharedPtr<FJsonObject>& OutObject)
{
	const TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(JsonString);
	return FJsonSerializer::Deserialize(Reader, OutObject);
}

void UMBUtilityFunctionLibrary::JsonObjectToString(TSharedPtr<FJsonObject> JsonObject, FString& OutString)
{
	const auto Json_writer = TJsonWriterFactory<>::Create(&OutString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Json_writer);
}

const FString UMBUtilityFunctionLibrary::EnumToString(const FString& Enum, int32 EnumValue)
{
	const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, *Enum, true);
	if (!EnumPtr)
		return NSLOCTEXT("Invalid", "Invalid", "Invalid").ToString();

	FString Result;
#if WITH_EDITOR
	Result = EnumPtr->GetDisplayNameTextByIndex(EnumValue).ToString();
#else
	Result = EnumPtr->GetNameStringByIndex(EnumValue);
#endif

	Result.RemoveSpacesInline();
	return Result;
}

const int32 UMBUtilityFunctionLibrary::StringToEnum(const FString& Enum, const FString& EnumString)
{
	const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, *Enum, true);
	if (!EnumPtr)
		return 0;

	return EnumPtr->GetIndexByNameString(EnumString);
}

void UMBUtilityFunctionLibrary::GetMergeItemData(const FMergeFieldItem& Item, FMergeItemData& OutData)
{
	auto GI = UGameplayStatics::GetGameInstance(GEngine->GameViewport->GetWorld());
	auto MergeSubsystem = GI->GetSubsystem<UMergeSubsystem>();

	FString RowName = EnumToString("EMergeItemType", (int32)Item.Type);
	auto Row = MergeSubsystem->MergeItemsDataTable->FindRow<FMergeItemChainRow>(FName(RowName),"");

	if (!Row)
		return;
	
	OutData = Row->ItemsChain[Item.Level - 1];
}

bool UMBUtilityFunctionLibrary::IsMergeItemMaxLevel(const FMergeFieldItem& Item)
{
	auto GI = UGameplayStatics::GetGameInstance(GEngine->GameViewport->GetWorld());
	auto MergeSubsystem = GI->GetSubsystem<UMergeSubsystem>();

	FString RowName = EnumToString("EMergeItemType", (int32)Item.Type);
	auto Row = MergeSubsystem->MergeItemsDataTable->FindRow<FMergeItemChainRow>(FName(RowName),"");

	if (!Row)
		return false;

	return Row->ItemsChain.Num() == Item.Level;
}
