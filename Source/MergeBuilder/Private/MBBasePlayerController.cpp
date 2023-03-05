// Fill out your copyright notice in the Description page of Project Settings.


#include "MBBasePlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraActor.h"
#include "MergeField/MBMergeFieldPawn.h"
#include "TopDownPawn.h"

void AMBBasePlayerController::BeginPlay()
{
	Super::BeginPlay();

	MergeFieldPawn = Cast<AMBMergeFieldPawn>(UGameplayStatics::GetActorOfClass(GetWorld(), AMBMergeFieldPawn::StaticClass()));
	check(MergeFieldPawn);

	TopDownPawn = Cast<ATopDownPawn>(UGameplayStatics::GetActorOfClass(GetWorld(), ATopDownPawn::StaticClass()));
	check(TopDownPawn);
}

void AMBBasePlayerController::SwitchToMergeField()
{
	Possess(MergeFieldPawn);
}

void AMBBasePlayerController::SwitchToCity()
{
	Possess(TopDownPawn);
}

void AMBBasePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}