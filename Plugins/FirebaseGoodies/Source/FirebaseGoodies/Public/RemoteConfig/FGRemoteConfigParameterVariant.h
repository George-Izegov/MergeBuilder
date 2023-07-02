// Copyright (c) 2022 Nineva Studios

#pragma once

#include "Misc/Variant.h"

#include "FGRemoteConfigParameterVariant.generated.h"

UENUM()
enum class ERemoteConfigParameterValueType : uint8
{
	Null,
	Long,
	Float,
	Bool,
	String,
};

UCLASS(BlueprintType)
class FIREBASEGOODIES_API UFGRemoteConfigParameterVariant : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Long To Remote Config Parameter Variant", CompactNodeTitle = "->", BlueprintAutocast), Category = "Firebase Goodies|Remote Config")
	static UFGRemoteConfigParameterVariant* Conv_longToFGRemoteConfigParameterVariant(int64 Value);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Float To Remote Config Parameter Variant", CompactNodeTitle = "->", BlueprintAutocast), Category = "Firebase Goodies|Remote Config")
	static UFGRemoteConfigParameterVariant* Conv_floatToFGRemoteConfigParameterVariant(float Value);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Bool To Remote Config Parameter Variant", CompactNodeTitle = "->", BlueprintAutocast), Category = "Firebase Goodies|Remote Config")
	static UFGRemoteConfigParameterVariant* Conv_boolToFGRemoteConfigParameterVariant(bool Value);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "String To Remote Config Parameter Variant", CompactNodeTitle = "->", BlueprintAutocast), Category = "Firebase Goodies|Remote Config")
	static UFGRemoteConfigParameterVariant* Conv_StringToFGRemoteConfigParameterVariant(const FString& Value);

	template<typename T>
	T GetValue() const;

	ERemoteConfigParameterValueType Type = ERemoteConfigParameterValueType::Null;
	FVariant Value;
};
