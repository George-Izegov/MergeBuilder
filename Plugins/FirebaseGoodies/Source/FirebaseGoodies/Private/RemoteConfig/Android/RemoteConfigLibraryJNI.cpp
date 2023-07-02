// Copyright (c) 2022 Nineva Studios

#include "FirebaseGoodiesLog.h"

#include "RemoteConfig/Callbacks/FGRemoteConfigCallback.h"

#include "Android/Utils/FGJavaConvertor.h"

#include "Android/AndroidApplication.h"
#include "Android/AndroidJNI.h"
#include "Android/AndroidJava.h"

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGRemoteConfig_onSuccessCallback(JNIEnv* env, jclass clazz, jlong objAddr)
{
	UFGRemoteConfigCallback* callback = reinterpret_cast<UFGRemoteConfigCallback*>(objAddr);

	if (IsValid(callback))
	{
		callback->ExecuteSuccess();
		callback->RemoveFromRoot();
	}
	else
	{
		UE_LOG(LogFirebaseGoodies, Error, TEXT("Invalid success callback"));
	}
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGRemoteConfig_onErrorCallback(JNIEnv* env, jclass clazz, jlong objAddr, jstring errorMessage)
{
	UFGRemoteConfigCallback* callback = reinterpret_cast<UFGRemoteConfigCallback*>(objAddr);

	if (IsValid(callback))
	{
		callback->ExecuteError(FGJavaConvertor::FromJavaString(errorMessage));
		callback->RemoveFromRoot();
	}
	else
	{
		UE_LOG(LogFirebaseGoodies, Error, TEXT("Invalid error callback"));
	}
}