// Fill out your copyright notice in the Description page of Project Settings.


#include "MergeField/MBBaseMergeItemActor.h"
#include "Kismet/GameplayStatics.h"
#include "MergeField/MBMergeFieldManager.h"
#include "User/AccountSubsystem.h"

// Sets default values
AMBBaseMergeItemActor::AMBBaseMergeItemActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Root = CreateDefaultSubobject<USceneComponent>(FName("Root"));
	SetRootComponent(Root);

	auto AttachmentRules = FAttachmentTransformRules(EAttachmentRule::KeepRelative, false);
	Sprite = CreateDefaultSubobject<UPaperSpriteComponent>(FName("Sprite"));
	Sprite->AttachToComponent(Root, AttachmentRules);
}

// Called when the game starts or when spawned
void AMBBaseMergeItemActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMBBaseMergeItemActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMBBaseMergeItemActor::Initialize(const FMergeFieldItem& InBaseData, const FMergeItemData& InTableData, const FIntPoint& Index)
{
	BaseData = InBaseData;
	TableData = InTableData;
	FieldIndex = Index;

	InitVisual();
}

void AMBBaseMergeItemActor::HandleInteraction()
{
	if (!TableData.Interactable)
		return;

	if (TableData.InteractType == EItemInteractType::None)
		return;

	auto AccountSystem = GetGameInstance()->GetSubsystem<UAccountSubsystem>();
	
	if (!AccountSystem->HasEnoughEnergy(1))
		return;

	bool Result = false;
	switch (TableData.InteractType)
	{
	case EItemInteractType::SpawnItem:
	{
		Result = GenerateNewItem();
		break;
	}
	case EItemInteractType::AddValue:
	{
		AddConsumableValue();
			Result = true;
		break;
	}
	default:
		break;
	}

	if (Result)
	{
		AccountSystem->SpendEnergy(TableData.EnergyConsume);
	}
}

bool AMBBaseMergeItemActor::GenerateNewItem()
{
	auto FieldManager = Cast<AMBMergeFieldManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AMBMergeFieldManager::StaticClass()));

	return FieldManager->GenerateNewItemFromAnother(this);
}

void AMBBaseMergeItemActor::AddConsumableValue()
{
	auto AccountSystem = GetGameInstance()->GetSubsystem<UAccountSubsystem>();

	switch (TableData.AddValueType)
	{
	case EConsumableParamType::SoftCoin:
	{
		AccountSystem->AddSoftCoins(TableData.AddValueCount);
		break;
	}
	case EConsumableParamType::PremCoin:
	{
		AccountSystem->AddPremCoins(TableData.AddValueCount);
		break;
	}
	case EConsumableParamType::Energy:
	{
		AccountSystem->AddEnergy(TableData.AddValueCount);
		break;
	}
	case EConsumableParamType::Experience:
	{
		AccountSystem->AddExperience(TableData.AddValueCount);
		break;
	}
	}

	auto FieldManager = Cast<AMBMergeFieldManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AMBMergeFieldManager::StaticClass()));

	FieldManager->DestroyItem(FieldIndex);
}
