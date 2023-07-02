// Copyright (c) 2022 Nineva Studios

#include "FGValueVariantUtils.h"
#include "FGMethodCallUtils.h"

#include "FirebaseGoodiesLog.h"

namespace ValueVariantUtils
{
FString GetJavaObjectClassName(JNIEnv* Env, jobject Object);

jobject VariantToJavaObject(JNIEnv* Env, const FFGValueVariant& Value)
{
	switch (Value.Type)
	{
		case EValueType::Null:
		{
			return nullptr;
		}
		case EValueType::Integer:
		{
			jclass IntegerClass = FJavaWrapper::FindClass(Env, "java/lang/Integer", false);
			jmethodID IntegerCtor = FJavaWrapper::FindMethod(Env, IntegerClass, "<init>", "(I)V", false);
			return Env->NewObject(IntegerClass, IntegerCtor, Value.GetValue<int32>());
		}
		case EValueType::Float:
		{
			jclass FloatClass = FJavaWrapper::FindClass(Env, "java/lang/Float", false);
			jmethodID FloatCtor = FJavaWrapper::FindMethod(Env, FloatClass, "<init>", "(F)V", false);
			return Env->NewObject(FloatClass, FloatCtor, Value.GetValue<float>());
		}
		case EValueType::Bool:
		{
			jclass BooleanClass = FJavaWrapper::FindClass(Env, "java/lang/Boolean", false);
			jmethodID BooleanCtor = FJavaWrapper::FindMethod(Env, BooleanClass, "<init>", "(Z)V", false);
			return Env->NewObject(BooleanClass, BooleanCtor, Value.GetValue<bool>());
		}
		case EValueType::String:
		{
			return Env->NewStringUTF(TCHAR_TO_UTF8(*Value.GetValue<FString>()));
		}
		case EValueType::Array:
		{
			return VariantArrayToJavaList(Env, Value.GetValue<TArray<FFGValueVariant>>());
		}
		case EValueType::Map:
		{
			return VariantMapToJavaMap(Env, Value.GetValue<TMap<FString, FFGValueVariant>>());
		}
		case EValueType::ServerTimestamp:
		{
			return FGMethodCallUtils::CallStaticObjectMethod("com/ninevastudios/unrealfirebase/FGFirestore", "serverTimestamp", "()Lcom/google/firebase/firestore/FieldValue;");
		}
		default:
			return nullptr;
	}
}

FFGValueVariant JavaObjectToVariant(JNIEnv* Env, jobject Object)
{
	FFGValueVariant Variant;
	if (Object == nullptr) return Variant;

	jclass IntegerClass = FJavaWrapper::FindClass(Env, "java/lang/Integer", false);
	if (Env->IsInstanceOf(Object, IntegerClass))
	{
		jmethodID GetIntMethod = FJavaWrapper::FindMethod(Env, IntegerClass, "intValue", "()I", false);

		Variant.Type = EValueType::Integer;
		Variant.Value = Env->CallIntMethod(Object, GetIntMethod);
		return Variant;
	}

	jclass LongClass = FJavaWrapper::FindClass(Env, "java/lang/Long", false);
	if (Env->IsInstanceOf(Object, LongClass))
	{
		jmethodID GetLongMethod = FJavaWrapper::FindMethod(Env, LongClass, "longValue", "()J", false);

		Variant.Type = EValueType::Integer;
		Variant.Value = (int32)Env->CallLongMethod(Object, GetLongMethod);
		return Variant;
	}

	jclass FloatClass = FJavaWrapper::FindClass(Env, "java/lang/Float", false);
	if (Env->IsInstanceOf(Object, FloatClass))
	{
		jmethodID GetFloatMethod = FJavaWrapper::FindMethod(Env, FloatClass, "floatValue", "()F", false);

		Variant.Type = EValueType::Float;
		Variant.Value = Env->CallFloatMethod(Object, GetFloatMethod);
		return Variant;
	}

	jclass DoubleClass = FJavaWrapper::FindClass(Env, "java/lang/Double", false);
	if (Env->IsInstanceOf(Object, DoubleClass))
	{
		jmethodID GetDoubleMethod = FJavaWrapper::FindMethod(Env, DoubleClass, "doubleValue", "()D", false);

		Variant.Type = EValueType::Float;
		Variant.Value = (float)Env->CallDoubleMethod(Object, GetDoubleMethod);
		return Variant;
	}

	jclass BooleanClass = FJavaWrapper::FindClass(Env, "java/lang/Boolean", false);
	if (Env->IsInstanceOf(Object, BooleanClass))
	{
		jmethodID GetBoolMethod = FJavaWrapper::FindMethod(Env, BooleanClass, "booleanValue", "()Z", false);

		Variant.Type = EValueType::Bool;
		Variant.Value = (bool)Env->CallBooleanMethod(Object, GetBoolMethod);
		return Variant;
	}

	jclass StringClass = FJavaWrapper::FindClass(Env, "java/lang/String", false);
	if (Env->IsInstanceOf(Object, StringClass))
	{
		Variant.Type = EValueType::String;
		jstring StringObject = static_cast<jstring>(Object);
		const auto chars = Env->GetStringUTFChars(StringObject, 0);
		Variant.Value = FString(UTF8_TO_TCHAR(chars));
		Env->ReleaseStringUTFChars(StringObject, chars);
		return Variant;
	}

	jclass ListClass = FJavaWrapper::FindClass(Env, "java/util/List", false);
	if (Env->IsInstanceOf(Object, ListClass))
	{
		Variant.Type = EValueType::Array;
		Variant.Value = JavaListToVariantArray(Env, Object);
		return Variant;
	}

	jclass HashMapClass = FJavaWrapper::FindClass(Env, "java/util/HashMap", false);
	if (Env->IsInstanceOf(Object, HashMapClass))
	{
		Variant.Type = EValueType::Map;
		Variant.Value = JavaMapToVariantMap(Env, Object);
		return Variant;
	}

	jclass TimestampClass = FAndroidApplication::FindJavaClass("com/google/firebase/Timestamp");
	if (Env->IsInstanceOf(Object, TimestampClass))
	{
		Variant.Type = EValueType::ServerTimestamp;
		long timestamp = FGMethodCallUtils::CallLongMethod(Object, "getSeconds", "()J");
		Variant.Value = FDateTime::FromUnixTimestamp(timestamp);
		return Variant;
	}

	UE_LOG(LogFirebaseGoodies, Warning, TEXT("Java class %s cannot be converted to a variant"), *GetJavaObjectClassName(Env, Object));

	return Variant;
}

FString GetJavaObjectClassName(JNIEnv* Env, jobject Object)
{
	jclass ObjectClass = Env->GetObjectClass(Object);
	jclass ClassClass = FJavaWrapper::FindClass(Env, "java/lang/Class", false);
	jmethodID GetNameMethod = FJavaWrapper::FindMethod(Env, ClassClass, "getName", "()Ljava/lang/String;", false);

	jstring Name = static_cast<jstring>(Env->CallObjectMethod(ObjectClass, GetNameMethod));
	const auto chars = Env->GetStringUTFChars(Name, 0);
	FString Return = FString(UTF8_TO_TCHAR(chars));
	Env->ReleaseStringUTFChars(Name, chars);

	return Return;
}

jobject VariantArrayToJavaList(JNIEnv* Env, const TArray<FFGValueVariant>& ValueArray)
{
	jclass ArrayListClass = FJavaWrapper::FindClass(Env, "java/util/ArrayList", false);
	jmethodID ArrayListCtor = FJavaWrapper::FindMethod(Env, ArrayListClass, "<init>", "()V", false);
	jmethodID AddMethod = FJavaWrapper::FindMethod(Env, ArrayListClass, "add", "(Ljava/lang/Object;)Z", false);
	jobject ArrayList = Env->NewObject(ArrayListClass, ArrayListCtor);

	for (const auto& DataValue : ValueArray)
	{
		jobject Value = VariantToJavaObject(Env, DataValue);
		Env->CallBooleanMethod(ArrayList, AddMethod, Value);
		Env->DeleteLocalRef(Value);
	}

	return ArrayList;
}

jobject VariantMapToJavaMap(JNIEnv* Env, const TMap<FString, FFGValueVariant>& ValueMap)
{
	jclass HashMapClass = FJavaWrapper::FindClass(Env, "java/util/HashMap", false);
	jmethodID HasMapCtor = FJavaWrapper::FindMethod(Env, HashMapClass, "<init>", "()V", false);
	jmethodID PutMethod = FJavaWrapper::FindMethod(Env, HashMapClass, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;", false);
	jobject HashMap = Env->NewObject(HashMapClass, HasMapCtor);

	for (const auto& DataPair : ValueMap)
	{
		jobject Key = Env->NewStringUTF(TCHAR_TO_UTF8(*DataPair.Key));
		jobject Value = VariantToJavaObject(Env, DataPair.Value);
		jobject PrevValue = Env->CallObjectMethod(HashMap, PutMethod, Key, Value);

		if (PrevValue) Env->DeleteLocalRef(PrevValue);
		Env->DeleteLocalRef(Key);
		Env->DeleteLocalRef(Value);
	}

	return HashMap;
}

TArray<FFGValueVariant> JavaListToVariantArray(JNIEnv* Env, jobject ListObject)
{
	jclass ListClass = FJavaWrapper::FindClass(Env, "java/util/List", false);
	jmethodID SizeMethod = FJavaWrapper::FindMethod(Env, ListClass, "size", "()I", false);
	jmethodID GeteMethod = FJavaWrapper::FindMethod(Env, ListClass, "get", "(I)Ljava/lang/Object;", false);

	const int Size = Env->CallIntMethod(ListObject, SizeMethod);
	TArray<FFGValueVariant> VariantArray;

	for (int i = 0; i < Size; i++)
	{
		jobject ArrayItem = Env->CallObjectMethod(ListObject, GeteMethod, i);
		VariantArray.Add(JavaObjectToVariant(Env, ArrayItem));
		Env->DeleteLocalRef(ArrayItem);
	}

	return VariantArray;
}

TMap<FString, FFGValueVariant> JavaMapToVariantMap(JNIEnv* Env, jobject MapObject)
{
	jclass MapClass = FJavaWrapper::FindClass(Env, "java/util/Map", false);
	jmethodID KeySetMethod = FJavaWrapper::FindMethod(Env, MapClass, "keySet", "()Ljava/util/Set;", false);
	jmethodID GetMethod = FJavaWrapper::FindMethod(Env, MapClass, "get", "(Ljava/lang/Object;)Ljava/lang/Object;", false);

	jclass SetClass = FJavaWrapper::FindClass(Env, "java/util/Set", false);
	jmethodID IteratorMethod = FJavaWrapper::FindMethod(Env, SetClass, "iterator", "()Ljava/util/Iterator;", false);

	jclass IteratorClass = FJavaWrapper::FindClass(Env, "java/util/Iterator", false);
	jmethodID HasNextMethod = FJavaWrapper::FindMethod(Env, IteratorClass, "hasNext", "()Z", false);
	jmethodID NextMethod = FJavaWrapper::FindMethod(Env, IteratorClass, "next", "()Ljava/lang/Object;", false);

	jobject KeySet = Env->CallObjectMethod(MapObject, KeySetMethod);
	jobject Iter = Env->CallObjectMethod(KeySet, IteratorMethod);

	TMap<FString, FFGValueVariant> VariantMap;
	while (Env->CallBooleanMethod(Iter, HasNextMethod))
	{
		jobject Key = Env->CallObjectMethod(Iter, NextMethod);
		jobject Value = Env->CallObjectMethod(MapObject, GetMethod, Key);

		FFGValueVariant Variant = JavaObjectToVariant(Env, Value);

		if (Variant.Type != EValueType::Null)
		{
			VariantMap.Add(FJavaHelper::FStringFromLocalRef(Env, static_cast<jstring>(Key)), Variant);
		}

		Env->DeleteLocalRef(Value);
	}

	Env->DeleteLocalRef(KeySet);
	Env->DeleteLocalRef(Iter);

	return VariantMap;
}

}
