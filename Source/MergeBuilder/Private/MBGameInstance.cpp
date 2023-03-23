// Fill out your copyright notice in the Description page of Project Settings.


#include "MBGameInstance.h"
#include "User/AccountSubsystem.h"
#include "MergeSubsystem.h"
#include "CitySystem/CityBuilderSubsystem.h"

void UMBGameInstance::Init()
{
	Super::Init();

	ApplicationWillEnterBackgroundDelegate.AddDynamic(this, &UMBGameInstance::SaveAllData);
}

void UMBGameInstance::SaveAllData()
{
	GEngine->AddOnScreenDebugMessage(1, 3.0f, FColor::Red, FString::Printf(TEXT("SaveAllData()")));

	auto AccountSubsystem = GetSubsystem<UAccountSubsystem>();
	auto MergeSubsystem = GetSubsystem<UMergeSubsystem>();
	auto CityBuilderSubsystem = GetSubsystem<UCityBuilderSubsystem>();

	AccountSubsystem->SaveAccount();
	MergeSubsystem->SaveField();
	CityBuilderSubsystem->SaveCity();
}
