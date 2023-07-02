// Copyright (c) 2022 Nineva Studios

#include "DatabaseRefAndroid.h"

#include "Android/Utils/FGValueVariantUtils.h"

#include "Database/Callback/FGDatabaseRefCallback.h"

#include "Android/AndroidJavaEnv.h"

DatabaseRefAndroid::DatabaseRefAndroid()
	: TransactionTask(nullptr)
{
}

DatabaseRefAndroid::~DatabaseRefAndroid()
{
}

void DatabaseRefAndroid::Init(const FString& DatabaseUrl, const FString& InitialPath)
{
	auto JUrl = FJavaClassObject::GetJString(DatabaseUrl);
	auto JPath = FJavaClassObject::GetJString(InitialPath);
	DatabaseRefJObject = MakeShared<FJavaClassObject>(
		"com/ninevastudios/unrealfirebase/FGDatabaseRef", "(Ljava/lang/String;Ljava/lang/String;)V", *JUrl , *JPath);

	SetValueMethod = DatabaseRefJObject->GetClassMethod("SetValue", "(Ljava/lang/Object;Ljava/lang/Object;)V");
	SetTimestampMethod = DatabaseRefJObject->GetClassMethod("SetTimestamp", "()V");
	SetPriorityMethod = DatabaseRefJObject->GetClassMethod("SetPriority", "(Ljava/lang/Object;)V");
	GetKeyMethod = DatabaseRefJObject->GetClassMethod("GetKey", "()Ljava/lang/String;");
	GetValueMethod = DatabaseRefJObject->GetClassMethod("GetValue", "(J)V");
	PushMethod = DatabaseRefJObject->GetClassMethod("Push", "()Ljava/lang/String;");
	RemoveValueMethod = DatabaseRefJObject->GetClassMethod("RemoveValue", "()V");
	UpdateChildrenMethod = DatabaseRefJObject->GetClassMethod("UpdateChildren", "(Ljava/util/HashMap;)V");
	AddValueListenerMethod = DatabaseRefJObject->GetClassMethod("AddValueListener", "(J)V");
	RemoveValueListenerMethod = DatabaseRefJObject->GetClassMethod("RemoveValueListener", "()V");
	AddChildListenerMethod = DatabaseRefJObject->GetClassMethod("AddChildListener", "(J)V");
	RemoveChildListenerMethod = DatabaseRefJObject->GetClassMethod("RemoveChildListener", "()V");
	KeepSyncedMethod = DatabaseRefJObject->GetClassMethod("KeepSynced", "(Z)V");
	RunTransactionMethod = DatabaseRefJObject->GetClassMethod("RunTransaction", "(J)V");
	OrderByChildMethod = DatabaseRefJObject->GetClassMethod("OrderByChild", "(Ljava/lang/String;)V");
	OrderByKeyMethod = DatabaseRefJObject->GetClassMethod("OrderByKey", "()V");
	OrderByPriorityMethod = DatabaseRefJObject->GetClassMethod("OrderByPriority", "()V");
	OrderByValueMethod = DatabaseRefJObject->GetClassMethod("OrderByValue", "()V");
	StartAtFloatMethod = DatabaseRefJObject->GetClassMethod("StartAtFloat", "(DLjava/lang/String;)V");
	StartAtStringMethod = DatabaseRefJObject->GetClassMethod("StartAtString", "(Ljava/lang/String;Ljava/lang/String;)V");
	EndAtFloatMethod = DatabaseRefJObject->GetClassMethod("EndAtFloat", "(DLjava/lang/String;)V");
	EndAtStringMethod = DatabaseRefJObject->GetClassMethod("EndAtString", "(Ljava/lang/String;Ljava/lang/String;)V");
	EqualToFloatMethod = DatabaseRefJObject->GetClassMethod("EqualToFloat", "(DLjava/lang/String;)V");
	EqualToBoolMethod = DatabaseRefJObject->GetClassMethod("EqualToBool", "(ZLjava/lang/String;)V");
	EqualToStringMethod = DatabaseRefJObject->GetClassMethod("EqualToString", "(Ljava/lang/String;Ljava/lang/String;)V");
	LimitToFirstMethod = DatabaseRefJObject->GetClassMethod("LimitToFirst", "(I)V");
	LimitToLastMethod = DatabaseRefJObject->GetClassMethod("LimitToLast", "(I)V");
	OnDisconnectSetValueMethod = DatabaseRefJObject->GetClassMethod("OnDisconnectSetValue", "(Ljava/lang/Object;)V");
	OnDisconnectSetTimestampMethod = DatabaseRefJObject->GetClassMethod("OnDisconnectSetTimestamp", "()V");
	OnDisconnectRemoveValueMethod = DatabaseRefJObject->GetClassMethod("OnDisconnectRemoveValue", "()V");
	OnDisconnectUpdateChildrenMethod = DatabaseRefJObject->GetClassMethod("OnDisconnectUpdateChildren", "(Ljava/util/HashMap;)V");
	CancelDisconnectOpsMethod = DatabaseRefJObject->GetClassMethod("CancelDisconnectOperations", "()V");
}

