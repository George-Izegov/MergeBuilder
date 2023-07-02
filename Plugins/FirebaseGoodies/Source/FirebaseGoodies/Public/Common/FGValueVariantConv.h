// Copyright (c) 2022 Nineva Studios

#pragma once

#include "Common/FGValueVariant.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "FGValueVariantConv.generated.h"

UCLASS()
class FIREBASEGOODIES_API UFGValueVariantConv : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	* Conterts int to Value Variant.
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Integer To Value Variant", CompactNodeTitle = "->", BlueprintAutocast), Category = "Firebase Goodies|Value Variant")
	static FFGValueVariant Conv_intToFGValueVariant(int32 Value);

	/**
	* Conterts float to Value Variant.
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Float To Value Variant", CompactNodeTitle = "->", BlueprintAutocast), Category = "Firebase Goodies|Value Variant")
	static FFGValueVariant Conv_floatToFGValueVariant(float Value);

	/**
	* Conterts bool to Value Variant.
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Bool To Value Variant", CompactNodeTitle = "->", BlueprintAutocast), Category = "Firebase Goodies|Value Variant")
	static FFGValueVariant Conv_boolToFGValueVariant(bool Value);

	/**
	* Conterts String to Value Variant.
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "String To Value Variant", CompactNodeTitle = "->", BlueprintAutocast), Category = "Firebase Goodies|Value Variant")
	static FFGValueVariant Conv_StringToFGValueVariant(const FString& Value);

	/**
	* Conterts Array to Value Variant.
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Array To Value Variant", CompactNodeTitle = "->", BlueprintAutocast), Category = "Firebase Goodies|Value Variant")
	static FFGValueVariant Conv_ArrayToFGValueVariant(const TArray<FFGValueVariant>& Value);

	/**
	* Conterts Map to Value Variant.
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Map To Value Variant", CompactNodeTitle = "->", BlueprintAutocast), Category = "Firebase Goodies|Value Variant")
	static FFGValueVariant Conv_MapToFGValueVariant(const TMap<FString, FFGValueVariant>& Value);

	/**
	* Conterts DateTime to Value Variant.
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Map To Value Variant", CompactNodeTitle = "->", BlueprintAutocast), Category = "Firebase Goodies|Value Variant")
	static FFGValueVariant Conv_DateTimeToFGValueVariant(const FDateTime& Value);
};