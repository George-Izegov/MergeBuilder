// Copyright (c) 2022 Nineva Studios

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Common/FGValueVariant.h"

#include "Dom/JsonObject.h"

#include "FGDemoFunctions.generated.h"

UCLASS()
class FIREBASEGOODIES_API UFGDemoFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Demo Functions")
	static FString SaveTestFile();
	
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Demo Functions")
	static void CrashApplication();
	
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Demo Functions")
	static void NotifyIos(const TMap<FString, FFGValueVariant>& NotificationPayload);
	
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Demo Functions")
	static TMap<FString, FFGValueVariant> DeserializeJson(const FString& PayloadString);
	
	static FFGValueVariant DeserealizeJsonValue(TSharedPtr<FJsonValue> Value);
};
