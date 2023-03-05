// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "WebRequestSubsystem.generated.h"

//DECLARE_LOG_CATEGORY_EXTERN(LogWebRequest, Log, All);

UENUM(BlueprintType)
enum class EWebRequestType : uint8
{
	POST,
	GET,
	TYPE_MAX,
};

DECLARE_DELEGATE_TwoParams(FWebRequestResult, FHttpResponsePtr Response, bool bSuccessful);

/**
 * 
 */
UCLASS()
class MERGEBUILDER_API UWebRequestSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:

	UWebRequestSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	FHttpRequestCompleteDelegate& CallWebScript(const FString& ScriptURL, TSharedPtr<FJsonObject>& JsonRequest,
		const TArray<TPair<FString, FString>>& Headers, bool WithSSL = false, EWebRequestType RequestType = EWebRequestType::POST);
	FHttpRequestCompleteDelegate& CallWebScript(const FString& ScriptURL, TArray<TSharedPtr<FJsonValue>>& JsonRequest,
		const TArray<TPair<FString, FString>>& Headers, bool WithSSL = false, EWebRequestType RequestType = EWebRequestType::POST);
	FHttpRequestCompleteDelegate& CallWebScript(const FString& ScriptURL,
		const TArray<TPair<FString, FString>>& Headers, bool WithSSL = false, EWebRequestType RequestType = EWebRequestType::GET);

protected:
	FHttpRequestCompleteDelegate& ProcessRequestByJsonStream(const FString& ScriptURL, const FString& JsonStream,
		EWebRequestType RequestType, const TArray<TPair<FString, FString>>& Headers, bool WithSSL);

private:

	void InitRequest(FHttpRequestPtr& Request, const FString& RequestType, const FString& ScriptURL,
		const TArray<TPair<FString, FString>>& Headers);

	FHttpModule* HTTP;
};
