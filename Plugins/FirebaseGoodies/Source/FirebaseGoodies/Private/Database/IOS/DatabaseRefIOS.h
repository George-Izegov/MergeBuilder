// Copyright (c) 2022 Nineva Studios

#pragma once

#include "Database/Interface/IDatabaseRef.h"

#import <FirebaseDatabase/FirebaseDatabase.h>

class DatabaseRefIOS : public IDatabaseRef
{
public:
	DatabaseRefIOS();
	virtual ~DatabaseRefIOS() override;

	virtual void Init(const FString& DatabaseUrl, const FString& InitialPath) override;

	virtual void SetValue(const FFGValueVariant& Value, const FFGValueVariant& Priority) override;
	virtual void SetTimestamp() override;
	virtual void SetPriority(const FFGValueVariant& Priority) override;

	virtual FString GetKey() const override;
	virtual void GetValue(const FOnDataChangedDelegate& OnDataReceived, const FOnCancelledDelegate& OnCancelled) override;
	virtual void RemoveValue() override;

	virtual FString Push() override;

	virtual void UpdateChildren(const TMap<FString, FFGValueVariant>& Update) override;
	virtual void AddValueListener(const FOnDataChangedDelegate& OnDataChanged, const FOnCancelledDelegate& OnCancelled) override;
	virtual void RemoveValueListener() override;
	virtual void AddChildListener(const FOnChildEventDelegate& OnChildEvent, const FOnCancelledDelegate& OnCancelled) override;
	virtual void RemoveChildListener() override;

	virtual void KeepSynced(bool Sync) override;

	virtual void OrderByChild(const FString& Path) override;
	virtual void OrderByKey() override;
	virtual void OrderByPriority() override;
	virtual void OrderByValue() override;
	virtual void EndAtFloat(float Value, const FString& Key) override;
	virtual void EndAtString(const FString& Value, const FString& Key) override;
	virtual void StartAtFloat(float Value, const FString& Key) override;
	virtual void StartAtString(const FString& Value, const FString& Key) override;
	virtual void EqualToFloat(float Value, const FString& Key) override;
	virtual void EqualToBool(bool Value, const FString& Key) override;
	virtual void EqualToString(const FString& Value, const FString& Key) override;
	virtual void LimitToFirst(int Limit) override;
	virtual void LimitToLast(int Limit) override;

	virtual void OnDisconnectSetValue(const FFGValueVariant& Value) override;
	virtual void OnDisconnectSetTimestamp() override;
	virtual void OnDisconnectRemoveValue() override;
	virtual void OnDisconnectUpdateChildren(const TMap<FString, FFGValueVariant>& Update) override;
	virtual void CancelDisconnectOperations() override;

	virtual void BindTransactionTask(UFGTransactionTask* Task) override;
	virtual void RunTransaction() override;

private:
	FIRDatabaseReference* DbRef;
	FIRDatabaseQuery* DbQuery;

	FIRDatabaseHandle ValueChangedEventHandle;
	FIRDatabaseHandle ChildAddedEventHandle;
	FIRDatabaseHandle ChildChangedEventHandle;
	FIRDatabaseHandle ChildRemovedEventHandle;
	FIRDatabaseHandle ChildMovedEventHandle;

	UFGTransactionTask* TransactionTask;
};
