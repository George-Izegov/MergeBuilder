// Copyright (c) 2022 Nineva Studios

#include "Analytics/FGAnalyticsParameter.h"

#include "FirebaseGoodiesLog.h"

void UFGAnalyticsParameter::SetName(const FString& NewName)
{
	Name = NewName;
}

void UFGAnalyticsParameter::SetInt(int64 NewValue)
{
	if (Value.GetType() != EVariantTypes::Empty && Value.GetType() != EVariantTypes::Int64)
	{
		UE_LOG(LogFirebaseGoodies, Warning, TEXT("Parameter type redefinition"));
	}

	Value = NewValue;
}

void UFGAnalyticsParameter::SetFloat(float NewValue)
{
	if (Value.GetType() != EVariantTypes::Empty && Value.GetType() != EVariantTypes::Float)
	{
		UE_LOG(LogFirebaseGoodies, Warning, TEXT("Parameter type redefinition"));
	}

	Value = NewValue;
}

void UFGAnalyticsParameter::SetString(const FString& NewValue)
{
	if (Value.GetType() != EVariantTypes::Empty && Value.GetType() != EVariantTypes::String)
	{
		UE_LOG(LogFirebaseGoodies, Warning, TEXT("Parameter type redefinition"));
	}

	Value = NewValue;
}

FString UFGAnalyticsParameter::GetName()
{
	return Name;
}

EParameterType UFGAnalyticsParameter::GetType()
{
	switch (Value.GetType())
	{
		case EVariantTypes::Int64:
			return EParameterType::Integer;
		case EVariantTypes::Float:
			return EParameterType::Float;
		case EVariantTypes::String:
			return EParameterType::String;
		default:
			return EParameterType::Undefined;
	}
}

int64 UFGAnalyticsParameter::GetInt()
{
	if (Value.GetType() != EVariantTypes::Int64)
	{
		UE_LOG(LogFirebaseGoodies, Error, TEXT("Parameter is not an Integer parameter"));
		return 0;
	}

	return Value.GetValue<int64>();
}

float UFGAnalyticsParameter::GetFloat()
{
	if (Value.GetType() != EVariantTypes::Float)
	{
		UE_LOG(LogFirebaseGoodies, Error, TEXT("Parameter is not a Float parameter"));
		return 0.0;
	}

	return Value.GetValue<float>();
}

FString UFGAnalyticsParameter::GetString()
{
	if (Value.GetType() != EVariantTypes::String)
	{
		UE_LOG(LogFirebaseGoodies, Error, TEXT("Parameter is not a String parameter"));
		return FString();
	}

	return Value.GetValue<FString>();
}
