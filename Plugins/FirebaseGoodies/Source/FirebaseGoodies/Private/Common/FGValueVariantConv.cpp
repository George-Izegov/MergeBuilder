// Copyright (c) 2022 Nineva Studios

#include "Common/FGValueVariantConv.h"

FFGValueVariant UFGValueVariantConv::Conv_intToFGValueVariant(int32 Value)
{
	FFGValueVariant ValueVariant;
	ValueVariant.Type = EValueType::Integer;
	ValueVariant.Value = Value;

	return ValueVariant;
}

FFGValueVariant UFGValueVariantConv::Conv_floatToFGValueVariant(float Value)
{
	FFGValueVariant ValueVariant;
	ValueVariant.Type = EValueType::Float;
	ValueVariant.Value = Value;

	return ValueVariant;
}

FFGValueVariant UFGValueVariantConv::Conv_boolToFGValueVariant(bool Value)
{
	FFGValueVariant ValueVariant;
	ValueVariant.Type = EValueType::Bool;
	ValueVariant.Value = Value;

	return ValueVariant;
}

FFGValueVariant UFGValueVariantConv::Conv_StringToFGValueVariant(const FString& Value)
{
	FFGValueVariant ValueVariant;
	ValueVariant.Type = EValueType::String;
	ValueVariant.Value = Value;

	return ValueVariant;
}

FFGValueVariant UFGValueVariantConv::Conv_ArrayToFGValueVariant(const TArray<FFGValueVariant>& Value)
{
	FFGValueVariant ValueVariant;
	ValueVariant.Type = EValueType::Array;
	ValueVariant.Value = Value;

	return ValueVariant;
}

FFGValueVariant UFGValueVariantConv::Conv_MapToFGValueVariant(const TMap<FString, FFGValueVariant>& Value)
{
	FFGValueVariant ValueVariant;
	ValueVariant.Type = EValueType::Map;
	ValueVariant.Value = Value;

	return ValueVariant;
}

FFGValueVariant UFGValueVariantConv::Conv_DateTimeToFGValueVariant(const FDateTime& Value)
{
	FFGValueVariant ValueVariant;
	ValueVariant.Type = EValueType::ServerTimestamp;
	ValueVariant.Value = Value;

	return ValueVariant;
}
