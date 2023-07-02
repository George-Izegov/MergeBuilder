// Copyright (c) 2021 Nineva Studios

#pragma once

#include "UObject/Object.h"

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

#include "FGQuerySnapshot.generated.h"

class UFGDocumentSnapshot;
class UFGQuery;

/**
* A UFGQuerySnapshot contains zero or more UFGDocumentSnapshot objects.
*/
UCLASS(BlueprintType)
class FIREBASEGOODIES_API UFGQuerySnapshot : public UObject
{
	GENERATED_BODY()

	virtual ~UFGQuerySnapshot();

public:
	
	/**
	* The query on which you called Get() or listened to in order to get this UFGQuerySnapshot.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore")
	UFGQuery* GetQuery();

	/**
	* An Array of the UFGDocumentSnapshot that make up this document set.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore")
	TArray<UFGDocumentSnapshot*> GetDocuments();
	
	/**
	* Returns true if the snapshot contains the result of local writes that have not yet been committed to the backend.
	* If your listener has opted into metadata updates (via includeMetadataChanges:true) you will receive another snapshot with hasPendingWrites equal to NO
	* once the writes have been committed to the backend.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore")
	bool HasPendingWrites();
	
	/**
	* Returns true if the snapshot was created from cached data rather than guaranteed up-to-date server data.
	* If your listener has opted into metadata updates (via includeMetadataChanges:true) you will receive another snapshot with IsFromCache equal to NO
	* once the writes have been committed to the backend.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore")
	bool IsFromCache();

	/**
	 * Convenience function to check whether the native object is valid.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore")
	bool IsQuerySnapshotValid();

#if PLATFORM_ANDROID
	void Init(jobject javaQuerySnapshot);
	jobject JavaQuerySnapshotObject;
#endif
	
#if PLATFORM_IOS
	void Init(FIRQuerySnapshot* iosQuerySnapshot);
	FIRQuerySnapshot* QuerySnapshotObject;
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	void Init(firebase::firestore::QuerySnapshot querySnapshot);
	firebase::firestore::QuerySnapshot QuerySnapshot;
#endif
};
