#include "Firestore/FGWriteBatch.h"

#include "Callbacks/FGFirestoreCallbacks.h"
#include "Firestore/FGDocumentReference.h"
#include "FirebaseGoodiesLog.h"

#include "Async/Async.h"

#if PLATFORM_IOS
#include "IOS/Utils/FGValueVariantUtils.h"
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
#include "Desktop/FGVariantUtils.h"
#include "Desktop/FGUtils.h"
#endif

UFGWriteBatch::~UFGWriteBatch()
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGWriteBatch::~UFGWriteBatch"));

	if (!IsWriteBatchValid())
	{
		return;
	}

#if PLATFORM_ANDROID
	JavaWriteBatchObject = nullptr;
#elif PLATFORM_IOS
	WriteBatchObject = nil;
#endif
}

UFGWriteBatch* UFGWriteBatch::AddSetDataOperation(UFGDocumentReference* Document, const TMap<FString, FFGValueVariant>& Data, EDocumentSetOptions SetOptions)
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGWriteBatch::AddSetDataOperation"));

	if (!IsWriteBatchValid())
	{
		return this;
	}

#if PLATFORM_ANDROID
	JavaWriteBatchObject = FGMethodCallUtils::CallObjectMethod(JavaWriteBatchObject, "set", "(Lcom/google/firebase/firestore/DocumentReference;Ljava/lang/Object;I)Lcom/google/firebase/firestore/WriteBatch;",
		Document->JavaDocumentReferenceObject, ValueVariantUtils::VariantMapToJavaMap(FAndroidApplication::GetJavaEnv(), Data), (int)SetOptions);
#elif PLATFORM_IOS
	NSDictionary* data = ValueVariantUtils::VariantMapToNSDictionary(Data);
	
	WriteBatchObject = [WriteBatchObject setData:data
									 forDocument:Document->DocumentReferenceObject
										   merge:SetOptions == EDocumentSetOptions::Merge];
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	auto setOptions = SetOptions == EDocumentSetOptions::Merge ? firebase::firestore::SetOptions::Merge() : firebase::firestore::SetOptions();
	WriteBatch = WriteBatch.Set(Document->DocumentReference, FGVariantUtils::ValueVariantMapToFirebaseMapFieldValue(Data), setOptions);
#endif
	
	return this;
}

UFGWriteBatch* UFGWriteBatch::AddUpdateDataOperation(UFGDocumentReference* Document, const TMap<FString, FFGValueVariant>& Data)
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGWriteBatch::AddUpdateDataOperation"));

	if (!IsWriteBatchValid())
	{
		return this;
	}

#if PLATFORM_ANDROID
	JavaWriteBatchObject = FGMethodCallUtils::CallObjectMethod(JavaWriteBatchObject, "update", "(Lcom/google/firebase/firestore/DocumentReference;Ljava/util/Map;)Lcom/google/firebase/firestore/WriteBatch;",
		Document->JavaDocumentReferenceObject, ValueVariantUtils::VariantMapToJavaMap(FAndroidApplication::GetJavaEnv(), Data));
#elif PLATFORM_IOS
	NSDictionary* data = ValueVariantUtils::VariantMapToNSDictionary(Data);
	
	WriteBatchObject = [WriteBatchObject updateData:data
										forDocument:Document->DocumentReferenceObject];
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	WriteBatch = WriteBatch.Update(Document->DocumentReference, FGVariantUtils::ValueVariantMapToFirebaseMapFieldValue(Data));
#endif
	
	return this;
}

UFGWriteBatch* UFGWriteBatch::AddDeleteOperation(UFGDocumentReference* Document)
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGWriteBatch::AddDeleteOperation"));

	if (!IsWriteBatchValid())
	{
		return this;
	}

#if PLATFORM_ANDROID
	JavaWriteBatchObject = FGMethodCallUtils::CallObjectMethod(JavaWriteBatchObject, "delete", "(Lcom/google/firebase/firestore/DocumentReference;)Lcom/google/firebase/firestore/WriteBatch;",
		Document->JavaDocumentReferenceObject);
#elif PLATFORM_IOS
	WriteBatchObject = [WriteBatchObject deleteDocument:Document->DocumentReferenceObject];
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	WriteBatch = WriteBatch.Delete(Document->DocumentReference);
#endif
	
	return this;
}

void UFGWriteBatch::Commit(const FFirestoreVoidDelegate& OnSuccess, const FFirestoreStringDelegate& OnError)
{
	UFGFirestoreVoidCallback* SuccessCallback = NewObject<UFGFirestoreVoidCallback>();
	SuccessCallback->BindDelegate(OnSuccess);
	UFGFirestoreStringCallback* ErrorCallback = NewObject<UFGFirestoreStringCallback>();
	ErrorCallback->BindDelegate(OnError);

	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGWriteBatch::Commit"));

	if (!IsWriteBatchValid())
	{
		return;
	}

#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(UFGFirestoreLibrary::FGFirestoreClassName, "commitBatch", "(Lcom/google/firebase/firestore/WriteBatch;JJ)V",
		JavaWriteBatchObject, (jlong) SuccessCallback, (jlong) ErrorCallback);
#elif PLATFORM_IOS
	[WriteBatchObject commitWithCompletion:^(NSError* error) {
		if (error == nil) {
			SuccessCallback->ExecuteDelegate();
		} else {
			FString Error = FString([error localizedDescription]);
			ErrorCallback->ExecuteDelegate(Error);
		}
	}];
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	auto task = WriteBatch.Commit();
	task.AddOnCompletion([=](const ::firebase::Future<void>& callback) {
		if (callback.status() == firebase::FutureStatus::kFutureStatusComplete)
		{
			SuccessCallback->ExecuteDelegate();
		}
		else
		{
			FString ErrorMessage = FString(callback.error_message());
			UE_LOG(LogFirebaseGoodies, Error, TEXT("UFGWriteBatch::Commit error: %s"), *ErrorMessage);
			ErrorCallback->ExecuteDelegate(ErrorMessage);
		}

		FGUtils::ClearStringCopies();
	});
#endif
}

bool UFGWriteBatch::IsWriteBatchValid()
{
	bool Result = false;

#if PLATFORM_ANDROID
	Result = JavaWriteBatchObject != nullptr;
#elif PLATFORM_IOS
	Result = WriteBatchObject != nil;
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
void UFGWriteBatch::Init(jobject javaWriteBatch)
{
	JavaWriteBatchObject = javaWriteBatch;
}
#endif

#if PLATFORM_IOS
void UFGWriteBatch::Init(FIRWriteBatch* iosWriteBatch)
{
	WriteBatchObject = iosWriteBatch;
}
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
void UFGWriteBatch::Init(firebase::firestore::WriteBatch writeBatch)
{
	WriteBatch = writeBatch;
}
#endif
