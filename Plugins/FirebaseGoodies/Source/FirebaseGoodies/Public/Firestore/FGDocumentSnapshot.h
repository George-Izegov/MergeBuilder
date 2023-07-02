// Copyright (c) 2021 Nineva Studios

#pragma once

#include "UObject/Object.h"
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

#include "FGDocumentSnapshot.generated.h"

class UFGDocumentReference;

/**
 A UFGDocumentSnapshot contains data read from a document in your Firestore database.
 The data can be extracted with the data property or by using subscript syntax to access a specific field.
 For a UFGDocumentSnapshot that points to a non-existing document, any data access will return nil. You can use the exists property to explicitly verify a documents existence.
*/
UCLASS(BlueprintType)
class FIREBASEGOODIES_API UFGDocumentSnapshot : public UObject
{
	GENERATED_BODY()

	virtual ~UFGDocumentSnapshot();

public:

	/**
	* The ID of the document for which this UFGDocumentSnapshot contains data.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore")
	FString GetId();

	/**
	* True if the document exists.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore")
	bool Exists();

	/**
	* Retrieves all fields in the document as a map. Returns empty map if the document doesn’t exist.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore")
	TMap<FString, FFGValueVariant> GetData();

	/**
	* True if Data contains specified field.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore")
	bool Contains(const FString& FieldName);

	/**
	* Retrieves a specific field from the document. Returns FFGValueVariant type will be Null if the document or the field doesn’t exist.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore")
	FFGValueVariant GetValue(const FString& FieldName);

	/**
	* A UFGDocumentReference to the document location. Path is ignored on iOS.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore")
	UFGDocumentReference* GetDocumentReference(const FString& Path = "");
	
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
	bool IsDocumentSnapshotValid();

#if PLATFORM_ANDROID
	void Init(jobject javaDocumentSnapshot);
	jobject JavaDocumentSnapshotObject;
#endif
	
#if PLATFORM_IOS
	void Init(FIRDocumentSnapshot* iosDocumentSnapshot);
	FIRDocumentSnapshot* DocumentSnapshotObject;
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	void Init(firebase::firestore::DocumentSnapshot documentSnapshot);
	firebase::firestore::DocumentSnapshot DocumentSnapshot;
#endif
};
