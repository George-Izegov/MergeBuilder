// Copyright (c) 2022 Nineva Studios

#include "FGJavaConvertor.h"
#include "FGMethodCallUtils.h"
#include "RemoteConfig/FGRemoteConfigParameterVariant.h"
#include "Firestore/FGDocumentSnapshot.h"

namespace FGJavaConvertor
{
jobjectArray ConvertToJStringArray(const TArray<FString>& stringArray)
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();

	jobjectArray javaStringArray = (jobjectArray)Env->NewObjectArray(stringArray.Num(), FJavaWrapper::JavaStringClass, nullptr);

	for (int i = 0; i < stringArray.Num(); i++)
	{
		Env->SetObjectArrayElement(javaStringArray, i, *FJavaClassObject::GetJString(stringArray[i]));
	}

	return javaStringArray;
}

jbooleanArray ConvertToJBooleanArray(const TArray<bool>& boolArray)
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();

	jbooleanArray javaBooleanArray = (jbooleanArray)Env->NewBooleanArray(boolArray.Num());

	jboolean* javaBooleanArrayPtr = Env->GetBooleanArrayElements(javaBooleanArray, 0);

	for (int i = 0; i < boolArray.Num(); i++)
	{
		javaBooleanArrayPtr[i] = boolArray[i];
	}

	return javaBooleanArray;
}

jintArray ConvertToJIntArray(const TArray<int>& intArray)
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();

	jintArray javaIntArray = (jintArray)Env->NewIntArray(intArray.Num());

	jint* javaIntArrayPtr = (jint*)malloc(intArray.Num() * sizeof(jint));

	for (int i = 0; i < intArray.Num(); i++)
	{
		javaIntArrayPtr[i] = (jint)intArray[i];
	}

	Env->SetIntArrayRegion(javaIntArray, 0, intArray.Num(), javaIntArrayPtr);

	free(javaIntArrayPtr);

	return javaIntArray;
}

jbyteArray ConvertToJByteArray(const TArray<uint8>& byteArray)
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();

	jbyteArray javaByteArray = (jbyteArray)Env->NewByteArray(byteArray.Num());

	jbyte* javaByteArrayPtr = (jbyte*)malloc(byteArray.Num() * sizeof(jbyte));

	for (int i = 0; i < byteArray.Num(); i++)
	{
		javaByteArrayPtr[i] = byteArray[i];
	}

	Env->SetByteArrayRegion(javaByteArray, 0, byteArray.Num(), javaByteArrayPtr);

	free(javaByteArrayPtr);

	return javaByteArray;
}

jlongArray ConvertToJLongArray(const TArray<long>& longArray)
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();

	jlongArray javaLongArray = (jlongArray)Env->NewLongArray(longArray.Num());

	jlong* javaLongArrayPtr = (jlong*)malloc(longArray.Num() * sizeof(jlong));

	for (int i = 0; i < longArray.Num(); i++)
	{
		javaLongArrayPtr[i] = longArray[i];
	}

	Env->SetLongArrayRegion(javaLongArray, 0, longArray.Num(), javaLongArrayPtr);

	free(javaLongArrayPtr);

	return javaLongArray;
}

TArray<uint8> ConvertToByteArray(jbyteArray javaArray)
{
	TArray<uint8> byteArray;

	JNIEnv* Env = FAndroidApplication::GetJavaEnv();

	jbyte* javaByte = Env->GetByteArrayElements(javaArray, 0);

	int length = Env->GetArrayLength(javaArray);

	for (int i = 0; i < length; i++)
	{
		byteArray.Add(javaByte[i]);
	}

	return byteArray;
}

TArray<int> ConvertToIntArray(jintArray javaArray)
{
	TArray<int> intArray;

	JNIEnv* Env = FAndroidApplication::GetJavaEnv();

	jint* javaInt = Env->GetIntArrayElements(javaArray, 0);

	int length = Env->GetArrayLength(javaArray);

	for (int i = 0; i < length; i++)
	{
		intArray.Add(javaInt[i]);
	}

	return intArray;
}

