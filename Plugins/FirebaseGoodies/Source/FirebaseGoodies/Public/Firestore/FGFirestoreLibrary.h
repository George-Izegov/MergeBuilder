// Copyright (c) 2021 Nineva Studios

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Common/FGValueVariant.h"

#if PLATFORM_ANDROID
#include "Android/Utils/FGJavaConvertor.h"
#include "Android/Utils/FGMethodCallUtils.h"
#include "Android/Utils/FGValueVariantUtils.h"
#endif

#include "FGFirestoreLibrary.generated.h"

class UFGWriteBatch; 
class UFGCollectionReference; 
class UFGDocumentReference;
class UFGDocumentSnapshot;
class UFGQuery;
class UFGQuerySnapshot;

/**
* An enum that configures the behavior of DocumentReference.GetDocument() and Query.GetDocuments().
* By providing a source enum these methods can be configured to fetch results only from the server, only from the local cache,
* or attempt to fetch results from the server and fall back to the cache (which is the default).
*/
UENUM(BlueprintType)
enum class EFirestoreSourceType : uint8
{
	/**
	* Causes Firestore to try to retrieve an up-to-date (server-retrieved) snapshot, but fall back to returning cached data if the server can’t be reached.
	*/
	Default = 0,
	/**
	* Causes Firestore to avoid the cache, generating an error if the server cannot be reached.
	* Note that the cache will still be updated if the server request succeeds. Also note that latency-compensation still takes effect,
	* so any pending write operations will be visible in the returned data (merged into the server-provided data).
	*/
	Server = 1,
	/**
	* Causes Firestore to immediately return a value from the cache, ignoring the server completely (implying that the returned value may be stale with respect to the value on the server).
	* If there is no data in the cache to satisfy the GetDocument[s] call, DocumentReference.GetDocument() will return an error and QuerySnapshot.GetDocuments()
	* will return an empty QuerySnapshot with no documents.
	*/
	Cache = 2
};

/**
* An enum that configures SetData operation behaviour for DocumentReference.
*/
UENUM(BlueprintType)
enum class EDocumentSetOptions : uint8
{
	/**
	* The data will not be merged with the existing document data.
	*/
	Overwrite = 0,
	/**
	* The data will be merged with the existing document data.
	*/
	Merge = 1,
};

UENUM(BlueprintType)
enum class EFirestoreListenerExecutorType : uint8
{
	/**
	* The default executor for user visible callbacks. It is an executor scheduling callbacks on Android's main thread.
	*/
	Default = 0,
	/**
	* An executor that executes the provided runnable immediately on the current thread.
	*/
	Direct = 1,
	/**
	* An executor that runs tasks in parallel on Android's AsyncTask.THREAD_POOL_EXECUTOR.
	*/
	Background = 2
};

UENUM(BlueprintType)
enum class EFirestoreErrorCode : uint8
{
	/**
	* The operation completed successfully.
	*/
	Ok = 0,

	/** The operation was cancelled (typically by the caller). */
	Cancelled = 1,

	/** Unknown error or an error from a different error domain. */
	Unknown = 2,

	/**
	 * Client specified an invalid argument. Indicates arguments that are problematic regardless of the state of the system (e.g., an invalid field name).
	 */
	InvalidArgument = 3,

	/**
	 * Deadline expired before operation could complete. For operations that change the state of the
	 * system, this error may be returned even if the operation has completed successfully. For
	 * example, a successful response from a server could have been delayed long enough for the
	 * deadline to expire.
	 */
	DeadlineExceeded = 4,

	/** Some requested document was not found. */
	NotFound = 5,

	/** Some document that we attempted to create already exists. */
	AlreadyExists = 6,

	/** The caller does not have permission to execute the specified operation. */
	PermissionDenied = 7,

	/**
	 * Some resource has been exhausted, perhaps a per-user quota, or perhaps the entire file system
	 * is out of space.
	 */
	ResourceExhausted = 8,

	/**
	 * Operation was rejected because the system is not in a state required for the operation's
	 * execution.
	 */
	FailedPrecondition = 9,

	/**
	 * The operation was aborted, typically due to a concurrency issue like transaction aborts, etc.
	 */
	Aborted = 10,

	/** Operation was attempted past the valid range. */
	OutOfRange = 11,

	/** Operation is not implemented or not supported/enabled. */
	Unimplemented = 12,

	/**
	 * Internal errors. Means some invariants expected by underlying system has been broken. If you
	 * see one of these errors, something is very broken.
	 */
	Internal = 13,

	/**
	 * The service is currently unavailable. This is a most likely a transient condition and may be
	 * corrected by retrying with a backoff.
	 */
	Unavailable = 14,

	/** Unrecoverable data loss or corruption. */
	DataLoss = 15,

	/** The request does not have valid authentication credentials for the operation. */
	Unauthenticated = 16
};

DECLARE_DYNAMIC_DELEGATE(FFirestoreVoidDelegate);
DECLARE_DYNAMIC_DELEGATE_OneParam(FFirestoreStringDelegate, const FString&, String);
DECLARE_DYNAMIC_DELEGATE_OneParam(FFirestoreDocumentDelegate, UFGDocumentReference*, Document);
DECLARE_DYNAMIC_DELEGATE_OneParam(FFirestoreDocumentSnapshotDelegate, UFGDocumentSnapshot*, DocumentSnapshot);
DECLARE_DYNAMIC_DELEGATE_OneParam(FFirestoreQuerySnapshotDelegate, UFGQuerySnapshot*, QuerySnapshot);
DECLARE_DYNAMIC_DELEGATE_OneParam(FFirestoreListenerErrorDelegate, EFirestoreErrorCode, ErrorCode);

UCLASS()
class FIREBASEGOODIES_API UFGFirestoreLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	/**
	* Gets a UFGCollectionReference referring to the collection at the specified path within the database.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore")
	static UFGCollectionReference* GetCollection(const FString& CollectionPath);

	/**
	* Creates a write batch, used for performing multiple writes as a single atomic operation.
	* The maximum number of writes allowed in a single batch is 500.
	* Unlike transactions, write batches are persisted offline and therefore are preferable when you don’t need to condition your writes on read data.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore")
	static UFGWriteBatch* GetWriteBatch();

	/**
	* Gets a UFGDocumentReference referring to the document at the specified path within the database.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore")
	static UFGDocumentReference* GetDocument(const FString& Path);

	/**
	* Creates and returns a new Query that includes all documents in the database that are contained in a collection or subcollection with the given collectionID.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore")
	static UFGQuery* GetCollectionQuery(const FString& CollectionId);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase Goodies|Firestore")
	static FFGValueVariant FirestoreTimestamp();

	static const ANSICHAR* FGFirestoreClassName;
};
