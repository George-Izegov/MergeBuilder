// Copyright (c) 2021 Nineva Studios

#include "Firestore/FGFirestoreLibrary.h"
#include "Firestore/FGCollectionReference.h"
#include "Firestore/FGDocumentReference.h"
#include "Firestore/FGDocumentSnapshot.h"
#include "Firestore/FGWriteBatch.h"
#include "Firestore/FGQuery.h"
#include "Firestore/FGQuerySnapshot.h"

#include "Async/Async.h"
#include "FirebaseGoodiesLog.h"

#if PLATFORM_ANDROID
#include "Android/Utils/FGValueVariantUtils.h"
#endif

#if PLATFORM_IOS
#import <FirebaseFirestore/FirebaseFirestore.h>
#include "IOS/Utils/FGValueVariantUtils.h"
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
#include "firebase/firestore.h"
#include "Desktop/FGVariantUtils.h"
#endif

const ANSICHAR* UFGFirestoreLibrary::FGFirestoreClassName = "com/ninevastudios/unrealfirebase/FGFirestore";

UFGCollectionReference* UFGFirestoreLibrary::GetCollection(const FString& CollectionPath)
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGFirestoreLibrary::GetCollection"));
	
	UFGCollectionReference* Result = NewObject<UFGCollectionReference>();
	
	if (CollectionPath.IsEmpty())
	{
		UE_LOG(LogFirebaseGoodies, Error, TEXT("Collection Path should not be empty."));
		return Result;
	}
	
#if PLATFORM_ANDROID
	jobject javaCollection = FGMethodCallUtils::CallStaticObjectMethod(FGFirestoreClassName, "getCollection", "(Ljava/lang/String;)Lcom/google/firebase/firestore/CollectionReference;", 
		*FJavaClassObject::GetJString(CollectionPath));
	Result->Init(javaCollection);
#elif PLATFORM_IOS
	FIRCollectionReference* collection = [[FIRFirestore firestore] collectionWithPath:CollectionPath.GetNSString()];
	Result->Init(collection);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	firebase::firestore::CollectionReference reference = firebase::firestore::Firestore::GetInstance(firebase::App::GetInstance())->Collection(TCHAR_TO_ANSI(*CollectionPath));
	Result->Init(reference);
#endif
	
	return Result;
}

UFGWriteBatch* UFGFirestoreLibrary::GetWriteBatch()
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGFirestoreLibrary::GetWriteBatch"));
	
	UFGWriteBatch* Result = NewObject<UFGWriteBatch>();
	
#if PLATFORM_ANDROID
	jobject javaBatch = FGMethodCallUtils::CallStaticObjectMethod(FGFirestoreClassName, "getBatch", "()Lcom/google/firebase/firestore/WriteBatch;");
	Result->Init(javaBatch);
#elif PLATFORM_IOS
	FIRWriteBatch* batch = [[FIRFirestore firestore] batch];
	Result->Init(batch);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	firebase::firestore::WriteBatch batch = firebase::firestore::Firestore::GetInstance(firebase::App::GetInstance())->batch();
	Result->Init(batch);
#endif
	
	return Result;
}

UFGDocumentReference* UFGFirestoreLibrary::GetDocument(const FString& Path)
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGFirestoreLibrary::GetDocument"));
	
	UFGDocumentReference* Result = NewObject<UFGDocumentReference>();
	
#if PLATFORM_ANDROID
	jobject javaDocument = FGMethodCallUtils::CallStaticObjectMethod(FGFirestoreClassName, "getDocument", "(Ljava/lang/String;)Lcom/google/firebase/firestore/DocumentReference;",
		*FJavaClassObject::GetJString(Path));
	Result->Init(javaDocument);
#elif PLATFORM_IOS
	FIRDocumentReference* document = [[FIRFirestore firestore] documentWithPath:Path.GetNSString()];
	Result->Init(document);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	firebase::firestore::DocumentReference reference = firebase::firestore::Firestore::GetInstance(firebase::App::GetInstance())->Document(TCHAR_TO_ANSI(*Path));
	Result->Init(reference);
#endif
	
	return Result;
}

UFGQuery* UFGFirestoreLibrary::GetCollectionQuery(const FString& CollectionId)
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGFirestoreLibrary::GetCollectionQuery"));
	
	UFGQuery* Result = NewObject<UFGQuery>();

#if PLATFORM_ANDROID
	jobject javaQuery = FGMethodCallUtils::CallStaticObjectMethod(FGFirestoreClassName, "getCollectionQuery", "(Ljava/lang/String;)Lcom/google/firebase/firestore/Query;",
		*FJavaClassObject::GetJString(CollectionId));
	Result->InitQuery(javaQuery);
#elif PLATFORM_IOS
	FIRQuery* query = [[FIRFirestore firestore] collectionGroupWithID:CollectionId.GetNSString()];
	Result->InitQuery(query);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	firebase::firestore::Query query = firebase::firestore::Firestore::GetInstance(firebase::App::GetInstance())->CollectionGroup(TCHAR_TO_ANSI(*CollectionId));
	Result->InitQuery(query);
