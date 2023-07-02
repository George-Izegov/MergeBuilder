#include "Firestore/FGDocumentReference.h"

#include "Callbacks/FGFirestoreCallbacks.h"
#include "Firestore/FGCollectionReference.h"
#include "Firestore/FGListenerRegistration.h"
#include "Firestore/FGDocumentSnapshot.h"

#include "FirebaseGoodiesLog.h"

#include "Async/Async.h"

#if PLATFORM_IOS
#include "IOS/Utils/FGValueVariantUtils.h"
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
#include "Desktop/FGVariantUtils.h"
#include "Desktop/FGUtils.h"
#endif

UFGDocumentReference::~UFGDocumentReference()
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGDocumentReference::~UFGDocumentReference"));

	if (!IsDocumentReferenceValid())
	{
		return;
	}

#if PLATFORM_ANDROID
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	Env->DeleteGlobalRef(JavaDocumentReferenceObject);
	JavaDocumentReferenceObject = nullptr;
#elif PLATFORM_IOS
	(FIRDocumentReference*)CFBridgingRelease(DocumentReferenceObject);
	DocumentReferenceObject = nil;
#endif
}

FString UFGDocumentReference::GetId()
{
	FString Result;

	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGDocumentReference::GetId"));

	if (!IsDocumentReferenceValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result = FGMethodCallUtils::CallStringMethod(JavaDocumentReferenceObject, "getId", "()Ljava/lang/String;");
#elif PLATFORM_IOS
	Result = FString([DocumentReferenceObject documentID]);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	Result = DocumentReference.id().c_str();
#endif

	return Result;
}

UFGCollectionReference* UFGDocumentReference::GetParent()
{
	UFGCollectionReference* Result = NewObject<UFGCollectionReference>();

	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGDocumentReference::GetParent"));

	if (!IsDocumentReferenceValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result->Init(FGMethodCallUtils::CallObjectMethod(JavaDocumentReferenceObject, "getParent", "()Lcom/google/firebase/firestore/CollectionReference;"));
#elif PLATFORM_IOS
	Result->Init([DocumentReferenceObject parent]);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	Result->Init(DocumentReference.Parent());
#endif

	return Result;
}

FString UFGDocumentReference::GetPath()
{
	FString Result;

	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGDocumentReference::GetPath"));

	if (!IsDocumentReferenceValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result = FGMethodCallUtils::CallStringMethod(JavaDocumentReferenceObject, "getPath", "()Ljava/lang/String;");
#elif PLATFORM_IOS
	Result = FString([DocumentReferenceObject path]);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	Result = DocumentReference.path().c_str();
#endif

	return Result;
}

UFGCollectionReference* UFGDocumentReference::GetCollection(const FString& Path)
{
	UFGCollectionReference* Result = NewObject<UFGCollectionReference>();

	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGDocumentReference::GetCollection"));

	if (!IsDocumentReferenceValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result->Init(FGMethodCallUtils::CallObjectMethod(JavaDocumentReferenceObject, "collection", "(Ljava/lang/String;)Lcom/google/firebase/firestore/CollectionReference;", *FJavaClassObject::GetJString(Path)));
#elif PLATFORM_IOS
	Result->Init([DocumentReferenceObject collectionWithPath:Path.GetNSString()]);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	Result->Init(DocumentReference.Collection(TCHAR_TO_ANSI(*Path)));
#endif

	return Result;
}

void UFGDocumentReference::SetData(const TMap<FString, FFGValueVariant>& Data, const FFirestoreVoidDelegate& OnSuccess, const FFirestoreStringDelegate& OnError, EDocumentSetOptions SetOptions)
{
	UFGFirestoreVoidCallback* SuccessCallback = NewObject<UFGFirestoreVoidCallback>();
	SuccessCallback->BindDelegate(OnSuccess);
	UFGFirestoreStringCallback* ErrorCallback = NewObject<UFGFirestoreStringCallback>();
	ErrorCallback->BindDelegate(OnError);

	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGDocumentReference::SetData"));

	if (!IsDocumentReferenceValid())
	{
		return;
	}

#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(UFGFirestoreLibrary::FGFirestoreClassName, "setDocumentData", "(Lcom/google/firebase/firestore/DocumentReference;Ljava/lang/Object;IJJ)V",
		JavaDocumentReferenceObject, ValueVariantUtils::VariantMapToJavaMap(FAndroidApplication::GetJavaEnv(), Data), (int) SetOptions, (jlong) SuccessCallback, (jlong) ErrorCallback);
#elif PLATFORM_IOS
	NSDictionary* data = ValueVariantUtils::VariantMapToNSDictionary(Data);
	
	[DocumentReferenceObject setData:data
							   merge:SetOptions == EDocumentSetOptions::Merge
						  completion:^(NSError *_Nullable error) {
		if (error == nil) {
			SuccessCallback->ExecuteDelegate();
		} else {
			FString Error = FString([error localizedDescription]);
			ErrorCallback->ExecuteDelegate(Error);
		}
	}];
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	auto setOptions = SetOptions == EDocumentSetOptions::Merge ? firebase::firestore::SetOptions::Merge() : firebase::firestore::SetOptions();
	auto task = DocumentReference.Set(FGVariantUtils::ValueVariantMapToFirebaseMapFieldValue(Data), setOptions);
	task.AddOnCompletion([=](const ::firebase::Future<void>& callback) {
		if (callback.status() == firebase::FutureStatus::kFutureStatusComplete)
		{
			SuccessCallback->ExecuteDelegate();
		}
		else
		{
			FString ErrorMessage = FString(callback.error_message());
			UE_LOG(LogFirebaseGoodies, Error, TEXT("SetData error: %s"), *ErrorMessage);
			ErrorCallback->ExecuteDelegate(ErrorMessage);
		}

		FGUtils::ClearStringCopies();
	});
#endif
}

void UFGDocumentReference::UpdateData(const TMap<FString, FFGValueVariant>& Data, const FFirestoreVoidDelegate& OnSuccess, const FFirestoreStringDelegate& OnError)
{
	UFGFirestoreVoidCallback* SuccessCallback = NewObject<UFGFirestoreVoidCallback>();
	SuccessCallback->BindDelegate(OnSuccess);
	UFGFirestoreStringCallback* ErrorCallback = NewObject<UFGFirestoreStringCallback>();
	ErrorCallback->BindDelegate(OnError);

	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGDocumentReference::UpdateData"));

	if (!IsDocumentReferenceValid())
	{
		return;
	}

#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(UFGFirestoreLibrary::FGFirestoreClassName, "updateDocumentData", "(Lcom/google/firebase/firestore/DocumentReference;Ljava/util/Map;JJ)V",
		JavaDocumentReferenceObject, ValueVariantUtils::VariantMapToJavaMap(FAndroidApplication::GetJavaEnv(), Data), (jlong) SuccessCallback, (jlong) ErrorCallback);
#elif PLATFORM_IOS
	NSDictionary* data = ValueVariantUtils::VariantMapToNSDictionary(Data);
	
	[DocumentReferenceObject updateData:data
							 completion:^(NSError *_Nullable error) {
		if (error == nil) {
			SuccessCallback->ExecuteDelegate();
		} else {
			FString Error = FString([error localizedDescription]);
			ErrorCallback->ExecuteDelegate(Error);
		}
	}];
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	auto task = DocumentReference.Update(FGVariantUtils::ValueVariantMapToFirebaseMapFieldValue(Data));
	task.AddOnCompletion([=](const ::firebase::Future<void>& callback) {
		if (callback.status() == firebase::FutureStatus::kFutureStatusComplete)
		{
			SuccessCallback->ExecuteDelegate();
		}
		else
		{
			FString ErrorMessage = FString(callback.error_message());
			UE_LOG(LogFirebaseGoodies, Error, TEXT("UpdateData error: %s"), *ErrorMessage);
			ErrorCallback->ExecuteDelegate(ErrorMessage);
		}

		FGUtils::ClearStringCopies();
	});
#endif
}

void UFGDocumentReference::Delete(const FFirestoreVoidDelegate& OnSuccess, const FFirestoreStringDelegate& OnError)
{
	UFGFirestoreVoidCallback* SuccessCallback = NewObject<UFGFirestoreVoidCallback>();
	SuccessCallback->BindDelegate(OnSuccess);
	UFGFirestoreStringCallback* ErrorCallback = NewObject<UFGFirestoreStringCallback>();
	ErrorCallback->BindDelegate(OnError);

	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGDocumentReference::Delete"));

	if (!IsDocumentReferenceValid())
	{
		return;
	}

#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(UFGFirestoreLibrary::FGFirestoreClassName, "deleteDocument", "(Lcom/google/firebase/firestore/DocumentReference;JJ)V",
		JavaDocumentReferenceObject, (jlong) SuccessCallback, (jlong) ErrorCallback);
#elif PLATFORM_IOS
	[DocumentReferenceObject deleteDocumentWithCompletion:^(NSError *_Nullable error) {
		if (error == nil) {
			SuccessCallback->ExecuteDelegate();
		} else {
			FString Error = FString([error localizedDescription]);
			ErrorCallback->ExecuteDelegate(Error);
		}
	}];
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	auto task = DocumentReference.Delete();
	task.AddOnCompletion([=](const ::firebase::Future<void>& callback) {
		if (callback.status() == firebase::FutureStatus::kFutureStatusComplete)
		{
			SuccessCallback->ExecuteDelegate();
		}
		else
		{
			FString ErrorMessage = FString(callback.error_message());
			UE_LOG(LogFirebaseGoodies, Error, TEXT("Delete error: %s"), *ErrorMessage);
			ErrorCallback->ExecuteDelegate(ErrorMessage);
		}
	});
#endif
}

void UFGDocumentReference::GetDocumentSnapshot(const FFirestoreDocumentSnapshotDelegate& OnSuccess, const FFirestoreStringDelegate& OnError, EFirestoreSourceType SourceType)
{
	UFGDocumentSnapshotCallback* SuccessCallback = NewObject<UFGDocumentSnapshotCallback>();
	SuccessCallback->BindDelegate(OnSuccess);
	UFGFirestoreStringCallback* ErrorCallback = NewObject<UFGFirestoreStringCallback>();
	ErrorCallback->BindDelegate(OnError);

	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGDocumentReference::GetDocumentSnapshot"));

	if (!IsDocumentReferenceValid())
	{
		return;
	}

#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(UFGFirestoreLibrary::FGFirestoreClassName, "getDocumentSnapshot", "(Lcom/google/firebase/firestore/DocumentReference;IJJ)V",
		JavaDocumentReferenceObject, (int) SourceType, (jlong) SuccessCallback, (jlong) ErrorCallback);
#elif PLATFORM_IOS
	FIRFirestoreSource source;
	if (SourceType == EFirestoreSourceType::Cache) {
		source = FIRFirestoreSourceCache;
	} else if (SourceType == EFirestoreSourceType::Server) {
		source = FIRFirestoreSourceServer;
	} else {
		source = FIRFirestoreSourceDefault;
	}
	
	[DocumentReferenceObject getDocumentWithSource:source
										completion:^(FIRDocumentSnapshot* snapshot, NSError* error) {
		if (error == nil) {
			UFGDocumentSnapshot* Result = NewObject<UFGDocumentSnapshot>();
			Result->Init(snapshot);
			SuccessCallback->ExecuteDelegate(Result);
		} else {
			FString Error = FString([error localizedDescription]);
			ErrorCallback->ExecuteDelegate(Error);
		}
	}];
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	auto task = DocumentReference.Get();
	task.AddOnCompletion([=](const ::firebase::Future<firebase::firestore::DocumentSnapshot>& callback) {
		if (callback.status() == firebase::FutureStatus::kFutureStatusComplete)
		{
			UFGDocumentSnapshot* Result = NewObject<UFGDocumentSnapshot>();
			Result->Init(*callback.result());
			SuccessCallback->ExecuteDelegate(Result);
		}
		else
		{
			FString ErrorMessage = FString(callback.error_message());
			UE_LOG(LogFirebaseGoodies, Error, TEXT("GetDocumentSnapshot error: %s"), *ErrorMessage);
			ErrorCallback->ExecuteDelegate(ErrorMessage);
		}
	});
#endif
}

