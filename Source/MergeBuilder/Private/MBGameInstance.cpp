// Fill out your copyright notice in the Description page of Project Settings.


#include "MBGameInstance.h"

#include "MBBasePlayerController.h"
#include "User/AccountSubsystem.h"
#include "MergeSubsystem.h"
#include "TimeSubsystem.h"
#include "CitySystem/CityBuilderSubsystem.h"
#include "Blueprint/UserWidget.h"

UMBGameInstance::UMBGameInstance()
{
	static ConstructorHelpers::FClassFinder<UShopSubsystem> ShopSubsystemClass(TEXT("/Game/Development/Logic/Utilities/BP_ShopSubsystem.BP_ShopSubsystem_C"));
	if (ShopSubsystemClass.Succeeded())
	{
		ShopSubsystem = NewObject<UShopSubsystem>(this, ShopSubsystemClass.Class, FName("ShopSubsystem"));
	}
	
	check(ShopSubsystem);

	static ConstructorHelpers::FClassFinder<UMBTutorialSubsystem> TutorialSubsystemClass(TEXT("/Game/Development/Logic/BP_TutorialSubsystem.BP_TutorialSubsystem_C"));
	if (TutorialSubsystemClass.Succeeded())
	{
		TutorialSubsystem = NewObject<UMBTutorialSubsystem>(this, TutorialSubsystemClass.Class, FName("TutorialSubsystem"));
	}
	
	check(TutorialSubsystem);
}

void UMBGameInstance::Init()
{
	Super::Init();

	ShopSubsystem->Init();

	TutorialSubsystem->Init();
	
	ApplicationWillEnterBackgroundDelegate.AddDynamic(this, &UMBGameInstance::SaveAllData);
	
	auto TimeSubsystem = GetSubsystem<UTimeSubsystem>();
	TimeSubsystem->OnTimeSuccessRequested.AddUObject(this, &UMBGameInstance::CheckAllDataLoaded);

	FTimerHandle TimerHandle;
	GetTimerManager().SetTimer(TimerHandle, this, &UMBGameInstance::CheckAllDataLoaded, 3.0f, false);
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

void UMBGameInstance::CheckAllDataLoaded()
{
	auto PC = Cast<AMBBasePlayerController>(GetFirstLocalPlayerController());
	
	if (!IsValid(PC->LoadingScreen))
		return;
	
	if (GetWorld()->TimeSeconds < 3.0f)
		return;

	auto TimeSubsystem = GetSubsystem<UTimeSubsystem>();
	if (!TimeSubsystem->IsTimeValid())
		return;

	PC->LoadingScreen->RemoveFromParent();

	OnGameLoaded.Broadcast();
}
