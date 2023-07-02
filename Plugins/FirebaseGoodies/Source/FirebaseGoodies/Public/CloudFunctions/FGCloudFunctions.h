// Copyright (c) 2022 Nineva Studios

#pragma once

#include "Common/FGValueVariant.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "FGCloudFunctions.generated.h"

USTRUCT(BlueprintType)
struct FMapWrapper
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Firebase Goodies|Cloud Functions")
	TMap<FString, FFGValueVariant> Map;
};

DECLARE_DYNAMIC_DELEGATE(FCloudFunctionsVoidDelegate);
DECLARE_DYNAMIC_DELEGATE_OneParam(FCloudFunctionsStringDelegate, FString, string);
DECLARE_DYNAMIC_DELEGATE_OneParam(FCloudFunctionsIntDelegate, int, intNum);
DECLARE_DYNAMIC_DELEGATE_OneParam(FCloudFunctionsFloatDelegate, float, floatNum);
DECLARE_DYNAMIC_DELEGATE_OneParam(FCloudFunctionsBoolDelegate, bool, boolVal);
DECLARE_DYNAMIC_DELEGATE_OneParam(FCloudFunctionsMapDelegate, const FMapWrapper&, mapVal);
DECLARE_DYNAMIC_DELEGATE_OneParam(FCloudFunctionsArrayDelegate, const TArray<FFGValueVariant>&, arrayVal);

UCLASS()
class FIREBASEGOODIES_API UFGCloudFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	* Calls cloud function with String return type.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Functions")
	static void CallStringFunction(const FString& MethodName, FString Region,
		const TMap<FString, FFGValueVariant>& Parameters,
		const FCloudFunctionsStringDelegate& OnSuccess,
		const FCloudFunctionsStringDelegate& OnError);

	/**
	* Calls cloud function with int return type.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Functions")
	static void CallIntFunction(const FString& MethodName, FString Region,
		const TMap<FString, FFGValueVariant>& Parameters,
		const FCloudFunctionsIntDelegate& OnSuccess,
		const FCloudFunctionsStringDelegate& OnError);

	/**
	* Calls cloud function with float return type.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Functions")
	static void CallFloatFunction(const FString& MethodName, FString Region,
		const TMap<FString, FFGValueVariant>& Parameters,
		const FCloudFunctionsFloatDelegate& OnSuccess,
		const FCloudFunctionsStringDelegate& OnError);

	/**
	* Calls cloud function with bool return type.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Functions")
	static void CallBoolFunction(const FString& MethodName, FString Region,
		const TMap<FString, FFGValueVariant>& Parameters,
		const FCloudFunctionsBoolDelegate& OnSuccess,
		const FCloudFunctionsStringDelegate& OnError);

	/**
	* Calls void cloud function.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Functions")
	static void CallVoidFunction(const FString& MethodName, FString Region,
		const TMap<FString, FFGValueVariant>& Parameters,
		const FCloudFunctionsVoidDelegate& OnSuccess,
		const FCloudFunctionsStringDelegate& OnError);

	/**
	* Calls cloud function with Map return type.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Functions")
	static void CallMapFunction(const FString& MethodName, FString Region,
		const TMap<FString, FFGValueVariant>& Parameters,
		const FCloudFunctionsMapDelegate& OnSuccess,
		const FCloudFunctionsStringDelegate& OnError);

	/**
	* Calls cloud function with Array return type.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Functions")
	static void CallArrayFunction(const FString& MethodName, FString Region,
		const TMap<FString, FFGValueVariant>& Parameters,
		const FCloudFunctionsArrayDelegate& OnSuccess,
		const FCloudFunctionsStringDelegate& OnError);

	static FCloudFunctionsStringDelegate OnCallCloudFunctionErrorCallback;
	static FCloudFunctionsStringDelegate OnCallStringCloudFunctionSuccessCallback;
	static FCloudFunctionsIntDelegate OnCallIntCloudFunctionSuccessCallback;
	static FCloudFunctionsFloatDelegate OnCallFloatCloudFunctionSuccessCallback;
	static FCloudFunctionsBoolDelegate OnCallBoolCloudFunctionSuccessCallback;
	static FCloudFunctionsVoidDelegate OnCallVoidCloudFunctionSuccessCallback;
	static FCloudFunctionsMapDelegate OnCallMapCloudFunctionSuccessCallback;
	static FCloudFunctionsArrayDelegate OnCallArrayCloudFunctionSuccessCallback;

	static const ANSICHAR* FGCloudFunctionsClassName;
};
