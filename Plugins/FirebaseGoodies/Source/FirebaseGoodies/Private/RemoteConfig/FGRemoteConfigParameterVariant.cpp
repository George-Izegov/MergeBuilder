// Copyright (c) 2022 Nineva Studios

#include "RemoteConfig/FGRemoteConfigParameterVariant.h"

template<typename T>
T UFGRemoteConfigParameterVariant::GetValue() const
{
	return Value.GetValue<T>();
}

template int64 UFGRemoteConfigParameterVariant::GetValue<int64>() const;
template float UFGRemoteConfigParameterVariant::GetValue<float>() const;
template bool UFGRemoteConfigParameterVariant::GetValue<bool>() const;
template FString UFGRemoteConfigParameterVariant::GetValue<FString>() const;

UFGRemoteConfigParameterVariant* UFGRemoteConfigParameterVariant::Conv_longToFGRemoteConfigParameterVariant(int64 Value)
{
	UFGRemoteConfigParameterVariant* ValueVariant = NewObject<UFGRemoteConfigParameterVariant>();
	ValueVariant->Type = ERemoteConfigParameterValueType::Long;
	ValueVariant->Value = Value;

	return ValueVariant;
}

UFGRemoteConfigParameterVariant* UFGRemoteConfigParameterVariant::Conv_floatToFGRemoteConfigParameterVariant(float Value)
{
	UFGRemoteConfigParameterVariant* ValueVariant = NewObject<UFGRemoteConfigParameterVariant>();
	ValueVariant->Type = ERemoteConfigParameterValueType::Float;
	ValueVariant->Value = Value;

	return ValueVariant;
}

UFGRemoteConfigParameterVariant* UFGRemoteConfigParameterVariant::Conv_boolToFGRemoteConfigParameterVariant(bool Value)
{
	UFGRemoteConfigParameterVariant* ValueVariant = NewObject<UFGRemoteConfigParameterVariant>();
	ValueVariant->Type = ERemoteConfigParameterValueType::Bool;
	ValueVariant->Value = Value;

	return ValueVariant;
}

UFGRemoteConfigParameterVariant* UFGRemoteConfigParameterVariant::Conv_StringToFGRemoteConfigParameterVariant(const FString& Value)
{
	UFGRemoteConfigParameterVariant* ValueVariant = NewObject<UFGRemoteConfigParameterVariant>();
	ValueVariant->Type = ERemoteConfigParameterValueType::String;
	ValueVariant->Value = Value;

	return ValueVariant;
}
