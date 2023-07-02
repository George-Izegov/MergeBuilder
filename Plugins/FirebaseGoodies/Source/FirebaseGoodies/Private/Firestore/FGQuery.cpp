#include "Firestore/FGQuery.h"
#include "FirebaseGoodiesLog.h"
#include "Firestore/FGListenerRegistration.h"
#include "Firestore/FGDocumentSnapshot.h"
#include "Firestore/FGQuerySnapshot.h"
#include "Callbacks/FGFirestoreCallbacks.h"

#include "Async/Async.h"

#if PLATFORM_IOS
#include "IOS/Utils/FGValueVariantUtils.h"
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
#include "Desktop/FGVariantUtils.h"
#include "Desktop/FGUtils.h"
#endif

UFGQuery::~UFGQuery()
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGQuery::~UFGQuery"));

	if (!IsQueryValid())
	{
		return;
	}

#if PLATFORM_ANDROID
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	Env->DeleteGlobalRef(JavaQueryObject);
	JavaQueryObject = nullptr;
#elif PLATFORM_IOS
	(FIRQuery*)CFBridgingRelease(QueryObject);
	QueryObject = nil;
#endif
}

UFGQuery* UFGQuery::WhereEqualTo(const FString& Field, const FFGValueVariant& Value)
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGQuery::WhereEqualTo"));

	UFGQuery* Result = NewObject<UFGQuery>();

	if (!IsQueryValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	jobject javaQuery = FGMethodCallUtils::CallObjectMethod(JavaQueryObject, "whereEqualTo", "(Ljava/lang/String;Ljava/lang/Object;)Lcom/google/firebase/firestore/Query;",
		*FJavaClassObject::GetJString(Field), ValueVariantUtils::VariantToJavaObject(FAndroidApplication::GetJavaEnv(), Value));
	Result->InitQuery(javaQuery);
#elif PLATFORM_IOS
	FIRQuery* iosQuery = [QueryObject queryWhereField:Field.GetNSString() isEqualTo:ValueVariantUtils::VariantToId(Value)];
	Result->InitQuery(iosQuery);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	auto newQuery = Query.WhereEqualTo(TCHAR_TO_ANSI(*Field), FGVariantUtils::ValueVariantToFirebaseFieldValue(Value));
	Result->InitQuery(newQuery);
#endif

	return Result;
}

UFGQuery* UFGQuery::WhereNotEqualTo(const FString& Field, const FFGValueVariant& Value)
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGQuery::WhereNotEqualTo"));

	UFGQuery* Result = NewObject<UFGQuery>();

	if (!IsQueryValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	jobject javaQuery = FGMethodCallUtils::CallObjectMethod(JavaQueryObject, "whereNotEqualTo", "(Ljava/lang/String;Ljava/lang/Object;)Lcom/google/firebase/firestore/Query;",
		*FJavaClassObject::GetJString(Field), ValueVariantUtils::VariantToJavaObject(FAndroidApplication::GetJavaEnv(), Value));
	Result->InitQuery(javaQuery);
#elif PLATFORM_IOS
	FIRQuery* iosQuery = [QueryObject queryWhereField:Field.GetNSString() isNotEqualTo:ValueVariantUtils::VariantToId(Value)];
	Result->InitQuery(iosQuery);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	auto newQuery = Query.WhereNotEqualTo(TCHAR_TO_ANSI(*Field), FGVariantUtils::ValueVariantToFirebaseFieldValue(Value));
	Result->InitQuery(newQuery);
#endif

	return Result;
}

UFGQuery* UFGQuery::WhereLessThan(const FString& Field, const FFGValueVariant& Value)
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGQuery::WhereLessThan"));

	UFGQuery* Result = NewObject<UFGQuery>();

	if (!IsQueryValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	jobject javaQuery = FGMethodCallUtils::CallObjectMethod(JavaQueryObject, "whereLessThan", "(Ljava/lang/String;Ljava/lang/Object;)Lcom/google/firebase/firestore/Query;",
		*FJavaClassObject::GetJString(Field), ValueVariantUtils::VariantToJavaObject(FAndroidApplication::GetJavaEnv(), Value));
	Result->InitQuery(javaQuery);
#elif PLATFORM_IOS
	FIRQuery* iosQuery = [QueryObject queryWhereField:Field.GetNSString() isLessThan:ValueVariantUtils::VariantToId(Value)];
	Result->InitQuery(iosQuery);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	auto newQuery = Query.WhereLessThan(TCHAR_TO_ANSI(*Field), FGVariantUtils::ValueVariantToFirebaseFieldValue(Value));
	Result->InitQuery(newQuery);
#endif

	return Result;
}

