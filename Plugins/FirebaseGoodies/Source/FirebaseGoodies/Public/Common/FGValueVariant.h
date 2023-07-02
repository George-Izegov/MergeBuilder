// Copyright (c) 2022 Nineva Studios

#pragma once

#include "Misc/Variant.h"

#include "FGValueVariant.generated.h"

struct FFGValueVariant;

template<>
struct TVariantTraits<TArray<FFGValueVariant>>
{
	static CONSTEXPR EVariantTypes GetType() { return EVariantTypes::Custom; }
};

template<>
struct TVariantTraits<TMap<FString, FFGValueVariant>>
{
	static CONSTEXPR EVariantTypes GetType() { return EVariantTypes::Custom; }
};

UENUM(BlueprintType)
enum class EValueType : uint8
{
	Null,
	Integer,
	Float,
	Bool,
	String,
	Array,
	Map,
	/// <summary>
	/// Only for Firestore Database
	/// </summary>
	ServerTimestamp
};

USTRUCT(BlueprintType)
struct FFGValueVariant
{
	GENERATED_BODY()

	template<typename T>
	T GetValue() const;

	UPROPERTY(BlueprintReadOnly, Category = "Firebase Goodies|Value Variant")
	EValueType Type = EValueType::Null;
	FVariant Value;

	friend FArchive& operator<<(FArchive& Ar, FFGValueVariant& Variant)
	{
		return Ar << Variant.Type << Variant.Value;
	}
};