#endif

	return Result;
}

FFGValueVariant UFGFirestoreLibrary::FirestoreTimestamp()
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGFirestoreLibrary::FirestoreTimestamp"));

	FFGValueVariant Result;

	Result.Type = EValueType::ServerTimestamp;

	return Result;
}

#if PLATFORM_ANDROID

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGFirestore_OnItemAddSuccess(JNIEnv* env, jclass clazz, jlong objAddr, jobject documentReference)
{
	UFGDocumentCallback* Callback = reinterpret_cast<UFGDocumentCallback*>(objAddr);

	if (IsValid(Callback))
	{
		UFGDocumentReference* Result = NewObject<UFGDocumentReference>();
		Result->Init(documentReference);
		Callback->ExecuteDelegate(Result);
	}
	else
	{
		UE_LOG(LogFirebaseGoodies, Error, TEXT("Invalid callback"));
	}
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGFirestore_OnItemAddError(JNIEnv* env, jclass clazz, jlong objAddr, jstring error)
{
	UFGFirestoreStringCallback* Callback = reinterpret_cast<UFGFirestoreStringCallback*>(objAddr);

	if (IsValid(Callback))
	{
		FString ErrorMessage = FGJavaConvertor::FromJavaString(error);
		Callback->ExecuteDelegate(ErrorMessage);
	}
	else
	{
		UE_LOG(LogFirebaseGoodies, Error, TEXT("Invalid callback"));
	}
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGFirestore_OnSetDocumentDataSuccess(JNIEnv* env, jclass clazz, jlong objAddr)
{
	UFGFirestoreVoidCallback* Callback = reinterpret_cast<UFGFirestoreVoidCallback*>(objAddr);

	if (IsValid(Callback))
	{
		Callback->ExecuteDelegate();
	}
	else
	{
		UE_LOG(LogFirebaseGoodies, Error, TEXT("Invalid callback"));
	}
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGFirestore_OnSetDocumentDataError(JNIEnv* env, jclass clazz, jlong objAddr, jstring error)
{
	UFGFirestoreStringCallback* Callback = reinterpret_cast<UFGFirestoreStringCallback*>(objAddr);

	if (IsValid(Callback))
	{
		FString ErrorMessage = FGJavaConvertor::FromJavaString(error);
		Callback->ExecuteDelegate(ErrorMessage);
	}
	else
	{
		UE_LOG(LogFirebaseGoodies, Error, TEXT("Invalid callback"));
	}
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGFirestore_OnUpdateDocumentDataSuccess(JNIEnv* env, jclass clazz, jlong objAddr)
{
	UFGFirestoreVoidCallback* Callback = reinterpret_cast<UFGFirestoreVoidCallback*>(objAddr);

	if (IsValid(Callback))
	{
		Callback->ExecuteDelegate();
	}
	else
	{
		UE_LOG(LogFirebaseGoodies, Error, TEXT("Invalid callback"));
	}
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGFirestore_OnUpdateDocumentDataError(JNIEnv* env, jclass clazz, jlong objAddr, jstring error)
{
	UFGFirestoreStringCallback* Callback = reinterpret_cast<UFGFirestoreStringCallback*>(objAddr);

	if (IsValid(Callback))
	{
		FString ErrorMessage = FGJavaConvertor::FromJavaString(error);
		Callback->ExecuteDelegate(ErrorMessage);
	}
	else
	{
		UE_LOG(LogFirebaseGoodies, Error, TEXT("Invalid callback"));
	}
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGFirestore_OnDeleteDocumentDataSuccess(JNIEnv* env, jclass clazz, jlong objAddr)
{
	UFGFirestoreVoidCallback* Callback = reinterpret_cast<UFGFirestoreVoidCallback*>(objAddr);

	if (IsValid(Callback))
	{
		Callback->ExecuteDelegate();
	}
	else
	{
		UE_LOG(LogFirebaseGoodies, Error, TEXT("Invalid callback"));
	}
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGFirestore_OnDeleteDocumentDataError(JNIEnv* env, jclass clazz, jlong objAddr, jstring error)
{
	UFGFirestoreStringCallback* Callback = reinterpret_cast<UFGFirestoreStringCallback*>(objAddr);

	if (IsValid(Callback))
	{
		FString ErrorMessage = FGJavaConvertor::FromJavaString(error);
		Callback->ExecuteDelegate(ErrorMessage);
	}
	else
	{
		UE_LOG(LogFirebaseGoodies, Error, TEXT("Invalid callback"));
	}
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGFirestore_OnGetDocumentSnapshotSuccess(JNIEnv* env, jclass clazz, jlong objAddr, jobject data)
{
	UFGDocumentSnapshotCallback* Callback = reinterpret_cast<UFGDocumentSnapshotCallback*>(objAddr);

	if (IsValid(Callback))
	{
		UFGDocumentSnapshot* Snapshot = NewObject<UFGDocumentSnapshot>();
		Snapshot->Init(data);
		Callback->ExecuteDelegate(Snapshot);
	}
	else
	{
		UE_LOG(LogFirebaseGoodies, Error, TEXT("Invalid callback"));
	}
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGFirestore_OnGetDocumentSnapshotError(JNIEnv* env, jclass clazz, jlong objAddr, jstring error)
{
	UFGFirestoreStringCallback* Callback = reinterpret_cast<UFGFirestoreStringCallback*>(objAddr);

	if (IsValid(Callback))
	{
		FString ErrorMessage = FGJavaConvertor::FromJavaString(error);
		Callback->ExecuteDelegate(ErrorMessage);
	}
	else
	{
		UE_LOG(LogFirebaseGoodies, Error, TEXT("Invalid callback"));
	}
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGFirestore_OnCommitBatchSuccess(JNIEnv* env, jclass clazz, jlong objAddr)
{
	UFGFirestoreVoidCallback* Callback = reinterpret_cast<UFGFirestoreVoidCallback*>(objAddr);

	if (IsValid(Callback))
	{
		Callback->ExecuteDelegate();
	}
	else
	{
		UE_LOG(LogFirebaseGoodies, Error, TEXT("Invalid callback"));
	}
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGFirestore_OnCommitBatchError(JNIEnv* env, jclass clazz, jlong objAddr, jstring error)
{
	UFGFirestoreStringCallback* Callback = reinterpret_cast<UFGFirestoreStringCallback*>(objAddr);

	if (IsValid(Callback))
	{
		FString ErrorMessage = FGJavaConvertor::FromJavaString(error);
		Callback->ExecuteDelegate(ErrorMessage);
	}
	else
	{
		UE_LOG(LogFirebaseGoodies, Error, TEXT("Invalid callback"));
	}
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGFirestore_OnGetQuerySnapshotSuccess(JNIEnv* env, jclass clazz, jlong objAddr, jobject data)
{
	UFGQuerySnapshotCallback* Callback = reinterpret_cast<UFGQuerySnapshotCallback*>(objAddr);

	if (IsValid(Callback))
	{
		UFGQuerySnapshot* Snapshot = NewObject<UFGQuerySnapshot>();
		Snapshot->Init(data);
		Callback->ExecuteDelegate(Snapshot);
	}
	else
	{
		UE_LOG(LogFirebaseGoodies, Error, TEXT("Invalid callback"));
	}
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGFirestore_OnGetQuerySnapshotError(JNIEnv* env, jclass clazz, jlong objAddr, jstring error)
{
	UFGFirestoreStringCallback* Callback = reinterpret_cast<UFGFirestoreStringCallback*>(objAddr);

	if (IsValid(Callback))
	{
		FString ErrorMessage = FGJavaConvertor::FromJavaString(error);
		Callback->ExecuteDelegate(ErrorMessage);
	}
	else
	{
		UE_LOG(LogFirebaseGoodies, Error, TEXT("Invalid callback"));
	}
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGFirestore_OnDocumentReferenceSnapshotEvent(JNIEnv* env, jclass clazz, jobject data, jlong documentRefPointer)
{
	UFGDocumentReference* DocumentReference = reinterpret_cast<UFGDocumentReference*>(documentRefPointer);
	
	if (DocumentReference == nullptr) {
		return;
	}
	
	UFGDocumentSnapshot* Snapshot = NewObject<UFGDocumentSnapshot>();
	Snapshot->Init(data);

	AsyncTask(ENamedThreads::GameThread, [=]() {
		DocumentReference->OnListenerEventCallback.ExecuteIfBound(Snapshot);
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGFirestore_OnDocumentReferenceSnapshotEventError(JNIEnv* env, jclass clazz, jint errorCode, jlong documentRefPointer)
{
	UFGDocumentReference* DocumentReference = reinterpret_cast<UFGDocumentReference*>(documentRefPointer);
	
	if (DocumentReference == nullptr) {
		return;
	}
	
	EFirestoreErrorCode Code = (EFirestoreErrorCode) errorCode;

	AsyncTask(ENamedThreads::GameThread, [=]() {
		DocumentReference->OnListenerEventErrorCallback.ExecuteIfBound(Code);
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGFirestore_OnQuerySnapshotEvent(JNIEnv* env, jclass clazz, jobject data, jlong queryRefPointer)
{
	UFGQuery* Query = reinterpret_cast<UFGQuery*>(queryRefPointer);
	
	if (Query == nullptr) {
		return;
	}
	
	UFGQuerySnapshot* Snapshot = NewObject<UFGQuerySnapshot>();
	Snapshot->Init(data);

	AsyncTask(ENamedThreads::GameThread, [=]() {
		Query->OnListenerEventCallback.ExecuteIfBound(Snapshot);
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGFirestore_OnQuerySnapshotEventError(JNIEnv* env, jclass clazz, jint errorCode, jlong queryRefPointer)
{
	UFGQuery* Query = reinterpret_cast<UFGQuery*>(queryRefPointer);
	
	if (Query == nullptr) {
		return;
	}
	
	EFirestoreErrorCode Code = (EFirestoreErrorCode) errorCode;

	AsyncTask(ENamedThreads::GameThread, [=]() {
		Query->OnListenerEventErrorCallback.ExecuteIfBound(Code);
	});
}

#endif
