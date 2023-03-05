// Fill out your copyright notice in the Description page of Project Settings.


#include "Utilities/WebRequestSubsystem.h"
#include "Serialization/JsonSerializer.h"

UWebRequestSubsystem::UWebRequestSubsystem()
{
}

void UWebRequestSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
}

void UWebRequestSubsystem::Deinitialize()
{
}

// init request, create request retrier
void UWebRequestSubsystem::InitRequest(FHttpRequestPtr& Request, const FString& RequestType, const FString& ScriptURL,
	const TArray<TPair<FString, FString>>& Headers)
{
	Request->SetURL(ScriptURL);
	Request->SetVerb(RequestType);
	Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", TEXT("application/json"));

	for (const auto& Header : Headers)
	{
		Request->SetHeader(Header.Key, Header.Value);
	}
}

FHttpRequestCompleteDelegate& UWebRequestSubsystem::ProcessRequestByJsonStream(const FString& ScriptURL, const FString& JsonStream,
	EWebRequestType RequestType, const TArray<TPair<FString, FString>>& Headers, bool WithSSL)
{
	FHttpRequestPtr IHTTPRequest = FHttpModule::Get().CreateRequest();

	FString ResultURL = "http://";
	if (WithSSL) ResultURL = "https://";

	ResultURL += ScriptURL;

	if (RequestType == EWebRequestType::POST)
	{
		InitRequest(IHTTPRequest, "POST", ResultURL, Headers);
	}
	else
	{
		InitRequest(IHTTPRequest, "GET", ResultURL, Headers);
	}

	if (RequestType == EWebRequestType::POST)
		IHTTPRequest->SetContentAsString(JsonStream);

	UE_LOG(LogTemp, Display, TEXT("Send %s request to %s with data: %s."), *IHTTPRequest->GetVerb(), *ResultURL, *JsonStream);

	IHTTPRequest->ProcessRequest();

	return IHTTPRequest->OnProcessRequestComplete();
}

FHttpRequestCompleteDelegate& UWebRequestSubsystem::CallWebScript(const FString& ScriptURL, TSharedPtr<FJsonObject>& JsonRequest,
	const TArray<TPair<FString, FString>>& Headers, bool WithSSL, EWebRequestType RequestType)
{
	FString JsonStream;
	const auto Json_writer = TJsonWriterFactory<>::Create(&JsonStream);
	FJsonSerializer::Serialize(JsonRequest.ToSharedRef(), Json_writer);

	return ProcessRequestByJsonStream(ScriptURL, JsonStream, RequestType, Headers, WithSSL);
}

FHttpRequestCompleteDelegate& UWebRequestSubsystem::CallWebScript(const FString& ScriptURL, TArray<TSharedPtr<FJsonValue>>& JsonRequest,
	const TArray<TPair<FString, FString>>& Headers, bool WithSSL, EWebRequestType RequestType)
{
	FString JsonStream;
	const auto Json_writer = TJsonWriterFactory<>::Create(&JsonStream);
	FJsonSerializer::Serialize(JsonRequest, Json_writer);

	return ProcessRequestByJsonStream(ScriptURL, JsonStream, RequestType, Headers, WithSSL);
}

FHttpRequestCompleteDelegate& UWebRequestSubsystem::CallWebScript(const FString& ScriptURL, const TArray<TPair<FString, FString>>& Headers, bool WithSSL, EWebRequestType RequestType)
{
	return ProcessRequestByJsonStream(ScriptURL, "", RequestType, Headers, WithSSL);
}
