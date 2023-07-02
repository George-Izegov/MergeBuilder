#include "Firestore/FGQuerySnapshot.h"
#include "Firestore/FGFirestoreLibrary.h"
#include "Firestore/FGDocumentSnapshot.h"
#include "Firestore/FGQuery.h"
#include "FirebaseGoodiesLog.h"

UFGQuerySnapshot::~UFGQuerySnapshot()
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGQuerySnapshot::~UFGQuerySnapshot"));

	if (!IsQuerySnapshotValid())
	{
		return;
	}

#if PLATFORM_ANDROID
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	Env->DeleteGlobalRef(JavaQuerySnapshotObject);
	JavaQuerySnapshotObject = nullptr;
#elif PLATFORM_IOS
	(FIRQuerySnapshot*)CFBridgingRelease(QuerySnapshotObject);
	QuerySnapshotObject = nil;
#endif
}

UFGQuery* UFGQuerySnapshot::GetQuery()
{
	UFGQuery* Result = NewObject<UFGQuery>();

	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGQuerySnapshot::GetQuery"));

	if (!IsQuerySnapshotValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	jobject javaQuery = FGMethodCallUtils::CallObjectMethod(JavaQuerySnapshotObject, "getQuery",
		"()Lcom/google/firebase/firestore/Query;");

	Result->InitQuery(javaQuery);
#elif PLATFORM_IOS
	Result->InitQuery([QuerySnapshotObject query]);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	Result->InitQuery(QuerySnapshot.query());
#endif

	return Result;
}

TArray<UFGDocumentSnapshot*> UFGQuerySnapshot::GetDocuments()
{
	TArray<UFGDocumentSnapshot*> Result;

	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGQuerySnapshot::GetDocuments"));

	if (!IsQuerySnapshotValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	jobjectArray javaDocumentArray = (jobjectArray) FGMethodCallUtils::CallStaticObjectMethod(UFGFirestoreLibrary::FGFirestoreClassName, "getQuerySnapshotDocuments",
		"(Lcom/google/firebase/firestore/QuerySnapshot;)[Lcom/google/firebase/firestore/DocumentSnapshot;", JavaQuerySnapshotObject);

	Result = FGJavaConvertor::FromJavaDocumentSnapshots(javaDocumentArray);
#elif PLATFORM_IOS
	NSArray<FIRQueryDocumentSnapshot*>* documents = QuerySnapshotObject.documents;
	
	for (id document in documents) {
		UFGDocumentSnapshot* Document = NewObject<UFGDocumentSnapshot>();
		Document->Init(document);
		
		Result.Add(Document);
	}
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	for (const auto& document : QuerySnapshot.documents())
	{
		UFGDocumentSnapshot* Document = NewObject<UFGDocumentSnapshot>();
		Document->Init(document);

		Result.Add(Document);
	}
#endif

	return Result;
}

bool UFGQuerySnapshot::HasPendingWrites()
{
	bool Result = false;

	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGQuerySnapshot::HasPendingWrites"));

	if (!IsQuerySnapshotValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result = FGMethodCallUtils::CallStaticBoolMethod(UFGFirestoreLibrary::FGFirestoreClassName, "hasQuerySnapshotPendingWrites", "(Lcom/google/firebase/firestore/QuerySnapshot;)Z", JavaQuerySnapshotObject);
#elif PLATFORM_IOS
	Result = [QuerySnapshotObject metadata].pendingWrites;
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	Result = QuerySnapshot.metadata().has_pending_writes();
#endif

	return Result;
}

bool UFGQuerySnapshot::IsFromCache()
{
	bool Result = false;

	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGQuerySnapshot::IsFromCache"));

	if (!IsQuerySnapshotValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result = FGMethodCallUtils::CallStaticBoolMethod(UFGFirestoreLibrary::FGFirestoreClassName, "isQuerySnapshotFromCache", "(Lcom/google/firebase/firestore/QuerySnapshot;)Z", JavaQuerySnapshotObject);
#elif PLATFORM_IOS
	Result = [QuerySnapshotObject metadata].fromCache;
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	Result = QuerySnapshot.metadata().is_from_cache();
#endif

	return Result;
}

bool UFGQuerySnapshot::IsQuerySnapshotValid()
{
	bool Result = false;

#if PLATFORM_ANDROID
	Result = JavaQuerySnapshotObject != nullptr;
#elif PLATFORM_IOS
	Result = QuerySnapshotObject != nil;
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
void UFGQuerySnapshot::Init(jobject javaQuerySnapshot)
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	JavaQuerySnapshotObject = Env->NewGlobalRef(javaQuerySnapshot);
}
#endif

#if PLATFORM_IOS
void UFGQuerySnapshot::Init(FIRQuerySnapshot* iosQuerySnapshot)
{
	QuerySnapshotObject = (FIRQuerySnapshot*)CFBridgingRetain(iosQuerySnapshot);
}
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
void UFGQuerySnapshot::Init(firebase::firestore::QuerySnapshot querySnapshot)
{
	QuerySnapshot = querySnapshot;
}
#endif
