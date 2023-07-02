#include "Firestore/FGCollectionReference.h"
#include "Firestore/FGDocumentReference.h"
#include "FirebaseGoodiesLog.h"
#include "Callbacks/FGFirestoreCallbacks.h"

#include "Async/Async.h"

#if PLATFORM_IOS
#include "IOS/Utils/FGValueVariantUtils.h"
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
#include "Desktop/FGVariantUtils.h"
#include "Desktop/FGUtils.h"
#endif

UFGCollectionReference::~UFGCollectionReference()
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGCollectionReference::~UFGCollectionReference"));

	if (!IsCollectionReferenceValid())
	{
		return;
	}

#if PLATFORM_ANDROID
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	Env->DeleteGlobalRef(JavaCollectionReferenceObject);
	JavaCollectionReferenceObject = nullptr;
#elif PLATFORM_IOS
	(FIRCollectionReference*)CFBridgingRelease(CollectionReferenceObject);
	CollectionReferenceObject = nil;
#endif
}

FString UFGCollectionReference::GetId()
{
	FString Result;

	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGCollectionReference::GetId"));

	if (!IsCollectionReferenceValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result = FGMethodCallUtils::CallStringMethod(JavaCollectionReferenceObject, "getId", "()Ljava/lang/String;");
#elif PLATFORM_IOS
	Result = FString([CollectionReferenceObject collectionID]);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	Result = CollectionReference.id().c_str();
#endif

	return Result;
}

UFGDocumentReference* UFGCollectionReference::GetParent()
{
	UFGDocumentReference* Result = NewObject<UFGDocumentReference>();

	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGCollectionReference::GetParent"));

	if (!IsCollectionReferenceValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result->Init(FGMethodCallUtils::CallObjectMethod(JavaCollectionReferenceObject, "getParent", "()Lcom/google/firebase/firestore/DocumentReference;"));
#elif PLATFORM_IOS
	Result->Init([CollectionReferenceObject parent]);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	Result->Init(CollectionReference.Parent());
#endif

	return Result;
}

FString UFGCollectionReference::GetPath()
{
	FString Result;

	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGCollectionReference::GetPath"));

	if (!IsCollectionReferenceValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result = FGMethodCallUtils::CallStringMethod(JavaCollectionReferenceObject, "getPath", "()Ljava/lang/String;");
#elif PLATFORM_IOS
	Result = FString([CollectionReferenceObject path]);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	Result = CollectionReference.path().c_str();
#endif

	return Result;
}

UFGDocumentReference* UFGCollectionReference::CreateDocument()
{
	UFGDocumentReference* Result = NewObject<UFGDocumentReference>();

	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGCollectionReference::CreateDocument"));

	if (!IsCollectionReferenceValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result->Init(FGMethodCallUtils::CallObjectMethod(JavaCollectionReferenceObject, "document", "()Lcom/google/firebase/firestore/DocumentReference;"));
#elif PLATFORM_IOS
	Result->Init([CollectionReferenceObject documentWithAutoID]);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	Result->Init(CollectionReference.Document());
#endif

	return Result;
}

UFGDocumentReference* UFGCollectionReference::GetDocument(const FString& Path)
{
	UFGDocumentReference* Result = NewObject<UFGDocumentReference>();

	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGCollectionReference::GetDocument"));

	if (!IsCollectionReferenceValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result->Init(FGMethodCallUtils::CallObjectMethod(JavaCollectionReferenceObject, "document", "(Ljava/lang/String;)Lcom/google/firebase/firestore/DocumentReference;", *FJavaClassObject::GetJString(Path)));
#elif PLATFORM_IOS
	Result->Init([CollectionReferenceObject documentWithPath:Path.GetNSString()]);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	Result->Init(CollectionReference.Document(FGUtils::GetStringCopy(Path)));
#endif

	return Result;
}

void UFGCollectionReference::AddItem(const TMap<FString, FFGValueVariant>& Data, const FFirestoreDocumentDelegate& OnSuccess, const FFirestoreStringDelegate& OnError)
{
	UFGDocumentCallback* SuccessCallback = NewObject<UFGDocumentCallback>();
	SuccessCallback->BindDelegate(OnSuccess);
	UFGFirestoreStringCallback* ErrorCallback = NewObject<UFGFirestoreStringCallback>();
	ErrorCallback->BindDelegate(OnError);

	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGCollectionReference::AddItem"));

	if (!IsCollectionReferenceValid())
	{
		return;
	}

#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(UFGFirestoreLibrary::FGFirestoreClassName, "addItem", "(Lcom/google/firebase/firestore/CollectionReference;Ljava/lang/Object;JJ)V",
		JavaCollectionReferenceObject, ValueVariantUtils::VariantMapToJavaMap(FAndroidApplication::GetJavaEnv(), Data), (jlong) SuccessCallback, (jlong)ErrorCallback);
#elif PLATFORM_IOS
	NSDictionary* data = ValueVariantUtils::VariantMapToNSDictionary(Data);
	__block FIRDocumentReference* document = (FIRDocumentReference*)CFBridgingRetain([CollectionReferenceObject addDocumentWithData:data
																				 completion:^(NSError* error) {
		if (error == nil) {
			UFGDocumentReference* Result = NewObject<UFGDocumentReference>();
			Result->Init(document);
			SuccessCallback->ExecuteDelegate(Result);
		} else {
			FString Error = FString([error localizedDescription]);
			ErrorCallback->ExecuteDelegate(Error);
		}
	}]);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	auto task = CollectionReference.Add(FGVariantUtils::ValueVariantMapToFirebaseMapFieldValue(Data));
	task.AddOnCompletion([=](const ::firebase::Future<firebase::firestore::DocumentReference>& callback) {
		if (callback.status() == firebase::FutureStatus::kFutureStatusComplete)
		{
			UFGDocumentReference* Result = NewObject<UFGDocumentReference>();
			Result->Init(*callback.result());
			SuccessCallback->ExecuteDelegate(Result);
		}
		else
		{
			FString ErrorMessage = FString(callback.error_message());
			UE_LOG(LogFirebaseGoodies, Error, TEXT("DeleteInstanceId error: %s"), *ErrorMessage);
			ErrorCallback->ExecuteDelegate(ErrorMessage);
		}

		FGUtils::ClearStringCopies();
	});
#endif
}

bool UFGCollectionReference::IsCollectionReferenceValid()
{
	bool Result = false;

#if PLATFORM_ANDROID
	Result = JavaCollectionReferenceObject != nullptr;
#elif PLATFORM_IOS
	Result = CollectionReferenceObject != nil;
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	Result = true;
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
void UFGCollectionReference::Init(jobject javaCollectionReference)
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	JavaCollectionReferenceObject = Env->NewGlobalRef(javaCollectionReference);
	
	InitQuery(JavaCollectionReferenceObject);
}
#endif

#if PLATFORM_IOS
void UFGCollectionReference::Init(FIRCollectionReference* iosCollectionReference)
{
	CollectionReferenceObject = (FIRCollectionReference*)CFBridgingRetain(iosCollectionReference);
	
	InitQuery(CollectionReferenceObject);
}
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
void UFGCollectionReference::Init(firebase::firestore::CollectionReference collectionReference)
{
	CollectionReference = collectionReference;
	InitQuery(CollectionReference);
}
#endif
