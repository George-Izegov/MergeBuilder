// Copyright (c) 2021 Nineva Studios

#include "FGVariantUtils.h"

#include "FGUtils.h"
#include "FirebaseGoodiesLog.h"
#include "Common/FGValueVariantConv.h"

namespace FGVariantUtils
{
#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	firebase::Variant ValueVariantToFirebaseVariant(const FFGValueVariant& Variant)
	{
		firebase::Variant result;

		if (Variant.Type == EValueType::Null)
		{
			return result;
		}

		switch (Variant.Type)
		{
			case EValueType::Integer:
			{
				result.set_int64_value(Variant.GetValue<int>());
				break;
			}
			case EValueType::Float:
			{
				result.set_double_value(Variant.GetValue<float>());
				break;
			}
			case EValueType::Bool:
			{
				result.set_bool_value(Variant.GetValue<bool>());
				break;
			}
			case EValueType::String:
			{
				result.set_string_value(FGUtils::GetStringCopy(Variant.GetValue<FString>()));
				break;
			}
			case EValueType::Array:
			{
				std::vector<firebase::Variant> vector;
				for (const auto& Element : Variant.GetValue<TArray<FFGValueVariant>>())
				{
					vector.push_back(ValueVariantToFirebaseVariant(Element));
				}

				result.set_vector(vector);
				break;
			}
			case EValueType::Map:
			{
				std::map<firebase::Variant, firebase::Variant> map;

				for (const auto& Pair : Variant.GetValue<TMap<FString, FFGValueVariant>>())
				{
					map.emplace(firebase::Variant::FromMutableString(FGUtils::GetStringCopy(Pair.Key)), ValueVariantToFirebaseVariant(Pair.Value));
				}

				result.set_map(map);
				break;
			}
		}

		return result;
	}

	firebase::Variant ValueVariantMapToFirebaseVariant(const TMap<FString, FFGValueVariant>& Values)
	{
		if (Values.Num() < 1)
		{
			return firebase::Variant();
		}

		std::map<char*, firebase::Variant> map;

		for (const auto& Pair : Values)
		{
			map.emplace(FGUtils::GetStringCopy(Pair.Key), ValueVariantToFirebaseVariant(Pair.Value));
		}

		return firebase::Variant(map);
	}

	FFGValueVariant FirebaseVariantToValueVariant(const firebase::Variant& Variant)
	{
		FFGValueVariant Result;

		if (Variant.is_bool())
		{
			Result = UFGValueVariantConv::Conv_boolToFGValueVariant(Variant.bool_value());
		}
		else if (Variant.is_int64())
		{
			Result = UFGValueVariantConv::Conv_intToFGValueVariant(Variant.int64_value());
		}
		else if (Variant.is_double())
		{
			Result = UFGValueVariantConv::Conv_floatToFGValueVariant(Variant.double_value());
		}
		else if (Variant.is_string())
		{
			Result = UFGValueVariantConv::Conv_StringToFGValueVariant(UTF8_TO_TCHAR(Variant.string_value()));
		}
		else if (Variant.is_vector())
		{
			Result = UFGValueVariantConv::Conv_ArrayToFGValueVariant(FirebaseVariantVectorToValueVariantArray(Variant.vector()));
		}
		else if (Variant.is_map())
		{
			Result = UFGValueVariantConv::Conv_MapToFGValueVariant(FirebaseVariantMapToValueVariantMap(Variant.map()));
		}

		return Result;
	}

	TArray<FFGValueVariant> FirebaseVariantVectorToValueVariantArray(const std::vector<firebase::Variant>& Vector)
	{
		TArray<FFGValueVariant> Result;

		for (const auto& Element : Vector)
		{
			Result.Add(FirebaseVariantToValueVariant(Element));
		}

		return Result;
	}

	TMap<FString, FFGValueVariant> FirebaseVariantMapToValueVariantMap(const std::map<firebase::Variant, firebase::Variant>& Map)
	{
		TMap<FString, FFGValueVariant> Result;

		for (const auto& Pair : Map)
		{
			Result.Add(Pair.first.string_value(), FirebaseVariantToValueVariant(Pair.second));
		}

		return Result;
	}

