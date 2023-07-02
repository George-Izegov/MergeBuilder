// Copyright (c) 2022 Nineva Studios

#pragma once

#include "Common/FGValueVariant.h"

#include "FGDatabaseRef.generated.h"

class UFGDataSnapshot;
class UFGMutableData;
class UFGTransactionTask;

class IDatabaseRef;

UENUM(BlueprintType)
enum class EChileEventType : uint8
{
	Added = 0,
	Changed,
	Removed,
	Moved
};

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnDataChangedDelegate, UFGDataSnapshot*, Data);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnCancelledDelegate, int, ErrorCode, FString, ErrorMessage);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnChildEventDelegate, EChileEventType, EventType, UFGDataSnapshot*, Data, FString, PreviousChildName);

UCLASS(BlueprintType)
class FIREBASEGOODIES_API UFGDatabaseRef : public UObject
{
	GENERATED_BODY()

public:
	UFGDatabaseRef();

	void InitPlatformData(const FString& DatabaseUrl, const FString& InitialPath);

	/**
	 * Create a database reference
	 *
	 * @return - Database reference
	 */
	UFUNCTION(BlueprintPure, Category = "Firebase Goodies|Realtime Database")
	static UFGDatabaseRef* MakeDatabaseRootRef();

	/**
	 * Create a database reference
	 *
	 * @param Path - Path to the initial database ref
	 * @return - Database ref
	 */
	UFUNCTION(BlueprintPure, Category = "Firebase Goodies|Realtime Database")
	static UFGDatabaseRef* MakeDatabaseRefFromPath(const FString& Path);

	/**
	 * Create a database reference.
	 *
	 * @param DatabaseUrl - database URL. Leave empty for default database URL.
	 * @param Path - relative path of the element. Leave empty for root reference.
	 * @return - Database reference
	 */
	UFUNCTION(BlueprintPure, Category = "Firebase Goodies|Realtime Database")
	static UFGDatabaseRef* MakeDatabaseReference(const FString& DatabaseUrl = "", const FString& Path = "");

	/**
	 * Get a reference to location relative to this one
	 *
	 * @param Path - Path to the new database ref
	 * @return - New database ref pointing to the child at the provided path
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Realtime Database")
	UFGDatabaseRef* Child(const FString& Path) const;

	/**
	 * Get a reference to the parent of this location
	 *
	 * @return - New database ref pointing to the parent of the current location
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Realtime Database")
	UFGDatabaseRef* Parent() const;

	/**
	 * Get a reference to the root
	 *
	 * @return - New database ref pointing to the root of the Database
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Realtime Database")
	UFGDatabaseRef* Root() const;

	/**
	 * Set value at the current path
	 *
	 * @param Value - Value to set at the current path
	 * @param Priority - Priority to set at the current path
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Realtime Database", meta = (AdvancedDisplay = "Priority", AutoCreateRefTerm = "Priority"))
	void SetValue(const FFGValueVariant& Value, const FFGValueVariant& Priority);

	/**
	 * Set value as the server timestamp at the current path
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Realtime Database")
	void SetTimestamp();

	/**
	 * Set priority at the current path
	 *
	 * @param Priority - Priority to set at the current path
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Realtime Database")
	void SetPriority(const FFGValueVariant& Priority);

	/**
	 * Get the key name for the current location
	 *
	 * @return - Key string value
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Realtime Database")
	FString GetKey() const;

	/**
	 * Query the database to receive a data snapshot at the current path.
	 *
	 * @param OnDataReceived - Called when the query successfully retrieved the data
	 * @param OnCancelled - Called when the query failed to receive the data
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Realtime Database", meta = (AutoCreateRefTerm = "OnCancelled"))
	void GetValue(const FOnDataChangedDelegate& OnDataReceived, const FOnCancelledDelegate& OnCancelled);

	/**
	 * Create a child at the current location
	 *
	 * @return - New database ref pointing to the new generated child
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Realtime Database")
	UFGDatabaseRef* Push();

	/**
	 * Remove value at the current path
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Realtime Database")
	void RemoveValue();

	/**
	 * Update the specific child keys to the specified values
	 *
	 * @param Update - The paths and values to update
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Realtime Database")
	void UpdateChildren(const TMap<FString, FFGValueVariant>& Update);

	/**
	 * Subscribe to receive events when a value changes at the current path.
	 *
	 * @param OnDataChanged - Called when the query successfully retrieved the data or the data is updated
	 * @param OnCancelled - Called when the query failed to receive the data
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Realtime Database", meta = (AutoCreateRefTerm = "OnCancelled"))
	void AddValueListener(const FOnDataChangedDelegate& OnDataChanged, const FOnCancelledDelegate& OnCancelled);

	/**
	 * Stop listening for value change events.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Realtime Database")
	void RemoveValueListener();

	/**
	 * Subscribe to receive events when a child node changes at the current path.
	 *
	 * @param OnChildEvent - Called when child nodes are added, changed, removed or moved
	 * @param OnCancelled - Called when the query failed to receive the data
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Realtime Database", meta = (AutoCreateRefTerm = "OnCancelled"))
	void AddChildListener(const FOnChildEventDelegate& OnChildEvent, const FOnCancelledDelegate& OnCancelled);

	/**
	 * Stop listening for child change events.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Realtime Database")
	void RemoveChildListener();

	/**
	 * The data at this location will be automatically downloaded and kept in sync.
	 *
	 * @param Sync - Pass true to keep this location in sync
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Realtime Database")
	void KeepSynced(bool Sync);

	/**
	 * Create a query in which child nodes are ordered by the values of the specified path.
	 *
	 * @param Path - Path to the child node to use for sorting
	 * @return - Modified database ref containing ordered data
	 */
	UFUNCTION(BlueprintPure, Category = "Firebase Goodies|Realtime Database")
	UFGDatabaseRef* OrderByChild(const FString& Path);

