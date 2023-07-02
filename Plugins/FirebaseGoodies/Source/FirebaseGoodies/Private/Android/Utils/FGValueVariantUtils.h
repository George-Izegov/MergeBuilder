// Copyright (c) 2022 Nineva Studios

#pragma once

#include "Android/AndroidApplication.h"
#include "Android/AndroidJNI.h"
#include "Android/AndroidJava.h"
#include "Common/FGValueVariant.h"

namespace ValueVariantUtils
{
	jobject VariantArrayToJavaList(JNIEnv* Env, const TArray<FFGValueVariant>& ValueArray);
	jobject VariantMapToJavaMap(JNIEnv* Env, const TMap<FString, FFGValueVariant>& ValueMap);
	TArray<FFGValueVariant> JavaListToVariantArray(JNIEnv* Env, jobject ListObject);
	TMap<FString, FFGValueVariant> JavaMapToVariantMap(JNIEnv* Env, jobject MapObject);
	jobject VariantToJavaObject(JNIEnv* Env, const FFGValueVariant& Value);
	FFGValueVariant JavaObjectToVariant(JNIEnv* Env, jobject Object);
}
