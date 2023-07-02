// Copyright (c) 2022 Nineva Studios

#include "DataSnapshotAndroid.h"

#include "Android/Utils/FGValueVariantUtils.h"
#include "Android/Utils/FGMethodCallUtils.h"

#include "Android/AndroidJavaEnv.h"

DataSnapshotAndroid::DataSnapshotAndroid(jobject DataSnapshot)
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	JDataSnapshot = Env->NewGlobalRef(DataSnapshot);

	jclass DataSnapshotClass = FAndroidApplication::FindJavaClass("com/ninevastudios/unrealfirebase/FGDataSnapshot");
	GetKeyMethod = FJavaWrapper::FindMethod(Env, DataSnapshotClass, "GetKey", "()Ljava/lang/String;", false);
	GetPriorityMethod = FJavaWrapper::FindMethod(Env, DataSnapshotClass, "GetPriority", "()Ljava/lang/Object;", false);
	GetValueMethod = FJavaWrapper::FindMethod(Env, DataSnapshotClass, "GetValue", "()Ljava/lang/Object;", false);
	ExistsMethod = FJavaWrapper::FindMethod(Env, DataSnapshotClass, "Exists", "()Z", false);
	ChildMethod = FJavaWrapper::FindMethod(Env, DataSnapshotClass, "Child", "(Ljava/lang/String;)Ljava/lang/Object;", false);
	GetChildrenMethod = FJavaWrapper::FindMethod(Env, DataSnapshotClass, "GetChildren", "()Ljava/util/ArrayList;", false);
	GetChildrenCountMethod = FJavaWrapper::FindMethod(Env, DataSnapshotClass, "GetChildrenCount", "()J", false);
	HasChildMethod = FJavaWrapper::FindMethod(Env, DataSnapshotClass, "HasChild", "(Ljava/lang/String;)Z", false);
	HasChildrenMethod = FJavaWrapper::FindMethod(Env, DataSnapshotClass, "HasChildren", "()Z", false);
}

DataSnapshotAndroid::~DataSnapshotAndroid()
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	Env->DeleteGlobalRef(JDataSnapshot);
}

FString DataSnapshotAndroid::GetKey() const
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	jstring Key = static_cast<jstring>(Env->CallObjectMethod(JDataSnapshot, GetKeyMethod));
	return FJavaHelper::FStringFromLocalRef(Env, Key);
}

FFGValueVariant DataSnapshotAndroid::GetPriority() const
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	jobject Data = Env->CallObjectMethod(JDataSnapshot, GetPriorityMethod);
	FFGValueVariant PriorityVariant = ValueVariantUtils::JavaObjectToVariant(Env, Data);
	Env->DeleteLocalRef(Data);
	return PriorityVariant;
}

FFGValueVariant DataSnapshotAndroid::GetValue()
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	jobject Data = Env->CallObjectMethod(JDataSnapshot, GetValueMethod);
	FFGValueVariant ValueVariant = ValueVariantUtils::JavaObjectToVariant(Env, Data);
	Env->DeleteLocalRef(Data);
	return ValueVariant;
}

bool DataSnapshotAndroid::Exists() const
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	return Env->CallBooleanMethod(JDataSnapshot, ExistsMethod);
}

TSharedPtr<IDataSnapshot> DataSnapshotAndroid::Child(const FString& Path) const
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	auto JPath = FJavaClassObject::GetJString(Path);
	jobject JChild = Env->CallObjectMethod(JDataSnapshot, ChildMethod, *JPath);

	DataSnapshotAndroid* snapshot = new DataSnapshotAndroid(JChild);
	return MakeShareable(snapshot);
}

TArray<TSharedPtr<IDataSnapshot>> DataSnapshotAndroid::GetChildren() const
{
	TArray<TSharedPtr<IDataSnapshot>> Children;
	
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	jclass ListClass = FJavaWrapper::FindClass(Env, "java/util/List", false);
	jmethodID SizeMethod = FJavaWrapper::FindMethod(Env, ListClass, "size", "()I", false);
	jmethodID GeteMethod = FJavaWrapper::FindMethod(Env, ListClass, "get", "(I)Ljava/lang/Object;", false);
	jobject JChildren = Env->CallObjectMethod(JDataSnapshot, GetChildrenMethod);

	const int Size = Env->CallIntMethod(JChildren, SizeMethod);

	for (int i = 0; i < Size; i++)
	{
		jobject JChild = Env->CallObjectMethod(JChildren, GeteMethod, i);

		DataSnapshotAndroid* snapshot = new DataSnapshotAndroid(JChild);
		Children.Add(MakeShareable(snapshot));

		Env->DeleteLocalRef(JChild);
	}
	Env->DeleteLocalRef(JChildren);

	return Children;
}

int DataSnapshotAndroid::GetChildrenCount() const
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	return Env->CallLongMethod(JDataSnapshot, GetChildrenCountMethod);
}

bool DataSnapshotAndroid::HasChild(const FString& Path) const
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	auto JPath = FJavaClassObject::GetJString(Path);
	return Env->CallBooleanMethod(JDataSnapshot, HasChildMethod, *JPath);
}

bool DataSnapshotAndroid::HasChildren() const
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	return Env->CallBooleanMethod(JDataSnapshot, HasChildrenMethod);
}