void DatabaseRefAndroid::SetValue(const FFGValueVariant& Value, const FFGValueVariant& Priority)
{
	JNIEnv* JEnv = AndroidJavaEnv::GetJavaEnv();
	jobject JVal = ValueVariantUtils::VariantToJavaObject(JEnv, Value);
	jobject JPriority = ValueVariantUtils::VariantToJavaObject(JEnv, Priority);

	DatabaseRefJObject->CallMethod<void>(SetValueMethod, JVal, JPriority);

	JEnv->DeleteLocalRef(JVal);
	JEnv->DeleteLocalRef(JPriority);
}

void DatabaseRefAndroid::SetTimestamp()
{
	DatabaseRefJObject->CallMethod<void>(SetTimestampMethod);
}

void DatabaseRefAndroid::SetPriority(const FFGValueVariant& Priority)
{
	JNIEnv* JEnv = AndroidJavaEnv::GetJavaEnv();
	jobject JPriority = ValueVariantUtils::VariantToJavaObject(JEnv, Priority);

	DatabaseRefJObject->CallMethod<void>(SetPriorityMethod, JPriority);

	JEnv->DeleteLocalRef(JPriority);
}

FString DatabaseRefAndroid::GetKey() const
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	jstring Key = static_cast<jstring>(DatabaseRefJObject->CallMethod<jobject>(GetKeyMethod));
	return FJavaHelper::FStringFromGlobalRef(Env, Key);
}

void DatabaseRefAndroid::GetValue(const FOnDataChangedDelegate& OnDataReceived, const FOnCancelledDelegate& OnCancelled)
{
	UFGDatabaseRefCallback* NativeCallback = NewObject<UFGDatabaseRefCallback>();
	NativeCallback->BindDataChangedDelegate(OnDataReceived);
	NativeCallback->BindCancelDelegate(OnCancelled);
	
	DatabaseRefJObject->CallMethod<void>(GetValueMethod, (jlong)NativeCallback);
}

void DatabaseRefAndroid::RemoveValue()
{
	DatabaseRefJObject->CallMethod<void>(RemoveValueMethod);
}

FString DatabaseRefAndroid::Push()
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	jstring NewChild = static_cast<jstring>(DatabaseRefJObject->CallMethod<jobject>(PushMethod));
	return FJavaHelper::FStringFromGlobalRef(Env, NewChild);
}

void DatabaseRefAndroid::UpdateChildren(const TMap<FString, FFGValueVariant>& Update)
{
	FFGValueVariant MapWrapper;
	MapWrapper.Type = EValueType::Map;
	MapWrapper.Value = Update;

	JNIEnv* JEnv = AndroidJavaEnv::GetJavaEnv();
	jobject JMap = ValueVariantUtils::VariantToJavaObject(JEnv, MapWrapper);

	DatabaseRefJObject->CallMethod<void>(UpdateChildrenMethod, JMap);

	JEnv->DeleteLocalRef(JMap);
}

void DatabaseRefAndroid::AddValueListener(const FOnDataChangedDelegate& OnDataChanged, const FOnCancelledDelegate& OnCancelled)
{
	UFGDatabaseRefCallback* NativeCallback = NewObject<UFGDatabaseRefCallback>();
	NativeCallback->BindDataChangedDelegate(OnDataChanged);
	NativeCallback->BindCancelDelegate(OnCancelled);

	DatabaseRefJObject->CallMethod<void>(AddValueListenerMethod, (jlong)NativeCallback);
}

void DatabaseRefAndroid::RemoveValueListener()
{
	DatabaseRefJObject->CallMethod<void>(RemoveValueListenerMethod);
}

void DatabaseRefAndroid::AddChildListener(const FOnChildEventDelegate& OnChildEvent, const FOnCancelledDelegate& OnCancelled)
{
	UFGDatabaseRefCallback* NativeCallback = NewObject<UFGDatabaseRefCallback>();
	NativeCallback->BindOnChildEventDelegate(OnChildEvent);
	NativeCallback->BindCancelDelegate(OnCancelled);

	DatabaseRefJObject->CallMethod<void>(AddChildListenerMethod, (jlong)NativeCallback);
}

void DatabaseRefAndroid::RemoveChildListener()
{
	DatabaseRefJObject->CallMethod<void>(RemoveChildListenerMethod);
}

void DatabaseRefAndroid::KeepSynced(bool Sync)
{
	DatabaseRefJObject->CallMethod<void>(KeepSyncedMethod, Sync);
}

