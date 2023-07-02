// Copyright (c) 2021 Nineva Studios

#pragma once

#include "UObject/Object.h"
#include "FGFirestoreLibrary.h"

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

#include "FGDocumentReference.generated.h"

class UFGCollectionReference;
class UFGDocumentSnapshot;
class UFGListenerRegistration;

/**
* A UFGDocumentReference refers to a document location in a Firestore database and can be used to write, read, or listen to the location.
* The document at the referenced location may or may not exist. A UFGDocumentReference can also be used to create a UFGCollectionReference to a subcollection.
*/
UCLASS(BlueprintType)
class FIREBASEGOODIES_API UFGDocumentReference : public UObject
{
	GENERATED_BODY()

	virtual ~UFGDocumentReference();

public:

	/**
	* The ID of the document referred to.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore")
	FString GetId();

	/**
	* A reference to the collection to which this UFGDocumentReference belongs.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore")
	UFGCollectionReference* GetParent();

	/**
	* A string representing the path of the referenced document (relative to the root of the database).
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore")
	FString GetPath();

	/**
	* Gets a UFGCollectionReference referring to the collection at the specified path, relative to this document.
	*
	* @param Path - The slash-separated relative path of the collection for which to get a UFGCollectionReference.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore")
	UFGCollectionReference* GetCollection(const FString& Path);

	/**
	* Writes to the document referred to by this DocumentReference. If the document does not yet exist, it will be created.
	* If you pass EDocumentSetOptions::Merge, the provided data will be merged into any existing document.
	*
	* @param Data - A map containing the data for the document.
	* @param OnSuccess - Callback to be invoked after a successful operation.
	* @param OnError - Callback to be invoked if an error occurs.
	* @param SetOptions - Whether to merge the provided data into any existing document.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore", meta = (AutoCreateRefTerm = "OnSuccess, OnError"))
	void SetData(const TMap<FString, FFGValueVariant>& Data, const FFirestoreVoidDelegate& OnSuccess, const FFirestoreStringDelegate& OnError, EDocumentSetOptions SetOptions = EDocumentSetOptions::Overwrite);

	/**
	* Updates fields in the document referred to by this UFGDocumentReference. If the document does not exist, the update fails and the specified completion block receives an error.
	*
	* @param Data - A map containing the data for the document.
	* @param OnSuccess - Callback to be invoked after a successful operation.
	* @param OnError - Callback to be invoked if an error occurs.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore", meta = (AutoCreateRefTerm = "OnSuccess, OnError"))
	void UpdateData(const TMap<FString, FFGValueVariant>& Data, const FFirestoreVoidDelegate& OnSuccess, const FFirestoreStringDelegate& OnError);

	/**
	* Deletes the document referred to by this UFGDocumentReference.
	*
	* @param OnSuccess - Callback to be invoked after a successful operation.
	* @param OnError - Callback to be invoked if an error occurs.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore", meta = (AutoCreateRefTerm = "OnSuccess, OnError"))
	void Delete(const FFirestoreVoidDelegate& OnSuccess, const FFirestoreStringDelegate& OnError);

	/**
	* Reads the document referenced by this UFGDocumentReference.
	*
	* @param OnSuccess - Callback to be invoked after a successful operation.
	* @param OnError - Callback to be invoked if an error occurs.
	* @param SourceType - indicates whether the results should be fetched from the cache only, the server only, or to attempt the server and fall back to the cache.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore", meta = (AutoCreateRefTerm = "OnSuccess, OnError"))
	void GetDocumentSnapshot(const FFirestoreDocumentSnapshotDelegate& OnSuccess, const FFirestoreStringDelegate& OnError, EFirestoreSourceType SourceType = EFirestoreSourceType::Default);

	/**
	* Attaches a listener for DocumentSnapshot events.
	*
	* @param OnEvent - Callback to be invoked after a document is changed.
	* @param OnEventError - Callback to be invoked if an error occurs.
	* @param ExecutorType - The way to run the callback on Android. Ignored on iOS.
	* @param IncludeOnlyMetadataChanges - Whether metadata-only changes should trigger snapshot events.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore", meta = (AutoCreateRefTerm = "OnEvent, OnEventError"))
	UFGListenerRegistration* AddSnapshotListener(const FFirestoreDocumentSnapshotDelegate& OnEvent, const FFirestoreListenerErrorDelegate& OnEventError, EFirestoreListenerExecutorType ExecutorType = EFirestoreListenerExecutorType::Default, bool IncludeOnlyMetadataChanges = false);

	/**
	* Convenience function to check whether the native object is valid.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore")
	bool IsDocumentReferenceValid();

	FFirestoreDocumentSnapshotDelegate OnListenerEventCallback;
	FFirestoreListenerErrorDelegate OnListenerEventErrorCallback;

#if PLATFORM_ANDROID
	void Init(jobject javaDocumentReference);
	jobject JavaDocumentReferenceObject;
#endif
	
#if PLATFORM_IOS
	void Init(FIRDocumentReference* iosDocumentReference);
	FIRDocumentReference* DocumentReferenceObject;
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	void Init(firebase::firestore::DocumentReference documentReference);
	firebase::firestore::DocumentReference DocumentReference;
#endif
};