	firebase::firestore::FieldValue ValueVariantToFirebaseFieldValue(const FFGValueVariant& Variant)
	{
		firebase::firestore::FieldValue result;

		if (Variant.Type == EValueType::Null)
		{
			return result;
		}

		switch (Variant.Type)
		{
		case EValueType::Integer:
		{
			result = firebase::firestore::FieldValue::Integer(Variant.GetValue<int>());
			break;
		}
		case EValueType::Float:
		{
			result = firebase::firestore::FieldValue::Double(Variant.GetValue<float>());
			break;
		}
		case EValueType::Bool:
		{
			result = firebase::firestore::FieldValue::Boolean(Variant.GetValue<bool>());
			break;
		}
		case EValueType::String:
		{
			result = firebase::firestore::FieldValue::String(FGUtils::GetStringCopy(Variant.GetValue<FString>()));
			break;
		}
		case EValueType::Array:
		{
			result = firebase::firestore::FieldValue::Array(ValueVariantArrayToFirebaseFieldValueVector(Variant.GetValue<TArray<FFGValueVariant>>()));
			break;
		}
		case EValueType::Map:
		{
			firebase::firestore::MapFieldValue map = ValueVariantMapToFirebaseMapFieldValue(Variant.GetValue<TMap<FString, FFGValueVariant>>());

			result = firebase::firestore::FieldValue::Map(map);
			break;
		}
		case EValueType::ServerTimestamp:
		{
			result = firebase::firestore::FieldValue::ServerTimestamp();
			break;
		}
		}

		return result;
	}

	firebase::firestore::MapFieldValue ValueVariantMapToFirebaseMapFieldValue(const TMap<FString, FFGValueVariant>& Values)
	{
		if (Values.Num() < 1)
		{
			return firebase::firestore::MapFieldValue();
		}

		std::unordered_map<std::string, firebase::firestore::FieldValue> map;

		for (const auto& Pair : Values)
		{
			map.emplace(FGUtils::GetStringCopy(Pair.Key), ValueVariantToFirebaseFieldValue(Pair.Value));
		}

		return map;
	}

	std::vector<firebase::firestore::FieldValue> ValueVariantArrayToFirebaseFieldValueVector(const TArray<FFGValueVariant>& Values)
	{
		std::vector<firebase::firestore::FieldValue> vector;
		for (const auto& Element : Values)
		{
			vector.push_back(ValueVariantToFirebaseFieldValue(Element));
		}

		return vector;
	}

	FFGValueVariant FirebaseFieldValueToValueVariant(const firebase::firestore::FieldValue& Variant)
	{
		FFGValueVariant Result;

		if (Variant.is_boolean())
		{
			Result = UFGValueVariantConv::Conv_boolToFGValueVariant(Variant.boolean_value());
		}
		else if (Variant.is_integer())
		{
			Result = UFGValueVariantConv::Conv_intToFGValueVariant(Variant.integer_value());
		}
		else if (Variant.is_double())
		{
			Result = UFGValueVariantConv::Conv_floatToFGValueVariant(Variant.double_value());
		}
		else if (Variant.is_string())
		{
			Result = UFGValueVariantConv::Conv_StringToFGValueVariant(Variant.string_value().c_str());
		}
		else if (Variant.is_array())
		{
			Result = UFGValueVariantConv::Conv_ArrayToFGValueVariant(FirebaseFieldValueVectorToValueVariantArray(Variant.array_value()));
		}
		else if (Variant.is_map())
		{
			Result = UFGValueVariantConv::Conv_MapToFGValueVariant(FirebaseMapFieldValueToValueVariantMap(Variant.map_value()));
		}
		else if (Variant.is_timestamp())
		{
			UE_LOG(LogFirebaseGoodies, Verbose, TEXT("Variant.is_timestamp()"));
			firebase::Timestamp value = Variant.timestamp_value();

			Result = UFGValueVariantConv::Conv_DateTimeToFGValueVariant(FDateTime::FromUnixTimestamp(value.seconds()));
		}

		return Result;
	}

	TArray<FFGValueVariant> FirebaseFieldValueVectorToValueVariantArray(const std::vector<firebase::firestore::FieldValue>& Vector)
	{
		TArray<FFGValueVariant> Result;

		for (const auto& Element : Vector)
		{
			Result.Add(FirebaseFieldValueToValueVariant(Element));
		}

		return Result;
	}

	TMap<FString, FFGValueVariant> FirebaseMapFieldValueToValueVariantMap(const firebase::firestore::MapFieldValue& Values)
	{
		TMap<FString, FFGValueVariant> Result;

		for (const auto& Pair : Values)
		{
			Result.Add(Pair.first.c_str(), FirebaseFieldValueToValueVariant(Pair.second));
		}

		return Result;
	}
#endif
}