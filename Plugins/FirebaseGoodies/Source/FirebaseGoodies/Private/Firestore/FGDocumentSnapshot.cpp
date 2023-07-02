#include "Firestore/FGDocumentSnapshot.h"

#include "Firestore/FGDocumentReference.h"

#include "FirebaseGoodiesLog.h"
#include "Firestore/FGFirestoreLibrary.h"

#if PLATFORM_IOS
#include "IOS/Utils/FGValueVariantUtils.h"
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
#include "Desktop/FGVariantUtils.h"
#include "Desktop/FGUtils.h"
#endif

UFGDocumentSnapshot::~UFGDocumentSnapshot()
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGDocumentSnapshot::~UFGDocumentSnapshot"));

	if (!IsDocumentSnapshotValid())
	{
		return;
	}

#if PLATFORM_ANDROID
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	Env->DeleteGlobalRef(JavaDocumentSnapshotObject);
	JavaDocumentSnapshotObject = nullptr;
#elif PLATFORM_IOS
	(FIRDocumentSnapshot*)CFBridgingRelease(DocumentSnapshotObject);
	DocumentSnapshotObject = nil;
#endif
}

FString UFGDocumentSnapshot::GetId()
{
	FString Result;
	
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGDocumentSnapshot::GetId"));

	if (!IsDocumentSnapshotValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result = FGMethodCallUtils::CallStringMethod(JavaDocumentSnapshotObject, "getId", "()Ljava/lang/String;");
#elif PLATFORM_IOS
	Result = FString([DocumentSnapshotObject documentID]);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	Result = DocumentSnapshot.id().c_str();
#endif

	return Result;
}

bool UFGDocumentSnapshot::Exists()
{
	bool Result = false;

	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGDocumentSnapshot::Exists"));

	if (!IsDocumentSnapshotValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result = FGMethodCallUtils::CallBoolMethod(JavaDocumentSnapshotObject, "exists", "()Z");
#elif PLATFORM_IOS
	Result = [DocumentSnapshotObject exists];
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	Result = DocumentSnapshot.exists();
#endif

	return Result;
}

TMap<FString, FFGValueVariant> UFGDocumentSnapshot::GetData()
{
	TMap<FString, FFGValueVariant> Result;

	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGDocumentSnapshot::GetData"));

	if (!IsDocumentSnapshotValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	jobject javaMap = FGMethodCallUtils::CallObjectMethod(JavaDocumentSnapshotObject, "getData", "()Ljava/util/Map;");
	Result = ValueVariantUtils::JavaMapToVariantMap(FAndroidApplication::GetJavaEnv(), javaMap);
#elif PLATFORM_IOS
	Result = ValueVariantUtils::NSDictionaryToVariantMap([DocumentSnapshotObject data]);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	Result = FGVariantUtils::FirebaseMapFieldValueToValueVariantMap(DocumentSnapshot.GetData());
#endif

	return Result;
}

bool UFGDocumentSnapshot::Contains(const FString& FieldName)
{
	bool Result = false;

	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGDocumentSnapshot::Contains"));

	if (!IsDocumentSnapshotValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result = FGMethodCallUtils::CallBoolMethod(JavaDocumentSnapshotObject, "contains", "(Ljava/lang/String;)Z", *FJavaClassObject::GetJString(FieldName));
#elif PLATFORM_IOS
	Result = [[DocumentSnapshotObject data] objectForKey:FieldName.GetNSString()] != nil;
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	Result = DocumentSnapshot.GetData().find(TCHAR_TO_ANSI(*FieldName)) != DocumentSnapshot.GetData().end();
#endif

	return Result;
}

FFGValueVariant UFGDocumentSnapshot::GetValue(const FString& FieldName)
{
	FFGValueVariant Result;

	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGDocumentSnapshot::GetValue"));

	if (!IsDocumentSnapshotValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	jobject javaField = FGMethodCallUtils::CallObjectMethod(JavaDocumentSnapshotObject, "get", "(Ljava/lang/String;)Ljava/lang/Object;", *FJavaClassObject::GetJString(FieldName));
	Result = ValueVariantUtils::JavaObjectToVariant(FAndroidApplication::GetJavaEnv(), javaField);
#elif PLATFORM_IOS
	Result = ValueVariantUtils::IdToVariant([DocumentSnapshotObject valueForField:FieldName.GetNSString()]);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	Result = FGVariantUtils::FirebaseFieldValueToValueVariant(DocumentSnapshot.Get(TCHAR_TO_ANSI(*FieldName)));
#endif

	return Result;
}

UFGDocumentReference* UFGDocumentSnapshot::GetDocumentReference(const FString& Path)
{
	UFGDocumentReference* Result = NewObject<UFGDocumentReference>();

	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGDocumentSnapshot::GetDocumentReference"));

	if (!IsDocumentSnapshotValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	jobject javaDocument;
	if (!Path.IsEmpty()) {
		javaDocument = FGMethodCallUtils::CallObjectMethod(JavaDocumentSnapshotObject, "getDocumentReference", "(Ljava/lang/String;)Lcom/google/firebase/firestore/DocumentReference;",
			*FJavaClassObject::GetJString(Path));
	}
	else {
		javaDocument = FGMethodCallUtils::CallObjectMethod(JavaDocumentSnapshotObject, "getReference", "()Lcom/google/firebase/firestore/DocumentReference;");
	}
	
	Result->Init(javaDocument);
#elif PLATFORM_IOS
	Result->Init([DocumentSnapshotObject reference]);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	Result->Init(DocumentSnapshot.reference());
#endif

	return Result;
}

bool UFGDocumentSnapshot::HasPendingWrites()
{
	bool Result = false;

	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGDocumentSnapshot::HasPendingWrites"));

	if (!IsDocumentSnapshotValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result = FGMethodCallUtils::CallStaticBoolMethod(UFGFirestoreLibrary::FGFirestoreClassName, "hasDocumentSnapshotPendingWrites", "(Lcom/google/firebase/firestore/DocumentSnapshot;)Z", JavaDocumentSnapshotObject);
#elif PLATFORM_IOS
	Result = [DocumentSnapshotObject metadata].pendingWrites;
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	Result = DocumentSnapshot.metadata().has_pending_writes();
#endif

	return Result;
}

bool UFGDocumentSnapshot::IsFromCache()
{
	bool Result = false;

	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGDocumentSnapshot::IsFromCache"));

	if (!IsDocumentSnapshotValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result = FGMethodCallUtils::CallStaticBoolMethod(UFGFirestoreLibrary::FGFirestoreClassName, "isDocumentSnapshotFromCache", "(Lcom/google/firebase/firestore/DocumentSnapshot;)Z", JavaDocumentSnapshotObject);
#elif PLATFORM_IOS
	Result = [DocumentSnapshotObject metadata].fromCache;
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	Result = DocumentSnapshot.metadata().is_from_cache();
#endif

	return Result;
}

bool UFGDocumentSnapshot::IsDocumentSnapshotValid()
{
	bool Result = false;

#if PLATFORM_ANDROID
	Result = JavaDocumentSnapshotObject != nullptr;
#elif PLATFORM_IOS
	Result = DocumentSnapshotObject != nil;
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
void UFGDocumentSnapshot::Init(jobject javaDocumentSnapshot)
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	JavaDocumentSnapshotObject = Env->NewGlobalRef(javaDocumentSnapshot);
}
#endif

#if PLATFORM_IOS
void UFGDocumentSnapshot::Init(FIRDocumentSnapshot* iosDocumentSnapshot)
{
	DocumentSnapshotObject = (FIRDocumentSnapshot*)CFBridgingRetain(iosDocumentSnapshot);
}
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
void UFGDocumentSnapshot::Init(firebase::firestore::DocumentSnapshot documentSnapshot)
{
	DocumentSnapshot = documentSnapshot;
}
#endif
