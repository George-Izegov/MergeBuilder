// Copyright (c) 2022 Nineva Studios

#pragma once

#include "Android/AndroidApplication.h"
#include "Android/AndroidJNI.h"
#include "Android/AndroidJava.h"

class UFGRemoteConfigParameterVariant;
class UFGDocumentSnapshot;

namespace FGJavaConvertor
{
jobjectArray ConvertToJStringArray(const TArray<FString>& stringArray);

jbooleanArray ConvertToJBooleanArray(const TArray<bool>& boolArray);

jintArray ConvertToJIntArray(const TArray<int>& intArray);

jbyteArray ConvertToJByteArray(const TArray<uint8>& byteArray);

jlongArray ConvertToJLongArray(const TArray<long>& longArray);

TArray<uint8> ConvertToByteArray(jbyteArray javaArray);

TArray<int> ConvertToIntArray(jintArray javaArray);

TArray<long> ConvertToLongArray(jlongArray javaArray);

TArray<FString> ConvertToStringArray(jobjectArray javaStringArray);

FString FromJavaString(jstring javaString);

int FromJavaInt(jint javaInt);

float FromJavaFloat(jfloat javaFloat);

bool FromJavaBool(jboolean javaBool);

FString FromJavaUri(jobject javaUri);

TMap<FString, FString> FromJavaMap(jobject javaMap);

jobject RemoteConfigParameterMapToJavaMap(const TMap<FString, UFGRemoteConfigParameterVariant*>& Values);
	
jobject StringMapToJavaMap(const TMap<FString, FString>& ValueMap);

TArray<UFGDocumentSnapshot*> FromJavaDocumentSnapshots(jobjectArray javaSnapshots);
}