TArray<long> ConvertToLongArray(jlongArray javaArray)
{
	TArray<long> longArray;

	JNIEnv* Env = FAndroidApplication::GetJavaEnv();

	jlong* javaLong = Env->GetLongArrayElements(javaArray, 0);

	int length = Env->GetArrayLength(javaArray);

	for (int i = 0; i < length; i++)
	{
		longArray.Add((long)javaLong[i]);
	}

	return longArray;
}

TArray<FString> ConvertToStringArray(jobjectArray javaStringArray)
{
	TArray<FString> stringArray;

	JNIEnv* Env = FAndroidApplication::GetJavaEnv();

	int length = Env->GetArrayLength(javaStringArray);

	for (int i = 0; i < length; i++)
	{
		jstring javaString = static_cast<jstring>(Env->GetObjectArrayElement(javaStringArray, i));

		stringArray.Add(FromJavaString(javaString));
	}

	return stringArray;
}

FString FromJavaString(jstring javaString)
{
	JNIEnv* Env = AndroidJavaEnv::GetJavaEnv();
	const char* UTFString = Env->GetStringUTFChars(javaString, nullptr);
	FString Result(UTF8_TO_TCHAR(UTFString));
	Env->ReleaseStringUTFChars(javaString, UTFString);
	Env->DeleteLocalRef(javaString);

	return Result;
}

int FromJavaInt(jint javaInt)
{
	JNIEnv* JEnv = AndroidJavaEnv::GetJavaEnv();
	int Result = static_cast<int>(javaInt);

	return Result;
}

float FromJavaFloat(jfloat javaFloat)
{
	JNIEnv* JEnv = AndroidJavaEnv::GetJavaEnv();
	float Result = static_cast<float>(javaFloat);

	return Result;
}

bool FromJavaBool(jboolean javaBool)
{
	JNIEnv* JEnv = AndroidJavaEnv::GetJavaEnv();
	bool Result = static_cast<bool>(javaBool);

	return Result;
}

FString FromJavaUri(jobject javaUri)
{
	return FGMethodCallUtils::CallStringMethod(javaUri, "toString", "()Ljava/lang/String;");
}

TMap<FString, FString> FromJavaMap(jobject javaMap)
{
	TMap<FString, FString> result;

	JNIEnv* env = FAndroidApplication::GetJavaEnv();
	jclass mapClass = env->FindClass("java/util/HashMap");
	jmethodID entrySet = env->GetMethodID(mapClass, "entrySet", "()Ljava/util/Set;");
	jobject set = env->CallObjectMethod(javaMap, entrySet);

	jclass setClass = env->FindClass("java/util/Set");
	jmethodID iterator = env->GetMethodID(setClass, "iterator", "()Ljava/util/Iterator;");
	jobject iter = env->CallObjectMethod(set, iterator);
	// Get the Iterator method IDs
	jclass iteratorClass = env->FindClass("java/util/Iterator");
	jmethodID hasNext = env->GetMethodID(iteratorClass, "hasNext", "()Z");
	jmethodID next = env->GetMethodID(iteratorClass, "next", "()Ljava/lang/Object;");
	// Get the Entry class method IDs
	jclass entryClass = env->FindClass("java/util/Map$Entry");
	jmethodID getKey = env->GetMethodID(entryClass, "getKey", "()Ljava/lang/Object;");
	jmethodID getValue = env->GetMethodID(entryClass, "getValue", "()Ljava/lang/Object;");
	// Iterate over the entry Set
	while (env->CallBooleanMethod(iter, hasNext))
	{
		jobject entry = env->CallObjectMethod(iter, next);
		jstring javaKey = (jstring)env->CallObjectMethod(entry, getKey);
		jstring javaValue = (jstring)env->CallObjectMethod(entry, getValue);

		FString Key = FromJavaString(javaKey);
		FString Value = FromJavaString(javaValue);
		result.Add(Key, Value);

		env->DeleteLocalRef(entry);
	}

	return result;
}

