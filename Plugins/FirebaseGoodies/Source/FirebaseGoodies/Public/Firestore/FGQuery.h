// Copyright (c) 2021 Nineva Studios

#pragma once

#include "UObject/Object.h"
#include "FGFirestoreLibrary.h"
#include "Common/FGValueVariant.h"

#if PLATFORM_ANDROID
#include "Android/Utils/FGJavaConvertor.h"
#include "Android/Utils/FGMethodCallUtils.h"
#include "Android/Utils/FGValueVariantUtils.h"
#endif

#if PLATFORM_IOS
#import <FirebaseFirestore/FirebaseFirestore.h>
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
#include "firebase/firestore.h"
#endif

#include "FGQuery.generated.h"

class UFGDocumentSnapshot;
class UFGListenerRegistration;

/**
* A UFGQuery refers to a Query which you can read or listen to. You can also construct refined UFGQuery objects by adding filters and ordering.
*/
UCLASS(BlueprintType)
class FIREBASEGOODIES_API UFGQuery : public UObject
{
	GENERATED_BODY()

protected:
	virtual ~UFGQuery();

public:

	/**
	* Creates and returns a new UFGQuery with the additional filter that documents must contain the specified field and the value must be equal to the specified value.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore")
	UFGQuery* WhereEqualTo(const FString& Field, const FFGValueVariant& Value);

	/**
	* Creates and returns a new UFGQuery with the additional filter that documents must contain the specified field and the value does not equal the specified value.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore")
	UFGQuery* WhereNotEqualTo(const FString& Field, const FFGValueVariant& Value);

	/**
	* Creates and returns a new UFGQuery with the additional filter that documents must contain the specified field and the value must be less than the specified value.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore")
	UFGQuery* WhereLessThan(const FString& Field, const FFGValueVariant& Value);

	/**
	* Creates and returns a new UFGQuery with the additional filter that documents must contain the specified field and the value must be less than or equal to the specified value.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore")
	UFGQuery* WhereLessThanOrEqualTo(const FString& Field, const FFGValueVariant& Value);

	/**
	* Creates and returns a new UFGQuery with the additional filter that documents must contain the specified field and the value must be greater than the specified value.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore")
	UFGQuery* WhereGreaterThan(const FString& Field, const FFGValueVariant& Value);

	/**
	* Creates and returns a new UFGQuery with the additional filter that documents must contain the specified field and the value must be greater than  or equal to the specified value.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore")
	UFGQuery* WhereGreaterThanOrEqualTo(const FString& Field, const FFGValueVariant& Value);

	/**
	* Creates and returns a new UFGQuery with the additional filter that documents must contain the specified field, it must be an array, and the array must contain the provided value.
	* A query can have only one WhereArrayContains filter.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore")
	UFGQuery* WhereArrayContains(const FString& Field, const FFGValueVariant& Value);

	/**
	* Creates and returns a new UFGQuery with the additional filter that documents must contain the specified field, the value must be an array,
	* and that array must contain at least one value from the provided array.
	* A query can have only one WhereArrayContainsAny filter and it cannot be combined with WhereArrayContains or WhereIn filters.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore")
	UFGQuery* WhereArrayContainsAny(const FString& Field, const TArray<FFGValueVariant>& Values);

	/**
	* Creates and returns a new UFGQuery with the additional filter that documents must contain the specified field and the value must equal one of the values from the provided array.
	* A query can have only one WhereIn filter, and it cannot be combined with an WhereArrayContainsAny filter.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore")
	UFGQuery* WhereIn(const FString& Field, const TArray<FFGValueVariant>& Values);

	/**
	* Creates and returns a new UFGQuery with the additional filter that documents must contain the specified field and the value does not equal any of the values from the provided array.
	* One special case is that WhereNotIn filters cannot match nil values. To query for documents where a field exists and is nil, use a WhereNotEqual filter, which can handle this special case.
	* A query can have only one WhereNotIn filter, and it cannot be combined with an WhereArrayContains, WhereArrayContainsAny, in, or WhereNotEqual filter.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore")
	UFGQuery* WhereNotIn(const FString& Field, const TArray<FFGValueVariant>& Values);

	/**
	* Creates and returns a new UFGQuery that’s additionally sorted by the specified field.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore")
	UFGQuery* OrderBy(const FString& Field, bool Ascending);

	/**
	* Creates and returns a new UFGQuery that only returns the first matching documents up to the specified number.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore")
	UFGQuery* Limit(int64 Value);

	/**
	* Creates and returns a new UFGQuery that only returns the last matching documents up to the specified number.
	* A query with a LimitToLast clause must have at least one OrderBy clause.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore")
	UFGQuery* LimitToLast(int64 Value);

	/**
	* Creates and returns a new UFGQuery that starts at the provided document (inclusive). The starting position is relative to the order of the query.
	* The document must contain all of the fields provided in the OrderBy of this query.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore")
	UFGQuery* StartAt(UFGDocumentSnapshot* Document);

	/**
	* Creates and returns a new UFGQuery that starts after the provided document (exclusive). The starting position is relative to the order of the query.
	* The document must contain all of the fields provided in the OrderBy of this query.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore")
	UFGQuery* StartAfter(UFGDocumentSnapshot* Document);

	/**
	* Creates and returns a new UFGQuery that ends before the provided document (exclusive). The end position is relative to the order of the query.
	* The document must contain all of the fields provided in the OrderBy of this query.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore")
	UFGQuery* EndBefore(UFGDocumentSnapshot* Document);

	/**
	* Creates and returns a new UFGQuery that ends at the provided document (inclusive). The end position is relative to the order of the query.
	* The document must contain all of the fields provided in the OrderBy of this query.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore")
	UFGQuery* EndAt(UFGDocumentSnapshot* Document);

	/**
	* Reads the documents matching this query.
	 *
	 * @param OnSuccess - Callback to be invoked after a successful operation.
	 * @param OnError - Callback to be invoked if an error occurs.
	 * @param Source - indicates whether the results should be fetched from the cache only, the server only, or to attempt the server and fall back to the cache.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore", meta = (AutoCreateRefTerm = "OnSuccess, OnError"))
	void Get(const FFirestoreQuerySnapshotDelegate& OnSuccess, const FFirestoreStringDelegate& OnError, EFirestoreSourceType Source = EFirestoreSourceType::Default);
	
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore", meta = (AutoCreateRefTerm = "OnEvent, OnEventError"))
	UFGListenerRegistration* AddSnapshotListener(const FFirestoreQuerySnapshotDelegate& OnEvent, const FFirestoreListenerErrorDelegate& OnEventError, EFirestoreListenerExecutorType ExecutorType = EFirestoreListenerExecutorType::Default, bool IncludeOnlyMetadataChanges = false);

	/**
	 * Convenience function to check whether the native object is valid.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore")
	bool IsQueryValid();
	
	FFirestoreQuerySnapshotDelegate OnListenerEventCallback;
	FFirestoreListenerErrorDelegate OnListenerEventErrorCallback;

#if PLATFORM_ANDROID
	void InitQuery(jobject javaQuery);
	jobject JavaQueryObject;
#endif
	
#if PLATFORM_IOS
	void InitQuery(FIRQuery* iosQuery);
	FIRQuery* QueryObject;
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	void InitQuery(firebase::firestore::Query query);
	firebase::firestore::Query Query;
#endif
};