UFGListenerRegistration* UFGDocumentReference::AddSnapshotListener(const FFirestoreDocumentSnapshotDelegate& OnEvent, const FFirestoreListenerErrorDelegate& OnEventError, EFirestoreListenerExecutorType ExecutorType, bool IncludeOnlyMetadataChanges)
{
	OnListenerEventCallback = OnEvent;
	OnListenerEventErrorCallback = OnEventError;

	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGDocumentReference::AddSnapshotListener"));

	UFGListenerRegistration* Result = NewObject<UFGListenerRegistration>();

	if (!IsDocumentReferenceValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	jobject javaListener = FGMethodCallUtils::CallStaticObjectMethod(UFGFirestoreLibrary::FGFirestoreClassName, "addDocumentReferenceListener", "(Lcom/google/firebase/firestore/DocumentReference;IZJ)Lcom/google/firebase/firestore/ListenerRegistration;",
		JavaDocumentReferenceObject, (int) ExecutorType, IncludeOnlyMetadataChanges, (jlong) this);
	Result->Init(javaListener);
#elif PLATFORM_IOS
	id<FIRListenerRegistration> iosListener = [DocumentReferenceObject addSnapshotListenerWithIncludeMetadataChanges:IncludeOnlyMetadataChanges
																  listener:^(FIRDocumentSnapshot* snapshot, NSError* error) {
		if (error == nil) {
			UFGDocumentSnapshot* Snapshot = NewObject<UFGDocumentSnapshot>();
			Snapshot->Init(snapshot);

			AsyncTask(ENamedThreads::GameThread, [=]() {
				OnListenerEventCallback.ExecuteIfBound(Snapshot);
			});
		} else {
			EFirestoreErrorCode Code = (EFirestoreErrorCode) error.code;
			
			AsyncTask(ENamedThreads::GameThread, [=]() {
				OnListenerEventErrorCallback.ExecuteIfBound(Code);
			});
		}
	}];

	Result->Init(iosListener);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	auto includeMetadataChanges = IncludeOnlyMetadataChanges ? firebase::firestore::MetadataChanges::kInclude : firebase::firestore::MetadataChanges::kExclude;
	auto listenerRegistration = DocumentReference.AddSnapshotListener(includeMetadataChanges, [=](const firebase::firestore::DocumentSnapshot& snapshot, firebase::firestore::Error error, const std::string& errorMessage) {
		if (error == firebase::firestore::Error::kErrorOk)
		{
			UFGDocumentSnapshot* Result = NewObject<UFGDocumentSnapshot>();
			Result->Init(snapshot);

			AsyncTask(ENamedThreads::GameThread, [=]() {
				OnListenerEventCallback.ExecuteIfBound(Result);
			});
		}
		else
		{
			FString ErrorMessage = FString(errorMessage.c_str());
			UE_LOG(LogFirebaseGoodies, Error, TEXT("OnListenerEventErrorCallback error: %s"), *ErrorMessage);

			EFirestoreErrorCode Code = (EFirestoreErrorCode) error;

			AsyncTask(ENamedThreads::GameThread, [=]() {
				OnListenerEventErrorCallback.ExecuteIfBound(Code);
			});
		}
	});

	Result->Init(listenerRegistration);
#endif

	return Result;
}

bool UFGDocumentReference::IsDocumentReferenceValid()
{
	bool Result = false;

#if PLATFORM_ANDROID
	Result = JavaDocumentReferenceObject != nullptr;
#elif PLATFORM_IOS
	Result = DocumentReferenceObject != nil;
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	Result = true;
#endif
#if !WITH_EDITOR
	if (!Result)
	{
		UE_LOG(LogFirebaseGoodies, Error, TEXT("Native object is not valid."));
	}
#endif
	return Result;
}

#if PLATFORM_ANDROID
void UFGDocumentReference::Init(jobject javaDocumentReference)
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	JavaDocumentReferenceObject = Env->NewGlobalRef(javaDocumentReference);
}
#endif

#if PLATFORM_IOS
void UFGDocumentReference::Init(FIRDocumentReference* iosDocumentReference)
{
	DocumentReferenceObject = (FIRDocumentReference*)CFBridgingRetain(iosDocumentReference);
}
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
void UFGDocumentReference::Init(firebase::firestore::DocumentReference documentReference)
{
	DocumentReference = documentReference;
}
#endif
