// Copyright (c) 2022 Nineva Studios

#pragma once

#include "Common/FGValueVariant.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "FGValueVariantAccess.generated.h"

UENUM(BlueprintType)
enum class EVariantGetExec : uint8
{
	Then,
	Failed
};

UCLASS()
class FIREBASEGOODIES_API UFGValueVariantAccess : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	* Get value
	*
	* @return - Integer value.
	*/
	UFUNCTION(BlueprintPure, meta = (CompactNodeTitle = "Integer"), Category = "Firebase Goodies|Value Variant")
	static int32 GetInteger(const FFGValueVariant& Variant);

	/**
	* Try to get value
	*
	* @return - Integer value.
	*/
	UFUNCTION(BlueprintCallable, meta = (ExpandEnumAsExecs = NextExec), Category = "Firebase Goodies|Value Variant")
	static int32 TryGetInteger(const FFGValueVariant& Variant, EVariantGetExec& NextExec);

	/**
	* Get value
	*
	* @return - Float value.
	*/
	UFUNCTION(BlueprintPure, meta = (CompactNodeTitle = "Float"), Category = "Firebase Goodies|Value Variant")
	static float GetFloat(const FFGValueVariant& Variant);

	/**
	* Try to get value
	*
	* @return - Float value.
	*/
	UFUNCTION(BlueprintCallable, meta = (ExpandEnumAsExecs = NextExec), Category = "Firebase Goodies|Value Variant")
	static float TryGetFloat(const FFGValueVariant& Variant, EVariantGetExec& NextExec);

	/**
	* Get value
	*
	* @return - Boolean value.
	*/
	UFUNCTION(BlueprintPure, meta = (CompactNodeTitle = "Bool"), Category = "Firebase Goodies|Value Variant")
	static bool GetBool(const FFGValueVariant& Variant);

	/**
	* Try to get value
	*
	* @return - Boolean value.
	*/
	UFUNCTION(BlueprintCallable, meta = (ExpandEnumAsExecs = NextExec), Category = "Firebase Goodies|Value Variant")
	static bool TryGetBool(const FFGValueVariant& Variant, EVariantGetExec& NextExec);

	/**
	* Get value
	*
	* @return - String value.
	*/
	UFUNCTION(BlueprintPure, meta = (CompactNodeTitle = "String"), Category = "Firebase Goodies|Value Variant")
	static FString GetString(const FFGValueVariant& Variant);

	/**
	* Try to get value
	*
	* @return - String value.
	*/
	UFUNCTION(BlueprintCallable, meta = (ExpandEnumAsExecs = NextExec), Category = "Firebase Goodies|Value Variant")
	static FString TryGetString(const FFGValueVariant& Variant, EVariantGetExec& NextExec);

	/**
	* Get value
	*
	* @return - Array value.
	*/
	UFUNCTION(BlueprintPure, meta = (CompactNodeTitle = "Array"), Category = "Firebase Goodies|Value Variant")
	static TArray<FFGValueVariant> GetArray(const FFGValueVariant& Variant);

	/**
	* Try to get value
	*
	* @return - Array value.
	*/
	UFUNCTION(BlueprintCallable, meta = (ExpandEnumAsExecs = NextExec), Category = "Firebase Goodies|Value Variant")
	static TArray<FFGValueVariant> TryGetArray(const FFGValueVariant& Variant, EVariantGetExec& NextExec);

	/**
	* Get value
	*
	* @return - Map value.
	*/
	UFUNCTION(BlueprintPure, meta = (CompactNodeTitle = "Map"), Category = "Firebase Goodies|Value Variant")
	static TMap<FString, FFGValueVariant> GetMap(const FFGValueVariant& Variant);

	/**
	* Try to get value
	*
	* @return - Map value.
	*/
	UFUNCTION(BlueprintCallable, meta = (ExpandEnumAsExecs = NextExec), Category = "Firebase Goodies|Value Variant")
	static TMap<FString, FFGValueVariant> TryGetMap(const FFGValueVariant& Variant, EVariantGetExec& NextExec);

	/**
	* Get value
	*
	* @return - DateTime value.
	*/
	UFUNCTION(BlueprintPure, meta = (CompactNodeTitle = "DateTime"), Category = "Firebase Goodies|Value Variant")
	static FDateTime GetDateTime(const FFGValueVariant& Variant);

	/**
	* Try to get value
	*
	* @return - DateTime value.
	*/
	UFUNCTION(BlueprintCallable, meta = (ExpandEnumAsExecs = NextExec), Category = "Firebase Goodies|Value Variant")
	static FDateTime TryGetDateTime(const FFGValueVariant& Variant, EVariantGetExec& NextExec);
};
