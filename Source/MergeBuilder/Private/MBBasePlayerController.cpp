// Fill out your copyright notice in the Description page of Project Settings.


#include "MBBasePlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "MergeField/MBMergeFieldPawn.h"
#include "TopDownPawn.h"
#include "Blueprint/UserWidget.h"

AMBBasePlayerController::AMBBasePlayerController()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> ScreenWidgetClass(TEXT("WidgetBlueprint'/Game/Development/Widgets/Common/W_MainLoadingScreen.W_MainLoadingScreen_C'"));
	if (ScreenWidgetClass.Succeeded())
	{
		LoadingScreenClass = ScreenWidgetClass.Class;
	}
}

void AMBBasePlayerController::BeginPlay()
{
	Super::BeginPlay();

	MergeFieldPawn = Cast<AMBMergeFieldPawn>(UGameplayStatics::GetActorOfClass(GetWorld(), AMBMergeFieldPawn::StaticClass()));
	check(MergeFieldPawn);

	TopDownPawn = Cast<ATopDownPawn>(UGameplayStatics::GetActorOfClass(GetWorld(), ATopDownPawn::StaticClass()));
	check(TopDownPawn);

	LoadingScreen = Cast<UUserWidget>(CreateWidget(this, LoadingScreenClass));

#if !WITH_EDITOR
	LoadingScreen->AddToViewport(50);
#endif
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

UUserWidget* AMBBasePlayerController::CreateUserWidget(TSubclassOf<UUserWidget> WidgetClass)
{
	if (!WidgetClass.Get())
		return nullptr;

	UUserWidget* Widget = CreateWidget<UUserWidget>(this, WidgetClass);

	return Widget;
}
