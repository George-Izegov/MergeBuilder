// Copyright (c) 2022 Nineva Studios

#include "MutableDataAndroid.h"

#include "Android/Utils/FGValueVariantUtils.h"
#include "Android/Utils/FGMethodCallUtils.h"

#include "Android/AndroidJavaEnv.h"

MutableDataAndroid::MutableDataAndroid(jobject MutableData)
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	JMutableData = Env->NewGlobalRef(MutableData);

	jclass MutableDataClass = FAndroidApplication::FindJavaClass("com/ninevastudios/unrealfirebase/FGMutableData");

	GetKeyMethod = FJavaWrapper::FindMethod(Env, MutableDataClass, "GetKey", "()Ljava/lang/String;", false);
	GetPriorityMethod = FJavaWrapper::FindMethod(Env, MutableDataClass, "GetPriority", "()Ljava/lang/Object;", false);
	SetPriorityMethod = FJavaWrapper::FindMethod(Env, MutableDataClass, "SetPriority", "(Ljava/lang/Object;)V", false);
	GetValueMethod = FJavaWrapper::FindMethod(Env, MutableDataClass, "GetValue", "()Ljava/lang/Object;", false);
	SetValueMethod = FJavaWrapper::FindMethod(Env, MutableDataClass, "SetValue", "(Ljava/lang/Object;)V", false);
	ChildMethod = FJavaWrapper::FindMethod(Env, MutableDataClass, "Child", "(Ljava/lang/String;)Ljava/lang/Object;", false);
	GetChildrenMethod = FJavaWrapper::FindMethod(Env, MutableDataClass, "GetChildren", "()Ljava/util/ArrayList;", false);
	GetChildrenCountMethod = FJavaWrapper::FindMethod(Env, MutableDataClass, "GetChildrenCount", "()J", false);
	HasChildMethod = FJavaWrapper::FindMethod(Env, MutableDataClass, "HasChild", "(Ljava/lang/String;)Z", false);
	HasChildrenMethod = FJavaWrapper::FindMethod(Env, MutableDataClass, "HasChildren", "()Z", false);
}

MutableDataAndroid::~MutableDataAndroid()
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	Env->DeleteGlobalRef(JMutableData);
}

FString MutableDataAndroid::GetKey() const
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	jstring Key = static_cast<jstring>(Env->CallObjectMethod(JMutableData, GetKeyMethod));
	return FJavaHelper::FStringFromLocalRef(Env, Key);
}

FFGValueVariant MutableDataAndroid::GetPriority() const
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	jobject Data = Env->CallObjectMethod(JMutableData, GetPriorityMethod);
	FFGValueVariant PriorityVariant = ValueVariantUtils::JavaObjectToVariant(Env, Data);
	Env->DeleteLocalRef(Data);
	return PriorityVariant;
}

void MutableDataAndroid::SetPriority(const FFGValueVariant& Priority)
{
	JNIEnv* Env = AndroidJavaEnv::GetJavaEnv();
	jobject JPriority = ValueVariantUtils::VariantToJavaObject(Env, Priority);
	Env->CallVoidMethod(JMutableData, SetPriorityMethod, JPriority);
	Env->DeleteLocalRef(JPriority);
}

FFGValueVariant MutableDataAndroid::GetValue()
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	jobject Data = Env->CallObjectMethod(JMutableData, GetValueMethod);
	FFGValueVariant ValueVariant = ValueVariantUtils::JavaObjectToVariant(Env, Data);
	Env->DeleteLocalRef(Data);
	return ValueVariant;
}

void MutableDataAndroid::SetValue(const FFGValueVariant& Value)
{
	JNIEnv* Env = AndroidJavaEnv::GetJavaEnv();
	jobject JValue = ValueVariantUtils::VariantToJavaObject(Env, Value);
	Env->CallVoidMethod(JMutableData, SetValueMethod, JValue);
	Env->DeleteLocalRef(JValue);
}

TSharedPtr<IMutableData> MutableDataAndroid::Child(const FString& Path) const
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	auto JPath = FJavaClassObject::GetJString(Path);

	jobject JChild = Env->CallObjectMethod(JMutableData, ChildMethod, *JPath);

	MutableDataAndroid* data = new MutableDataAndroid(JChild);
	return MakeShareable(data);
}

TArray<TSharedPtr<IMutableData>> MutableDataAndroid::GetChildren() const
{
	TArray<TSharedPtr<IMutableData>> Children;

	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	jclass ListClass = FJavaWrapper::FindClass(Env, "java/util/List", false);
	jmethodID SizeMethod = FJavaWrapper::FindMethod(Env, ListClass, "size", "()I", false);
	jmethodID GeteMethod = FJavaWrapper::FindMethod(Env, ListClass, "get", "(I)Ljava/lang/Object;", false);
	jobject JChildren = Env->CallObjectMethod(JMutableData, GetChildrenMethod);

	const int Size = Env->CallIntMethod(JChildren, SizeMethod);

	for (int i = 0; i < Size; i++)
	{
		jobject JChild = Env->CallObjectMethod(JChildren, GeteMethod, i);

		MutableDataAndroid* data = new MutableDataAndroid(JChild);
		Children.Add(MakeShareable(data));

		Env->DeleteLocalRef(JChild);
	}
	Env->DeleteLocalRef(JChildren);

	return Children;
}

int MutableDataAndroid::GetChildrenCount() const
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	return Env->CallLongMethod(JMutableData, GetChildrenCountMethod);
}

bool MutableDataAndroid::HasChild(const FString& Path) const
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	auto JPath = FJavaClassObject::GetJString(Path);
	return Env->CallBooleanMethod(JMutableData, HasChildMethod, *JPath);
}

bool MutableDataAndroid::HasChildren() const
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	return Env->CallBooleanMethod(JMutableData, HasChildrenMethod);
}