	/**
	 * Create a query in which child nodes are ordered by their keys.
	 *
	 * @return - Modified database ref containing ordered data
	 */
	UFUNCTION(BlueprintPure, Category = "Firebase Goodies|Realtime Database")
	UFGDatabaseRef* OrderByKey();

	/**
	 * Create a query in which child nodes are ordered by their priorities.
	 *
	 * @return - Modified database ref containing ordered data
	 */
	UFUNCTION(BlueprintPure, Category = "Firebase Goodies|Realtime Database")
	UFGDatabaseRef* OrderByPriority();

	/**
	 * Create a query in which nodes are ordered by their value
	 *
	 * @return - Modified database ref containing ordered data
	 */
	UFUNCTION(BlueprintPure, Category = "Firebase Goodies|Realtime Database")
	UFGDatabaseRef* OrderByValue();

	/**
	 * Create a query constrained to only return child nodes with a value and/or key less than or equal to the given value.
	 *
	 * @param Value - The value to end at, inclusive
	 * @param Key - The key to end at, inclusive
	 * @return - Modified database ref containing ordered data
	 */
	UFUNCTION(BlueprintPure, Category = "Firebase Goodies|Realtime Database")
	UFGDatabaseRef* EndAtFloat(float Value, const FString& Key);

	/**
	 * Create a query constrained to only return child nodes with a value and/or key less than or equal to the given value.
	 *
	 * @param Value - The value to end at, inclusive
	 * @param Key - The key to end at, inclusive
	 * @return - Modified database ref containing ordered data
	 */
	UFUNCTION(BlueprintPure, Category = "Firebase Goodies|Realtime Database")
	UFGDatabaseRef* EndAtString(const FString& Value, const FString& Key);

	/**
	 * Create a query constrained to only return child nodes with a value and/or key greater than or equal to the given value.
	 *
	 * @param Value - The value to start at, inclusive
	 * @param Key - The key to start at, inclusive
	 * @return - Modified database ref containing ordered data
	 */
	UFUNCTION(BlueprintPure, Category = "Firebase Goodies|Realtime Database")
	UFGDatabaseRef* StartAtFloat(float Value, const FString& Key);

	/**
	 * Create a query constrained to only return child nodes with a value and/or key greater than or equal to the given value.
	 *
	 * @param Value - The value to start at, inclusive
	 * @param Key - The key to start at, inclusive
	 * @return - Modified database ref containing ordered data
	 */
	UFUNCTION(BlueprintPure, Category = "Firebase Goodies|Realtime Database")
	UFGDatabaseRef* StartAtString(const FString& Value, const FString& Key);

	/**
	 * Create a query constrained to only return the child node with the given value and/or key.
	 *
	 * @param Value - The value to query for
	 * @param Key - The name of the child
	 * @return - Modified database ref containing ordered data
	 */
	UFUNCTION(BlueprintPure, Category = "Firebase Goodies|Realtime Database")
	UFGDatabaseRef* EqualToFloat(float Value, const FString& Key);

	/**
	 * Create a query constrained to only return the child node with the given value and/or key.
	 *
	 * @param Value - The value to query for
	 * @param Key - The name of the child
	 * @return - Modified database ref containing ordered data
	 */
	UFUNCTION(BlueprintPure, Category = "Firebase Goodies|Realtime Database")
	UFGDatabaseRef* EqualToBool(bool Value, const FString& Key);

	/**
	 * Create a query constrained to only return the child node with the given value and/or key.
	 *
	 * @param Value - The value to query for
	 * @param Key - The name of the child
	 * @return - Modified database ref containing ordered data
	 */
	UFUNCTION(BlueprintPure, Category = "Firebase Goodies|Realtime Database")
	UFGDatabaseRef* EqualToString(const FString& Value, const FString& Key);

	/**
	 * Create a query with limit and anchor it to the beginning of the dataset.
	 *
	 * @param Limit - The maximum number of child nodes to return
	 * @return - Modified database ref containing limited data
	 */
	UFUNCTION(BlueprintPure, Category = "Firebase Goodies|Realtime Database")
	UFGDatabaseRef* LimitToFirst(int Limit);

	/**
	 * Create a query with limit and anchor it to the end of the dataset.
	 *
	 * @param Limit - The maximum number of child nodes to return
	 * @return - Modified database ref containing limited data
	 */
	UFUNCTION(BlueprintPure, Category = "Firebase Goodies|Realtime Database")
	UFGDatabaseRef* LimitToLast(int Limit);

	/**
	 * Set value at the current path when a disconnect event occurs.
	 *
	 * @param Value - Value to set at the current path
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Realtime Database")
	void OnDisconnectSetValue(const FFGValueVariant& Value);

	/**
	 * Set value as the server timestamp at the current path when a disconnect event occurs.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Realtime Database")
	void OnDisconnectSetTimestamp();

	/**
	 * Remove value at the current path when a disconnect event occurs.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Realtime Database")
	void OnDisconnectRemoveValue();

	/**
	 * Update the specific child keys to the specified values when a disconnect event occurs.
	 *
	 * @param Update - The paths and values to update
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Realtime Database")
	void OnDisconnectUpdateChildren(const TMap<FString, FFGValueVariant>& Update);

	/**
	 * Remove value at the current path when a disconnect event occurs.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Realtime Database")
	void CancelDisconnectOperations();

	void BindTransactionTask(UFGTransactionTask* Task);
	void RunTransaction();

private:
	FString GetPathToParent() const;

	FString RefPath;

	TSharedPtr<IDatabaseRef> DatabaseRefImpl;
};