UFGQuery* UFGQuery::WhereLessThanOrEqualTo(const FString& Field, const FFGValueVariant& Value)
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGQuery::WhereLessThanOrEqualTo"));

	UFGQuery* Result = NewObject<UFGQuery>();

	if (!IsQueryValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	jobject javaQuery = FGMethodCallUtils::CallObjectMethod(JavaQueryObject, "whereLessThanOrEqualTo", "(Ljava/lang/String;Ljava/lang/Object;)Lcom/google/firebase/firestore/Query;",
		*FJavaClassObject::GetJString(Field), ValueVariantUtils::VariantToJavaObject(FAndroidApplication::GetJavaEnv(), Value));
	Result->InitQuery(javaQuery);
#elif PLATFORM_IOS
	FIRQuery* iosQuery = [QueryObject queryWhereField:Field.GetNSString() isLessThanOrEqualTo:ValueVariantUtils::VariantToId(Value)];
	Result->InitQuery(iosQuery);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	auto newQuery = Query.WhereLessThanOrEqualTo(TCHAR_TO_ANSI(*Field), FGVariantUtils::ValueVariantToFirebaseFieldValue(Value));
	Result->InitQuery(newQuery);
#endif

	return Result;
}

UFGQuery* UFGQuery::WhereGreaterThan(const FString& Field, const FFGValueVariant& Value)
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGQuery::WhereGreaterThan"));

	UFGQuery* Result = NewObject<UFGQuery>();

	if (!IsQueryValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	jobject javaQuery = FGMethodCallUtils::CallObjectMethod(JavaQueryObject, "whereGreaterThan", "(Ljava/lang/String;Ljava/lang/Object;)Lcom/google/firebase/firestore/Query;",
		*FJavaClassObject::GetJString(Field), ValueVariantUtils::VariantToJavaObject(FAndroidApplication::GetJavaEnv(), Value));
	Result->InitQuery(javaQuery);
#elif PLATFORM_IOS
	FIRQuery* iosQuery = [QueryObject queryWhereField:Field.GetNSString() isGreaterThan:ValueVariantUtils::VariantToId(Value)];
	Result->InitQuery(iosQuery);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	auto newQuery = Query.WhereGreaterThan(TCHAR_TO_ANSI(*Field), FGVariantUtils::ValueVariantToFirebaseFieldValue(Value));
	Result->InitQuery(newQuery);
#endif

	return Result;
}

UFGQuery* UFGQuery::WhereGreaterThanOrEqualTo(const FString& Field, const FFGValueVariant& Value)
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGQuery::WhereGreaterThanOrEqualTo"));

	UFGQuery* Result = NewObject<UFGQuery>();

	if (!IsQueryValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	jobject javaQuery = FGMethodCallUtils::CallObjectMethod(JavaQueryObject, "whereGreaterThanOrEqualTo", "(Ljava/lang/String;Ljava/lang/Object;)Lcom/google/firebase/firestore/Query;",
		*FJavaClassObject::GetJString(Field), ValueVariantUtils::VariantToJavaObject(FAndroidApplication::GetJavaEnv(), Value));
	Result->InitQuery(javaQuery);
#elif PLATFORM_IOS
	FIRQuery* iosQuery = [QueryObject queryWhereField:Field.GetNSString() isGreaterThanOrEqualTo:ValueVariantUtils::VariantToId(Value)];
	Result->InitQuery(iosQuery);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	auto newQuery = Query.WhereGreaterThanOrEqualTo(TCHAR_TO_ANSI(*Field), FGVariantUtils::ValueVariantToFirebaseFieldValue(Value));
	Result->InitQuery(newQuery);
#endif

	return Result;
}

