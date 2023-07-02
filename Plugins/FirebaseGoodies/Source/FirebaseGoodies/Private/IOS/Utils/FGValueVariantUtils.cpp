// Copyright (c) 2022 Nineva Studios

#include "FGValueVariantUtils.h"

#import <FirebaseFirestore/FirebaseFirestore.h>

#include "FirebaseGoodiesLog.h"

namespace ValueVariantUtils
{
id VariantToId(const FFGValueVariant& Value)
{
	switch (Value.Type)
	{
		case EValueType::Null:
		{
			return nullptr;
		}
		case EValueType::Integer:
		{
			return [NSNumber numberWithInt:Value.GetValue<int32>()];
		}
		case EValueType::Float:
		{
			return [NSNumber numberWithFloat:Value.GetValue<float>()];
		}
		case EValueType::Bool:
		{
			return [NSNumber numberWithBool:Value.GetValue<bool>()];
		}
		case EValueType::String:
		{
			return Value.GetValue<FString>().GetNSString();
		}
		case EValueType::Array:
		{
			return VariantArrayToNSArray(Value.GetValue<TArray<FFGValueVariant>>());
		}
		case EValueType::Map:
		{
			return VariantMapToNSDictionary(Value.GetValue<TMap<FString, FFGValueVariant>>());
		}
		case EValueType::ServerTimestamp:
		{
			return [FIRFieldValue fieldValueForServerTimestamp];
		}
		default:
			return nullptr;
	}
}

FFGValueVariant IdToVariant(id Object)
{
	FFGValueVariant Variant;
	if (Object == nil || Object == [NSNull null])
		return Variant;

	if ([Object isKindOfClass:[NSNumber class]])
	{
		NSNumber* Number = (NSNumber*)Object;
		const char* Type = [Object objCType];

		if (strcmp(Type, @encode(float)) == 0 ||
			strcmp(Type, @encode(double)) == 0)
		{
			Variant.Type = EValueType::Float;
			Variant.Value = (float)Number.floatValue;
			return Variant;
		}

		else if (strcmp(Type, @encode(BOOL)) == 0 ||
				 strcmp(Type, @encode(signed char)) == 0)
		{
			Variant.Type = EValueType::Bool;
			Variant.Value = Number.boolValue == YES ? true : false;
			return Variant;
		}

		else
		{
			Variant.Type = EValueType::Integer;
			Variant.Value = (int32)Number.intValue;
			return Variant;
		}
	}

	if ([Object isKindOfClass:[NSString class]])
	{
		Variant.Type = EValueType::String;
		Variant.Value = FString((NSString*)Object);
		return Variant;
	}

	if ([Object isKindOfClass:[NSArray class]])
	{
		Variant.Type = EValueType::Array;
		Variant.Value = NSArrayToVariantArray(Object);
		return Variant;
	}

	if ([Object isKindOfClass:[NSDictionary class]])
	{
		Variant.Type = EValueType::Map;
		Variant.Value = NSDictionaryToVariantMap(Object);
		return Variant;
	}
	
	if ([Object isKindOfClass:[FIRTimestamp class]])
	{
		Variant.Type = EValueType::ServerTimestamp;
		long timestamp = ((FIRTimestamp*) Object).seconds;
		Variant.Value = FDateTime::FromUnixTimestamp(timestamp);
		return Variant;
	}

	const FString ClassName(NSStringFromClass([Object class]));
	UE_LOG(LogFirebaseGoodies, Warning, TEXT("NSObject of type %s cannot be converted to a variant"), *ClassName);
	return Variant;
}

id VariantArrayToNSArray(const TArray<FFGValueVariant>& ValueArray)
{
	NSMutableArray* Array = [NSMutableArray arrayWithCapacity:ValueArray.Num()];
	for (const auto& DataValue : ValueArray)
	{
		[Array addObject:VariantToId(DataValue)];
	}

	return Array;
}

id VariantMapToNSDictionary(const TMap<FString, FFGValueVariant>& ValueMap)
{
	NSDictionary* Dict = [NSMutableDictionary dictionaryWithCapacity:ValueMap.Num()];
	for (const auto& DataPair : ValueMap)
	{
		[Dict setValue:VariantToId(DataPair.Value) forKey:DataPair.Key.GetNSString()];
	}

	return Dict;
}

TArray<FFGValueVariant> NSArrayToVariantArray(NSArray* Array)
{
	TArray<FFGValueVariant> VariantArray;

	for (id Object in Array)
	{
		VariantArray.Add(IdToVariant(Object));
	}

	return VariantArray;
}

TMap<FString, FFGValueVariant> NSDictionaryToVariantMap(NSDictionary* Dict)
{
	TMap<FString, FFGValueVariant> VariantMap;
	for (id Key in Dict)
	{
		id Value = [Dict objectForKey:Key];
		FFGValueVariant Variant = IdToVariant(Value);
		if (Variant.Type != EValueType::Null)
		{
			VariantMap.Add(FString((NSString*)Key), Variant);
		}
	}

	return VariantMap;
}

}
