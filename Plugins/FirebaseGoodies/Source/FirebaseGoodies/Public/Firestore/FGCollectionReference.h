// Copyright (c) 2021 Nineva Studios

#pragma once

#include "FGQuery.h"

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

#include "FGCollectionReference.generated.h"

class UFGDocumentReference;

/**
* A UFGCollectionReference object can be used for adding documents, getting document references, and querying for documents (using the methods inherited from UFGQuery).
*/
UCLASS(BlueprintType)
class FIREBASEGOODIES_API UFGCollectionReference : public UFGQuery
{
	GENERATED_BODY()

	virtual ~UFGCollectionReference();

public:

	/**
	* Get the ID of the referenced collection.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore")
	FString GetId();

	/**
	* For subcollections, parent returns the containing DocumentReference. For root collections, empty object is returned.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore")
	UFGDocumentReference* GetParent();

	/**
	* Get the string containing the slash-separated path to this this CollectionReference (relative to the root of the database).
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore")
	FString GetPath();

	/**
	* Returns a UFGDocumentReference pointing to a new document with an auto-generated ID.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore")
	UFGDocumentReference* CreateDocument();

	/**
	* Gets a UFGDocumentReference referring to the document at the specified path, relative to this collection’s own path.
	 *
	 * @param Path - The slash-separated relative path of the document for which to get a UFGDocumentReference.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore")
	UFGDocumentReference* GetDocument(const FString& Path);

	/**
	* Adds a new document to this collection with the specified data, assigning it a document ID automatically.
	 *
	 * @param Data - A map containing the data for the new document.
	 * @param OnSuccess - Callback to be invoked after a successful operation.
	 * @param OnError - Callback to be invoked if an error occurs.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore", meta = (AutoCreateRefTerm = "OnSuccess, OnError"))
	void AddItem(const TMap<FString, FFGValueVariant>& Data, const FFirestoreDocumentDelegate& OnSuccess, const FFirestoreStringDelegate& OnError);

	/**
	 * Convenience function to check whether the native object is valid.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore")
	bool IsCollectionReferenceValid();

#if PLATFORM_ANDROID
	void Init(jobject javaCollectionReference);
	jobject JavaCollectionReferenceObject;
#endif
	
#if PLATFORM_IOS
	void Init(FIRCollectionReference* iosCollectionReference);
	FIRCollectionReference* CollectionReferenceObject;
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	void Init(firebase::firestore::CollectionReference collectionReference);
	firebase::firestore::CollectionReference CollectionReference;
#endif
};