UFGQuery* UFGQuery::WhereArrayContains(const FString& Field, const FFGValueVariant& Value)
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGQuery::WhereArrayContains"));

	UFGQuery* Result = NewObject<UFGQuery>();

	if (!IsQueryValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	jobject javaQuery = FGMethodCallUtils::CallObjectMethod(JavaQueryObject, "whereArrayContains", "(Ljava/lang/String;Ljava/lang/Object;)Lcom/google/firebase/firestore/Query;",
		*FJavaClassObject::GetJString(Field), ValueVariantUtils::VariantToJavaObject(FAndroidApplication::GetJavaEnv(), Value));
	Result->InitQuery(javaQuery);
#elif PLATFORM_IOS
	FIRQuery* iosQuery = [QueryObject queryWhereField:Field.GetNSString() arrayContains:ValueVariantUtils::VariantToId(Value)];
	Result->InitQuery(iosQuery);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	auto newQuery = Query.WhereArrayContains(TCHAR_TO_ANSI(*Field), FGVariantUtils::ValueVariantToFirebaseFieldValue(Value));
	Result->InitQuery(newQuery);
#endif

	return Result;
}

UFGQuery* UFGQuery::WhereArrayContainsAny(const FString& Field, const TArray<FFGValueVariant>& Values)
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGQuery::WhereArrayContainsAny"));

	UFGQuery* Result = NewObject<UFGQuery>();

	if (!IsQueryValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	jobject javaQuery = FGMethodCallUtils::CallObjectMethod(JavaQueryObject, "whereArrayContainsAny", "(Ljava/lang/String;Ljava/util/List;)Lcom/google/firebase/firestore/Query;",
		*FJavaClassObject::GetJString(Field), ValueVariantUtils::VariantArrayToJavaList(FAndroidApplication::GetJavaEnv(), Values));
	Result->InitQuery(javaQuery);
#elif PLATFORM_IOS
	FIRQuery* iosQuery = [QueryObject queryWhereField:Field.GetNSString() arrayContainsAny:ValueVariantUtils::VariantArrayToNSArray(Values)];
	Result->InitQuery(iosQuery);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	auto newQuery = Query.WhereArrayContainsAny(TCHAR_TO_ANSI(*Field), FGVariantUtils::ValueVariantArrayToFirebaseFieldValueVector(Values));
	Result->InitQuery(newQuery);
#endif

	return Result;
}

UFGQuery* UFGQuery::WhereIn(const FString& Field, const TArray<FFGValueVariant>& Values)
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGQuery::WhereIn"));

	UFGQuery* Result = NewObject<UFGQuery>();

	if (!IsQueryValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	jobject javaQuery = FGMethodCallUtils::CallObjectMethod(JavaQueryObject, "whereIn", "(Ljava/lang/String;Ljava/util/List;)Lcom/google/firebase/firestore/Query;",
		*FJavaClassObject::GetJString(Field), ValueVariantUtils::VariantArrayToJavaList(FAndroidApplication::GetJavaEnv(), Values));
	Result->InitQuery(javaQuery);
#elif PLATFORM_IOS
	FIRQuery* iosQuery = [QueryObject queryWhereField:Field.GetNSString() in:ValueVariantUtils::VariantArrayToNSArray(Values)];
	Result->InitQuery(iosQuery);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	auto newQuery = Query.WhereIn(TCHAR_TO_ANSI(*Field), FGVariantUtils::ValueVariantArrayToFirebaseFieldValueVector(Values));
	Result->InitQuery(newQuery);
#endif

	return Result;
}

