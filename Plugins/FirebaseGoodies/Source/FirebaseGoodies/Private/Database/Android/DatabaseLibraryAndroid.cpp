// Copyright (c) 2022 Nineva Studios

#include "DatabaseLibraryAndroid.h"

#include "Android/Utils/FGMethodCallUtils.h"
#include "Android/Utils/FGValueVariantUtils.h"

static const ANSICHAR* FGDatabaseLibraryClassName = "com/ninevastudios/unrealfirebase/FGDatabaseLibrary";

void DatabaseLibraryAndroid::GoOffline()
{
	FGMethodCallUtils::CallStaticVoidMethod(FGDatabaseLibraryClassName, "GoOffline", "()V");
}

void DatabaseLibraryAndroid::GoOnline()
{
	FGMethodCallUtils::CallStaticVoidMethod(FGDatabaseLibraryClassName, "GoOnline", "()V");
}

void DatabaseLibraryAndroid::PurgeOutstandingWrites()
{
	FGMethodCallUtils::CallStaticVoidMethod(FGDatabaseLibraryClassName, "PurgeOutstandingWrites", "()V");
}

FFGValueVariant DatabaseLibraryAndroid::RealtimeDatabaseTimestamp()
{
	jobject javaTimestamp = FGMethodCallUtils::CallStaticObjectMethod(FGDatabaseLibraryClassName, "Timestamp", "()Ljava/util/HashMap;");
	JNIEnv* JEnv = AndroidJavaEnv::GetJavaEnv();
	return ValueVariantUtils::JavaObjectToVariant(JEnv, javaTimestamp);
}

void DatabaseLibraryAndroid::SetPersistenceCacheSizeBytes(int SizeInBytes)
{
	FGMethodCallUtils::CallStaticVoidMethod(FGDatabaseLibraryClassName, "SetPersistenceCacheSizeBytes", "(J)V", (jlong)SizeInBytes);
}

void DatabaseLibraryAndroid::SetPersistenceEnabled(bool bEnabled)
{
	FGMethodCallUtils::CallStaticVoidMethod(FGDatabaseLibraryClassName, "SetPersistenceEnabled", "(Z)V", bEnabled);
}
