// Fill out your copyright notice in the Description page of Project Settings.


#include "Utilities/TimeSubsystem.h"
#include "WebRequestSubsystem.h"
#include "MBUtilityFunctionLibrary.h"
#include "MBGameInstance.h"

UTimeSubsystem::UTimeSubsystem()
{
    TimeAPI_URLs.Add("timeapi.io/api/Time/current/zone?timeZone=Etc/UTC", "dateTime");
    TimeAPI_URLs.Add("worldtimeapi.org/api/timezone/Etc/UTC", "datetime");
}

void UTimeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    FTimerDelegate Delegate = FTimerDelegate::CreateUObject(this, &UTimeSubsystem::RequestTime);
    GetWorld()->GetTimerManager().SetTimerForNextTick(Delegate);

    auto GI = Cast<UMBGameInstance>(GetGameInstance());
    GI->ApplicationHasEnteredForegroundDelegate.AddDynamic(this, &UTimeSubsystem::RequestTime);
}

void UTimeSubsystem::Deinitialize()
{
}

void UTimeSubsystem::RequestTime()
{

    if (RemainURLs.Num() == 0)
    {
        RemainURLs = TimeAPI_URLs;
    }
    
    auto WebRequestSystem = GetGameInstance()->GetSubsystem<UWebRequestSubsystem>();

    FString URL;
    FString ParamName;
    TArray<FString> Keys;
    RemainURLs.GetKeys(Keys);
    URL = Keys[0];
    RemainURLs.RemoveAndCopyValue(Keys[0], ParamName);

    TArray<TPair<FString, FString>> Headers;
    FHttpRequestCompleteDelegate& CompleteDelegate = WebRequestSystem->CallWebScript(URL, Headers, true);

    CompleteDelegate.BindLambda([this, ParamName](FHttpRequestPtr RequestPtr, FHttpResponsePtr ResponsePtr, bool bSuccessful) {
        
        if (bSuccessful)
        {
            TSharedPtr<FJsonObject> JsonResponse;
            UMBUtilityFunctionLibrary::StringToJsonObject(ResponsePtr->GetContentAsString(), JsonResponse);

            FString StringDate = JsonResponse->GetStringField(ParamName);
            FDateTime::ParseIso8601(*StringDate, TimeUTC);

            TimeValid = true;

            OnTimeSuccessRequested.Broadcast();

            if (!GetWorld())
                return;

            if (GetWorld()->GetTimerManager().IsTimerActive(RecalculationTimer))
            {
                GetWorld()->GetTimerManager().ClearTimer(RecalculationTimer);
            }

            FTimerDelegate Delegate = FTimerDelegate::CreateUObject(this, &UTimeSubsystem::RecalculateTime);
            GetWorld()->GetTimerManager().SetTimer(RecalculationTimer, Delegate, 1.0f, true);
        }
        else
        {
            TimeValid = false;
            UE_LOG(LogTemp, Warning, TEXT("UTimeSubsystem::RequestTime() - Failed request: %s"), *ResponsePtr->GetContentAsString());
            FTimerHandle TimerHandle;
            FTimerDelegate Delegate = FTimerDelegate::CreateUObject(this, &UTimeSubsystem::RequestTime);
            
            if (!GetWorld())
                return;
            
            GetWorld()->GetTimerManager().SetTimer(TimerHandle, Delegate, 1.0f, false);
        }

     });

}

void UTimeSubsystem::RecalculateTime()
{
    FTimespan RecalculatePeriodTimespan = FTimespan(0, 0, 1);
    TimeUTC += RecalculatePeriodTimespan;
    //GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Blue, FString::Printf(TEXT("%i h:%i m:%i s"), TimeUTC.GetHour(), TimeUTC.GetMinute(), TimeUTC.GetSecond()));
}

const FDateTime& UTimeSubsystem::GetUTCNow()
{
    return TimeUTC;
}