UFGQuery* UFGQuery::WhereNotIn(const FString& Field, const TArray<FFGValueVariant>& Values)
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGQuery::WhereNotIn"));

	UFGQuery* Result = NewObject<UFGQuery>();

	if (!IsQueryValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	jobject javaQuery = FGMethodCallUtils::CallObjectMethod(JavaQueryObject, "whereNotIn", "(Ljava/lang/String;Ljava/util/List;)Lcom/google/firebase/firestore/Query;",
		*FJavaClassObject::GetJString(Field), ValueVariantUtils::VariantArrayToJavaList(FAndroidApplication::GetJavaEnv(), Values));
	Result->InitQuery(javaQuery);
#elif PLATFORM_IOS
	FIRQuery* iosQuery = [QueryObject queryWhereField:Field.GetNSString() notIn:ValueVariantUtils::VariantArrayToNSArray(Values)];
	Result->InitQuery(iosQuery);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	auto newQuery = Query.WhereNotIn(TCHAR_TO_ANSI(*Field), FGVariantUtils::ValueVariantArrayToFirebaseFieldValueVector(Values));
	Result->InitQuery(newQuery);
#endif

	return Result;
}

UFGQuery* UFGQuery::OrderBy(const FString& Field, bool Ascending)
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGQuery::OrderBy"));

	UFGQuery* Result = NewObject<UFGQuery>();

	if (!IsQueryValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	jobject javaQuery = FGMethodCallUtils::CallStaticObjectMethod(UFGFirestoreLibrary::FGFirestoreClassName, "orderQueryBy", "(Lcom/google/firebase/firestore/Query;Ljava/lang/String;Z)Lcom/google/firebase/firestore/Query;",
		JavaQueryObject, *FJavaClassObject::GetJString(Field), Ascending);
	Result->InitQuery(javaQuery);
#elif PLATFORM_IOS
	FIRQuery* iosQuery = [QueryObject queryOrderedByField:Field.GetNSString() descending:!Ascending];
	Result->InitQuery(iosQuery);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	auto newQuery = Query.OrderBy(TCHAR_TO_ANSI(*Field), Ascending ? firebase::firestore::Query::Direction::kAscending : firebase::firestore::Query::Direction::kDescending);
	Result->InitQuery(newQuery);
#endif

	return Result;
}

UFGQuery* UFGQuery::Limit(int64 Value)
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGQuery::Limit"));

	UFGQuery* Result = NewObject<UFGQuery>();

	if (!IsQueryValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	jobject javaQuery = FGMethodCallUtils::CallObjectMethod(JavaQueryObject, "limit", "(J)Lcom/google/firebase/firestore/Query;", Value);
	Result->InitQuery(javaQuery);
#elif PLATFORM_IOS
	FIRQuery* iosQuery = [QueryObject queryLimitedTo:Value];
	Result->InitQuery(iosQuery);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	auto newQuery = Query.Limit(Value);
	Result->InitQuery(newQuery);
#endif

	return Result;
}

UFGQuery* UFGQuery::LimitToLast(int64 Value)
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGQuery::LimitToLast"));

	UFGQuery* Result = NewObject<UFGQuery>();

	if (!IsQueryValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	jobject javaQuery = FGMethodCallUtils::CallObjectMethod(JavaQueryObject, "limitToLast", "(J)Lcom/google/firebase/firestore/Query;", Value);
	Result->InitQuery(javaQuery);
#elif PLATFORM_IOS
	FIRQuery* iosQuery = [QueryObject queryLimitedToLast:Value];
	Result->InitQuery(iosQuery);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	auto newQuery = Query.LimitToLast(Value);
	Result->InitQuery(newQuery);
#endif

	return Result;
}

UFGQuery* UFGQuery::StartAt(UFGDocumentSnapshot* Document)
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGQuery::StartAt"));

	UFGQuery* Result = NewObject<UFGQuery>();

	if (!IsQueryValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	jobject javaQuery = FGMethodCallUtils::CallObjectMethod(JavaQueryObject, "startAt", "(Lcom/google/firebase/firestore/DocumentSnapshot;)Lcom/google/firebase/firestore/Query;",
		Document->JavaDocumentSnapshotObject);
	Result->InitQuery(javaQuery);
#elif PLATFORM_IOS
	FIRQuery* iosQuery = [QueryObject queryStartingAtDocument:Document->DocumentSnapshotObject];
	Result->InitQuery(iosQuery);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	auto newQuery = Query.StartAt(Document->DocumentSnapshot);
	Result->InitQuery(newQuery);
#endif

	return Result;
}