void DatabaseRefAndroid::OrderByChild(const FString& Path)
{
	auto JPath = FJavaClassObject::GetJString(Path);
	DatabaseRefJObject->CallMethod<void>(OrderByChildMethod, *JPath);
}

void DatabaseRefAndroid::OrderByKey()
{
	DatabaseRefJObject->CallMethod<void>(OrderByKeyMethod);
}

void DatabaseRefAndroid::OrderByPriority()
{
	DatabaseRefJObject->CallMethod<void>(OrderByPriorityMethod);
}

void DatabaseRefAndroid::OrderByValue()
{
	DatabaseRefJObject->CallMethod<void>(OrderByValueMethod);
}

void DatabaseRefAndroid::EndAtFloat(float Value, const FString& Key)
{
	auto JKey = FJavaClassObject::GetJString(Key);
	DatabaseRefJObject->CallMethod<void>(EndAtFloatMethod, Value, *JKey);
}

void DatabaseRefAndroid::EndAtString(const FString& Value, const FString& Key)
{
	auto JValue = FJavaClassObject::GetJString(Value);
	auto JKey = FJavaClassObject::GetJString(Key);
	DatabaseRefJObject->CallMethod<void>(EndAtStringMethod, *JValue, *JKey);
}

void DatabaseRefAndroid::StartAtFloat(float Value, const FString& Key)
{
	auto JKey = FJavaClassObject::GetJString(Key);
	DatabaseRefJObject->CallMethod<void>(StartAtFloatMethod, Value, *JKey);
}

void DatabaseRefAndroid::StartAtString(const FString& Value, const FString& Key)
{
	auto JValue = FJavaClassObject::GetJString(Value);
	auto JKey = FJavaClassObject::GetJString(Key);
	DatabaseRefJObject->CallMethod<void>(StartAtStringMethod, *JValue, *JKey);
}

void DatabaseRefAndroid::EqualToFloat(float Value, const FString& Key)
{
	auto JKey = FJavaClassObject::GetJString(Key);
	DatabaseRefJObject->CallMethod<void>(EqualToFloatMethod, Value, *JKey);
}

void DatabaseRefAndroid::EqualToBool(bool Value, const FString& Key)
{
	auto JKey = FJavaClassObject::GetJString(Key);
	DatabaseRefJObject->CallMethod<void>(EqualToBoolMethod, Value, *JKey);
}

void DatabaseRefAndroid::EqualToString(const FString& Value, const FString& Key)
{
	auto JValue = FJavaClassObject::GetJString(Value);
	auto JKey = FJavaClassObject::GetJString(Key);
	DatabaseRefJObject->CallMethod<void>(EqualToStringMethod, *JValue, *JKey);
}

void DatabaseRefAndroid::LimitToFirst(int Limit)
{
	DatabaseRefJObject->CallMethod<void>(LimitToFirstMethod, Limit);
}

void DatabaseRefAndroid::LimitToLast(int Limit)
{
	DatabaseRefJObject->CallMethod<void>(LimitToLastMethod, Limit);
}

void DatabaseRefAndroid::OnDisconnectSetValue(const FFGValueVariant& Value)
{
	JNIEnv* JEnv = AndroidJavaEnv::GetJavaEnv();
	jobject JVal = ValueVariantUtils::VariantToJavaObject(JEnv, Value);

	DatabaseRefJObject->CallMethod<void>(OnDisconnectSetValueMethod, JVal);

	JEnv->DeleteLocalRef(JVal);
}

void DatabaseRefAndroid::OnDisconnectSetTimestamp()
{
	DatabaseRefJObject->CallMethod<void>(OnDisconnectSetTimestampMethod);
}

void DatabaseRefAndroid::OnDisconnectRemoveValue()
{
	DatabaseRefJObject->CallMethod<void>(OnDisconnectRemoveValueMethod);
}

void DatabaseRefAndroid::OnDisconnectUpdateChildren(const TMap<FString, FFGValueVariant>& Update)
{
	FFGValueVariant MapWrapper;
	MapWrapper.Type = EValueType::Map;
	MapWrapper.Value = Update;

	JNIEnv* JEnv = AndroidJavaEnv::GetJavaEnv();
	jobject JMap = ValueVariantUtils::VariantToJavaObject(JEnv, MapWrapper);

	DatabaseRefJObject->CallMethod<void>(OnDisconnectUpdateChildrenMethod, JMap);

	JEnv->DeleteLocalRef(JMap);
}

void DatabaseRefAndroid::CancelDisconnectOperations()
{
	DatabaseRefJObject->CallMethod<void>(CancelDisconnectOpsMethod);
}

void DatabaseRefAndroid::BindTransactionTask(UFGTransactionTask* Task)
{
	TransactionTask = Task;
}

void DatabaseRefAndroid::RunTransaction()
{
	DatabaseRefJObject->CallMethod<void>(RunTransactionMethod, (jlong)TransactionTask);
}
