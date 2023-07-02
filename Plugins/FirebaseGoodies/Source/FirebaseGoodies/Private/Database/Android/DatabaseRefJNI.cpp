// Copyright (c) 2022 Nineva Studios

#include "DataSnapshotAndroid.h"
#include "MutableDataAndroid.h"

#include "Database/FGDataSnapshot.h"
#include "Database/FGDatabaseRef.h"
#include "Database/FGMutableData.h"
#include "Database/FGTransactionTask.h"
#include "Database/Callback/FGDatabaseRefCallback.h" 

#include "FirebaseGoodiesLog.h"

#include "Async/Async.h"

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGDatabaseValueEventListener_onDataChangedCallback(
	JNIEnv* env, jclass clazz, jlong callbackAddr, jobject dataSnapshot)
{
	UFGDatabaseRefCallback* callback = reinterpret_cast<UFGDatabaseRefCallback*>(callbackAddr);

	UFGDataSnapshot* data = NewObject<UFGDataSnapshot>();
	DataSnapshotAndroid* snapshot = new DataSnapshotAndroid(dataSnapshot);
	data->Init(MakeShareable(snapshot));

	AsyncTask(ENamedThreads::GameThread, [=]() {
		callback->ExecuteDataChanged(data);
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGDatabaseValueEventListener_onCancelledCallback(
	JNIEnv* env, jclass clazz, jlong callbackAddr, int errorCode, jstring errorMessage)
{
	UFGDatabaseRefCallback* callback = reinterpret_cast<UFGDatabaseRefCallback*>(callbackAddr);
	FString ErrorMessage = FJavaHelper::FStringFromParam(env, errorMessage);

	UE_LOG(LogFirebaseGoodies, Error, TEXT("Database operation failed: %s"), *ErrorMessage);
	AsyncTask(ENamedThreads::GameThread, [=]() {
		callback->ExecuteCancel(errorCode, ErrorMessage);
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGDatabaseChildEventListener_onChildEventCallback(
	JNIEnv* env, jclass clazz, jlong objAddr, int eventType, jobject dataSnapshot, jstring prevChildName)
{
	UFGDatabaseRefCallback* callback = reinterpret_cast<UFGDatabaseRefCallback*>(objAddr);

	UFGDataSnapshot* data = NewObject<UFGDataSnapshot>();
	DataSnapshotAndroid* snapshot = new DataSnapshotAndroid(dataSnapshot);	
	data->Init(MakeShareable(snapshot));

	FString PrevChildName = FJavaHelper::FStringFromParam(env, prevChildName);

	AsyncTask(ENamedThreads::GameThread, [=]() {
		callback->ExecuteOnChildEvent((EChileEventType)eventType, data, PrevChildName);
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGDatabaseChildEventListener_onChildCancelledCallback(
	JNIEnv* env, jclass clazz, jlong objAddr, int errorCode, jstring errorMessage)
{
	UFGDatabaseRefCallback* callback = reinterpret_cast<UFGDatabaseRefCallback*>(objAddr);
	FString ErrorMessage = FJavaHelper::FStringFromParam(env, errorMessage);

	UE_LOG(LogFirebaseGoodies, Error, TEXT("Database operation failed: %s"), *ErrorMessage);
	AsyncTask(ENamedThreads::GameThread, [=]() {
		callback->ExecuteCancel(errorCode, ErrorMessage);
	});
}

JNI_METHOD jlong Java_com_ninevastudios_unrealfirebase_FGDatabaseRef_doTransactionCallback(
	JNIEnv* env, jclass clazz, jlong objAddr, jobject mutableData)
{
	UFGTransactionTask* transaction = reinterpret_cast<UFGTransactionTask*>(objAddr);

	UFGMutableData* data = NewObject<UFGMutableData>();
	MutableDataAndroid* mutableDataPtr = new MutableDataAndroid(mutableData);
	data->Init(MakeShareable(mutableDataPtr));

	return transaction->RunHandler(data);
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGDatabaseRef_onTransactionCompleteCallback(
	JNIEnv* env, jclass clazz, jlong objAddr, jboolean isSuccess, jstring errorMessage)
{
	UFGTransactionTask* transaction = reinterpret_cast<UFGTransactionTask*>(objAddr);

	if (isSuccess)
	{
		AsyncTask(ENamedThreads::GameThread, [=]() {
			transaction->Complete.Broadcast(TEXT(""));
		});
	}
	else
	{
		FString ErrorMessage = FJavaHelper::FStringFromParam(env, errorMessage);
		AsyncTask(ENamedThreads::GameThread, [=]() {
			transaction->Abort.Broadcast(ErrorMessage);
		});
	}
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGDatabaseRef_onQueryErrorCallback(
	JNIEnv* env, jclass clazz, jstring errorMessage)
{
	FString ErrorMessage = FJavaHelper::FStringFromParam(env, errorMessage);
	UE_LOG(LogFirebaseGoodies, Error, TEXT("Query failed: %s"), *ErrorMessage);
}