UFGQuery* UFGQuery::StartAfter(UFGDocumentSnapshot* Document)
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGQuery::StartAfter"));

	UFGQuery* Result = NewObject<UFGQuery>();

	if (!IsQueryValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	jobject javaQuery = FGMethodCallUtils::CallObjectMethod(JavaQueryObject, "startAfter", "(Lcom/google/firebase/firestore/DocumentSnapshot;)Lcom/google/firebase/firestore/Query;",
		Document->JavaDocumentSnapshotObject);
	Result->InitQuery(javaQuery);
#elif PLATFORM_IOS
	FIRQuery* iosQuery = [QueryObject queryStartingAfterDocument:Document->DocumentSnapshotObject];
	Result->InitQuery(iosQuery);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	auto newQuery = Query.StartAfter(Document->DocumentSnapshot);
	Result->InitQuery(newQuery);
#endif

	return Result;
}

UFGQuery* UFGQuery::EndBefore(UFGDocumentSnapshot* Document)
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGQuery::EndBefore"));

	UFGQuery* Result = NewObject<UFGQuery>();

	if (!IsQueryValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	jobject javaQuery = FGMethodCallUtils::CallObjectMethod(JavaQueryObject, "endBefore", "(Lcom/google/firebase/firestore/DocumentSnapshot;)Lcom/google/firebase/firestore/Query;",
		Document->JavaDocumentSnapshotObject);
	Result->InitQuery(javaQuery);
#elif PLATFORM_IOS
	FIRQuery* iosQuery = [QueryObject queryEndingBeforeDocument:Document->DocumentSnapshotObject];
	Result->InitQuery(iosQuery);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	auto newQuery = Query.EndBefore(Document->DocumentSnapshot);
	Result->InitQuery(newQuery);
#endif

	return Result;
}

UFGQuery* UFGQuery::EndAt(UFGDocumentSnapshot* Document)
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGQuery::EndAt"));

	UFGQuery* Result = NewObject<UFGQuery>();

	if (!IsQueryValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	jobject javaQuery = FGMethodCallUtils::CallObjectMethod(JavaQueryObject, "endAt", "(Lcom/google/firebase/firestore/DocumentSnapshot;)Lcom/google/firebase/firestore/Query;",
		Document->JavaDocumentSnapshotObject);
	Result->InitQuery(javaQuery);
#elif PLATFORM_IOS
	FIRQuery* iosQuery = [QueryObject queryEndingAtDocument:Document->DocumentSnapshotObject];
	Result->InitQuery(iosQuery);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	auto newQuery = Query.EndAt(Document->DocumentSnapshot);
	Result->InitQuery(newQuery);
#endif

	return Result;
}

void UFGQuery::Get(const FFirestoreQuerySnapshotDelegate& OnSuccess, const FFirestoreStringDelegate& OnError, EFirestoreSourceType Source)
{
	UFGQuerySnapshotCallback* SuccessCallback = NewObject<UFGQuerySnapshotCallback>();
	SuccessCallback->BindDelegate(OnSuccess);
	UFGFirestoreStringCallback* ErrorCallback = NewObject<UFGFirestoreStringCallback>();
	ErrorCallback->BindDelegate(OnError);

	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGQuery::Get"));

	if (!IsQueryValid())
	{
		return;
	}

#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(UFGFirestoreLibrary::FGFirestoreClassName, "getQuerySnapshot", "(Lcom/google/firebase/firestore/Query;IJJ)V",
		JavaQueryObject, (int) Source, (jlong) SuccessCallback, (jlong) ErrorCallback);
