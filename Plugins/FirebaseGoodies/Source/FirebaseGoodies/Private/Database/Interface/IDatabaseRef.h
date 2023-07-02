// Copyright (c) 2022 Nineva Studios

#pragma once

#include "Database/FGDatabaseRef.h"

class IDatabaseRef
{
public:
	virtual ~IDatabaseRef() = default;

	virtual void Init(const FString& DatabaseUrl, const FString& InitialPath) = 0;

	virtual void SetValue(const FFGValueVariant& Value, const FFGValueVariant& Priority) = 0;
	virtual void SetTimestamp() = 0;
	virtual void SetPriority(const FFGValueVariant& Priority) = 0;

	virtual FString GetKey() const = 0;
	virtual void GetValue(const FOnDataChangedDelegate& OnDataReceived, const FOnCancelledDelegate& OnCancelled) = 0;
	virtual void RemoveValue() = 0;

	virtual FString Push() = 0;

	virtual void UpdateChildren(const TMap<FString, FFGValueVariant>& Update) = 0;

	virtual void AddValueListener(const FOnDataChangedDelegate& OnDataChanged, const FOnCancelledDelegate& OnCancelled) = 0;
	virtual void RemoveValueListener() = 0;
	virtual void AddChildListener(const FOnChildEventDelegate& OnChildEvent, const FOnCancelledDelegate& OnCancelled) = 0;
	virtual void RemoveChildListener() = 0;

	virtual void KeepSynced(bool Sync) = 0;

	virtual void OrderByChild(const FString& Path) = 0;
	virtual void OrderByKey() = 0;
	virtual void OrderByPriority() = 0;
	virtual void OrderByValue() = 0;
	virtual void EndAtFloat(float Value, const FString& Key) = 0;
	virtual void EndAtString(const FString& Value, const FString& Key) = 0;
	virtual void StartAtFloat(float Value, const FString& Key) = 0;
	virtual void StartAtString(const FString& Value, const FString& Key) = 0;
	virtual void EqualToFloat(float Value, const FString& Key) = 0;
	virtual void EqualToBool(bool Value, const FString& Key) = 0;
	virtual void EqualToString(const FString& Value, const FString& Key) = 0;
	virtual void LimitToFirst(int Limit) = 0;
	virtual void LimitToLast(int Limit) = 0;

	virtual void OnDisconnectSetValue(const FFGValueVariant& Value) = 0;
	virtual void OnDisconnectSetTimestamp() = 0;
	virtual void OnDisconnectRemoveValue() = 0;
	virtual void OnDisconnectUpdateChildren(const TMap<FString, FFGValueVariant>& Update) = 0;
	virtual void CancelDisconnectOperations() = 0;

	virtual void BindTransactionTask(UFGTransactionTask* Task) = 0;
	virtual void RunTransaction() = 0;
};