jobject RemoteConfigParameterMapToJavaMap(const TMap<FString, UFGRemoteConfigParameterVariant*>& Values)
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();

	jclass HashMapClass = FJavaWrapper::FindClass(Env, "java/util/HashMap", false);
	jmethodID HashMapCtor = FJavaWrapper::FindMethod(Env, HashMapClass, "<init>", "()V", false);
	jmethodID PutMethod = FJavaWrapper::FindMethod(Env, HashMapClass, "put",
		"(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;", false);
	jobject HashMap = Env->NewObject(HashMapClass, HashMapCtor);
	for (const auto& DataPair : Values)
	{
		jobject Key = Env->NewStringUTF(TCHAR_TO_UTF8(*DataPair.Key));
		jobject Value = nullptr;
		switch (DataPair.Value->Type)
		{
			case ERemoteConfigParameterValueType::Float:
			{
				jclass DoubleClass = FJavaWrapper::FindClass(Env, "java/lang/Double", false);
				jmethodID DoubleCtor = FJavaWrapper::FindMethod(Env, DoubleClass, "<init>", "(D)V", false);

				Value = Env->NewObject(DoubleClass, DoubleCtor, (double)DataPair.Value->GetValue<float>());
				break;
			}
			case ERemoteConfigParameterValueType::Bool:
			{
				jclass BoolClass = FJavaWrapper::FindClass(Env, "java/lang/Boolean", false);
				jmethodID BoolCtor = FJavaWrapper::FindMethod(Env, BoolClass, "<init>", "(Z)V", false);

				Value = Env->NewObject(BoolClass, BoolCtor, DataPair.Value->GetValue<bool>());
				break;
			}
			case ERemoteConfigParameterValueType::Long:
			{
				jclass LongClass = FJavaWrapper::FindClass(Env, "java/lang/Long", false);
				jmethodID LongCtor = FJavaWrapper::FindMethod(Env, LongClass, "<init>", "(J)V", false);

				Value = Env->NewObject(LongClass, LongCtor, DataPair.Value->GetValue<int64>());
				break;
			}
			case ERemoteConfigParameterValueType::String:
			{
				Value = Env->NewStringUTF(TCHAR_TO_UTF8(*DataPair.Value->GetValue<FString>()));
				break;
			}
		}

		if (Value)
		{
			Env->CallObjectMethod(HashMap, PutMethod, Key, Value);
			Env->DeleteLocalRef(Value);
		}
	}

	return HashMap;
}
	
jobject StringMapToJavaMap(const TMap<FString, FString>& ValueMap)
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	
	jclass HashMapClass = FJavaWrapper::FindClass(Env, "java/util/HashMap", false);
	jmethodID HasMapCtor = FJavaWrapper::FindMethod(Env, HashMapClass, "<init>", "()V", false);
	jmethodID PutMethod = FJavaWrapper::FindMethod(Env, HashMapClass, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;", false);
	jobject HashMap = Env->NewObject(HashMapClass, HasMapCtor);
	
	for (const auto& DataPair : ValueMap)
	{
		jobject Key = Env->NewStringUTF(TCHAR_TO_UTF8(*DataPair.Key));
		jobject Value = Env->NewStringUTF(TCHAR_TO_UTF8(*DataPair.Value));
		jobject PrevValue = Env->CallObjectMethod(HashMap, PutMethod, Key, Value);
		
		if (PrevValue) Env->DeleteLocalRef(PrevValue);
		Env->DeleteLocalRef(Key);
		Env->DeleteLocalRef(Value);
	}
	
	return HashMap;
}

TArray<UFGDocumentSnapshot*> FromJavaDocumentSnapshots(jobjectArray javaSnapshots)
{
	TArray<UFGDocumentSnapshot*> Result;

	JNIEnv* Env = FAndroidApplication::GetJavaEnv();

	int length = Env->GetArrayLength(javaSnapshots);

	for (int i = 0; i < length; i++)
	{
		jobject javaSnapshot = Env->GetObjectArrayElement(javaSnapshots, i);

		UFGDocumentSnapshot* snapshot = NewObject<UFGDocumentSnapshot>();
		snapshot->Init(javaSnapshot);
		Result.Add(snapshot);
	}

	return Result;
}
}