#elif PLATFORM_IOS
	FIRFirestoreSource source;
	if (Source == EFirestoreSourceType::Cache) {
		source = FIRFirestoreSourceCache;
	} else if (Source == EFirestoreSourceType::Server) {
		source = FIRFirestoreSourceServer;
	} else {
		source = FIRFirestoreSourceDefault;
	}
	
	[QueryObject getDocumentsWithSource:source
							 completion:^(FIRQuerySnapshot* snapshot, NSError* error) {
		if (error == nil) {
			UFGQuerySnapshot* Result = NewObject<UFGQuerySnapshot>();
			Result->Init(snapshot);
			SuccessCallback->ExecuteDelegate(Result);
		} else {
			FString Error = FString([error localizedDescription]);
			ErrorCallback->ExecuteDelegate(Error);
		}
	}];
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	auto task = Query.Get((firebase::firestore::Source) Source);
	task.AddOnCompletion([=](const ::firebase::Future<firebase::firestore::QuerySnapshot>& callback) {
		if (callback.status() == firebase::FutureStatus::kFutureStatusComplete)
		{
			UFGQuerySnapshot* Result = NewObject<UFGQuerySnapshot>();
			Result->Init(*callback.result());
			SuccessCallback->ExecuteDelegate(Result);
		}
		else
		{
			FString ErrorMessage = FString(callback.error_message());
			UE_LOG(LogFirebaseGoodies, Error, TEXT("UFGQuery::Get error: %s"), *ErrorMessage);
			ErrorCallback->ExecuteDelegate(ErrorMessage);
		}

		FGUtils::ClearStringCopies();
	});
#endif
}

UFGListenerRegistration* UFGQuery::AddSnapshotListener(const FFirestoreQuerySnapshotDelegate& OnEvent, const FFirestoreListenerErrorDelegate& OnEventError, EFirestoreListenerExecutorType ExecutorType, bool IncludeOnlyMetadataChanges)
{
	OnListenerEventCallback = OnEvent;
	OnListenerEventErrorCallback = OnEventError;
	
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGQuery::AddSnapshotListener"));
	
	UFGListenerRegistration* Result = NewObject<UFGListenerRegistration>();

	if (!IsQueryValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	jobject javaListener = FGMethodCallUtils::CallStaticObjectMethod(UFGFirestoreLibrary::FGFirestoreClassName, "addQueryListener", "(Lcom/google/firebase/firestore/Query;IZJ)Lcom/google/firebase/firestore/ListenerRegistration;",
		JavaQueryObject, (int) ExecutorType, IncludeOnlyMetadataChanges, (jlong) this);
	Result->Init(javaListener);
#elif PLATFORM_IOS
	id<FIRListenerRegistration> iosListener = [QueryObject addSnapshotListenerWithIncludeMetadataChanges:IncludeOnlyMetadataChanges
																  listener:^(FIRQuerySnapshot* snapshot, NSError* error) {
		if (error == nil) {
			UFGQuerySnapshot* Snapshot = NewObject<UFGQuerySnapshot>();
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
	auto listener = Query.AddSnapshotListener(includeMetadataChanges, [=](const firebase::firestore::QuerySnapshot& snapshot, firebase::firestore::Error error, const std::string& errorMessage) {
		if (error == firebase::firestore::Error::kErrorOk)
		{
			UFGQuerySnapshot* Result = NewObject<UFGQuerySnapshot>();
			Result->Init(snapshot);

			AsyncTask(ENamedThreads::GameThread, [=]() {
				OnListenerEventCallback.ExecuteIfBound(Result);
			});
		}
		else
		{
			FString ErrorMessage = FString(errorMessage.c_str());
			UE_LOG(LogFirebaseGoodies, Error, TEXT("OnListenerEventErrorCallback error: %s"), *ErrorMessage);

			EFirestoreErrorCode Code = (EFirestoreErrorCode)error;

			AsyncTask(ENamedThreads::GameThread, [=]() {
				OnListenerEventErrorCallback.ExecuteIfBound(Code);
			});
		}
	});
	Result->Init(listener);
#endif
	
	return Result;
}

bool UFGQuery::IsQueryValid()
{
	bool Result = false;

#if PLATFORM_ANDROID
	Result = JavaQueryObject != nullptr;
#elif PLATFORM_IOS
	Result = QueryObject != nil;
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
void UFGQuery::InitQuery(jobject javaQuery)
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	JavaQueryObject = Env->NewGlobalRef(javaQuery);
}
#endif

#if PLATFORM_IOS
void UFGQuery::InitQuery(FIRQuery* iosQuery)
{
	QueryObject = (FIRQuery*)CFBridgingRetain(iosQuery);
}
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
void UFGQuery::InitQuery(firebase::firestore::Query query)
{
	Query = query;
}
#endif
