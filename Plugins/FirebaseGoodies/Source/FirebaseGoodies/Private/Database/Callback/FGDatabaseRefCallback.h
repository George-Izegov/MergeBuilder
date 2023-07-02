// Copyright (c) 2022 Nineva Studios

#pragma once

#include "Database/FGDatabaseRef.h"

#include "FGDatabaseRefCallback.generated.h"

class UFGDataSnapshot;

UCLASS()
class UFGDatabaseRefCallback : public UObject
{
	GENERATED_BODY()

public:
	UFGDatabaseRefCallback();

	void BindDataChangedDelegate(const FOnDataChangedDelegate& OnDataChanged);
	void BindCancelDelegate(const FOnCancelledDelegate& OnCancelled);
	void BindOnChildEventDelegate(const FOnChildEventDelegate& OnChildEvent);

	void ExecuteDataChanged(UFGDataSnapshot* Data);
	void ExecuteCancel(int ErrorCode, const FString& ErrorMessage);
	void ExecuteOnChildEvent(EChileEventType EventType, UFGDataSnapshot* Data, const FString& PreviousChildName);

private:
	UPROPERTY()
	FOnDataChangedDelegate OnDataChangedDelegate;
	UPROPERTY()
	FOnCancelledDelegate OnCancelledDelegate;
	UPROPERTY()
	FOnChildEventDelegate OnChildEventDelegate;
};
