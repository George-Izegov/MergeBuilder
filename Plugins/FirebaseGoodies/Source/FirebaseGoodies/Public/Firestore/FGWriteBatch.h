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

#include "FGWriteBatch.generated.h"

class UFGDocumentReference;

/**
* A write batch is used to perform multiple writes as a single atomic unit. It provides methods for adding writes to the write batch.
* None of the writes will be committed (or visible locally) until Commit is called.
* Unlike transactions, write batches are persisted offline and therefore are preferable when you don’t need to condition your writes on read data.
*/
UCLASS(BlueprintType)
class FIREBASEGOODIES_API UFGWriteBatch : public UObject
{
	GENERATED_BODY()

	virtual ~UFGWriteBatch();

public:

	/**
	* Writes to the document referred to by document. If the document doesn’t yet exist, this method creates it and then sets the data.
	* If the document exists, this method overwrites the document data with the new values.
	 *
	 * @param Document - Document to write the data to.
	 * @param Data - A map containing the data for the document.
	 * @param SetOptions - Whether to merge the provided data into any existing document.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore")
	UFGWriteBatch* AddSetDataOperation(UFGDocumentReference* Document, const TMap<FString, FFGValueVariant>& Data, EDocumentSetOptions SetOptions = EDocumentSetOptions::Overwrite);

	/**
	* Updates fields in the document referred to by document. If document does not exist, the write batch will fail.
	 *
	 * @param Document - Document to write the data to.
	 * @param Data - A map containing the data for the document.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore")
	UFGWriteBatch* AddUpdateDataOperation(UFGDocumentReference* Document, const TMap<FString, FFGValueVariant>& Data);

	/**
	* Deletes the document referred to by Document.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore")
	UFGWriteBatch* AddDeleteOperation(UFGDocumentReference* Document);

	/**
	* Commits all of the writes in this write batch as a single atomic unit.
	 
	 * @param OnSuccess - Callback to be invoked after a successful operation.
	 * @param OnError - Callback to be invoked if an error occurs.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore", meta = (AutoCreateRefTerm = "OnSuccess, OnError"))
	void Commit(const FFirestoreVoidDelegate& OnSuccess, const FFirestoreStringDelegate& OnError);

	/**
	 * Convenience function to check whether the native object is valid.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore")
	bool IsWriteBatchValid();

#if PLATFORM_ANDROID
	void Init(jobject javaWriteBatch);
	jobject JavaWriteBatchObject;
#endif
	
#if PLATFORM_IOS
	void Init(FIRWriteBatch* iosWriteBatch);
	FIRWriteBatch* WriteBatchObject;
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	void Init(firebase::firestore::WriteBatch writeBatch);
	firebase::firestore::WriteBatch WriteBatch;
#endif
};
