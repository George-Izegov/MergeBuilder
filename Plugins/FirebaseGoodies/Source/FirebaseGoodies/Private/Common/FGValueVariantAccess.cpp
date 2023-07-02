// Copyright (c) 2022 Nineva Studios

#include "Common/FGValueVariantAccess.h"

#include "FirebaseGoodiesLog.h"

int32 UFGValueVariantAccess::GetInteger(const FFGValueVariant& Variant)
{
	if (Variant.Type == EValueType::Integer)
	{
		return Variant.GetValue<int32>();
	}
	UE_LOG(LogFirebaseGoodies, Warning, TEXT("Variant does not contain an integer value. Returning default value."));
	return 0;
}

int32 UFGValueVariantAccess::TryGetInteger(const FFGValueVariant& Variant, EVariantGetExec& NextExec)
{
	if (Variant.Type == EValueType::Integer)
	{
		NextExec = EVariantGetExec::Then;
		return Variant.GetValue<int32>();
	}

	NextExec = EVariantGetExec::Failed;
	return 0;
}

float UFGValueVariantAccess::GetFloat(const FFGValueVariant& Variant)
{
	if (Variant.Type == EValueType::Float)
	{
		return Variant.GetValue<float>();
	}

	UE_LOG(LogFirebaseGoodies, Warning, TEXT("Variant does not contain a float value. Returning default value."));
	return 0.0;
}

float UFGValueVariantAccess::TryGetFloat(const FFGValueVariant& Variant, EVariantGetExec& NextExec)
{
	if (Variant.Type == EValueType::Float)
	{
		NextExec = EVariantGetExec::Then;
		return Variant.GetValue<float>();
	}
	if (Variant.Type == EValueType::Integer)
	{
		NextExec = EVariantGetExec::Then;
		return Variant.GetValue<int32>();
	}

	NextExec = EVariantGetExec::Failed;
	return 0.0;
}

bool UFGValueVariantAccess::GetBool(const FFGValueVariant& Variant)
{
	if (Variant.Type == EValueType::Bool)
	{
		return Variant.GetValue<bool>();
	}

	UE_LOG(LogFirebaseGoodies, Warning, TEXT("Variant does not contain a bool value. Returning default value."));
	return false;
}

bool UFGValueVariantAccess::TryGetBool(const FFGValueVariant& Variant, EVariantGetExec& NextExec)
{
	if (Variant.Type == EValueType::Bool)
	{
		NextExec = EVariantGetExec::Then;
		return Variant.GetValue<bool>();
	}

	NextExec = EVariantGetExec::Failed;
	return false;
}

FString UFGValueVariantAccess::GetString(const FFGValueVariant& Variant)
{
	if (Variant.Type == EValueType::String)
	{
		return Variant.GetValue<FString>();
	}

	UE_LOG(LogFirebaseGoodies, Warning, TEXT("Variant does not contain a string value. Returning empty string."));
	return FString();
}

FString UFGValueVariantAccess::TryGetString(const FFGValueVariant& Variant, EVariantGetExec& NextExec)
{
	if (Variant.Type == EValueType::String)
	{
		NextExec = EVariantGetExec::Then;
		return Variant.GetValue<FString>();
	}

	NextExec = EVariantGetExec::Failed;
	return FString();
}

TArray<FFGValueVariant> UFGValueVariantAccess::GetArray(const FFGValueVariant& Variant)
{
	if (Variant.Type == EValueType::Array)
	{
		return Variant.GetValue<TArray<FFGValueVariant>>();
	}

	UE_LOG(LogFirebaseGoodies, Warning, TEXT("Variant does not contain an array value. Returning empty array."));
	return TArray<FFGValueVariant>();
}

TArray<FFGValueVariant> UFGValueVariantAccess::TryGetArray(const FFGValueVariant& Variant, EVariantGetExec& NextExec)
{
	if (Variant.Type == EValueType::Array)
	{
		NextExec = EVariantGetExec::Then;
		return Variant.GetValue<TArray<FFGValueVariant>>();
	}

	NextExec = EVariantGetExec::Failed;
	return TArray<FFGValueVariant>();
}

TMap<FString, FFGValueVariant> UFGValueVariantAccess::GetMap(const FFGValueVariant& Variant)
{
	if (Variant.Type == EValueType::Map)
	{
		return Variant.GetValue<TMap<FString, FFGValueVariant>>();
	}

	UE_LOG(LogFirebaseGoodies, Warning, TEXT("Variant does not contain a map value. Returning empty map."));
	return TMap<FString, FFGValueVariant>();
}

TMap<FString, FFGValueVariant> UFGValueVariantAccess::TryGetMap(const FFGValueVariant& Variant, EVariantGetExec& NextExec)
{
	if (Variant.Type == EValueType::Map)
	{
		NextExec = EVariantGetExec::Then;
		return Variant.GetValue<TMap<FString, FFGValueVariant>>();
	}

	NextExec = EVariantGetExec::Failed;
	return TMap<FString, FFGValueVariant>();
}

FDateTime UFGValueVariantAccess::GetDateTime(const FFGValueVariant& Variant)
{
	if (Variant.Type == EValueType::ServerTimestamp)
	{
		return Variant.GetValue<FDateTime>();
	}

	UE_LOG(LogFirebaseGoodies, Warning, TEXT("Variant does not contain a FDateTime value. Returning FDateTime::Now."));
	return FDateTime::Now();
}

FDateTime UFGValueVariantAccess::TryGetDateTime(const FFGValueVariant& Variant, EVariantGetExec& NextExec)
{
	if (Variant.Type == EValueType::ServerTimestamp)
	{
		NextExec = EVariantGetExec::Then;
		return Variant.GetValue<FDateTime>();
	}

	NextExec = EVariantGetExec::Failed;
	return FDateTime::Now();
}
