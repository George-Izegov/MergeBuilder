// Copyright (c) 2022 Nineva Studios

#include "FGDatabaseRefCallback.h"

#include "Database/FGDataSnapshot.h"

UFGDatabaseRefCallback::UFGDatabaseRefCallback()
{
	if (StaticClass()->GetDefaultObject() != this)
	{
		AddToRoot();
	}
}

void UFGDatabaseRefCallback::BindDataChangedDelegate(const FOnDataChangedDelegate& OnDataChanged)
{
	OnDataChangedDelegate = OnDataChanged;
}

void UFGDatabaseRefCallback::BindCancelDelegate(const FOnCancelledDelegate& OnCancelled)
{
	OnCancelledDelegate = OnCancelled;
}

void UFGDatabaseRefCallback::BindOnChildEventDelegate(const FOnChildEventDelegate& OnChildEvent)
{
	OnChildEventDelegate = OnChildEvent;
}

void UFGDatabaseRefCallback::ExecuteDataChanged(UFGDataSnapshot* Data)
{
	AsyncTask(ENamedThreads::GameThread, [=]() {
		OnDataChangedDelegate.ExecuteIfBound(Data);
	});
	this->RemoveFromRoot();
}

void UFGDatabaseRefCallback::ExecuteCancel(int ErrorCode, const FString& ErrorMessage)
{
	AsyncTask(ENamedThreads::GameThread, [=]() {
		OnCancelledDelegate.ExecuteIfBound(ErrorCode, ErrorMessage);
	});
	this->RemoveFromRoot();
}

void UFGDatabaseRefCallback::ExecuteOnChildEvent(EChileEventType EventType, UFGDataSnapshot* Data, const FString& PreviousChildName)
{
	AsyncTask(ENamedThreads::GameThread, [=]() {
		OnChildEventDelegate.ExecuteIfBound(EventType, Data, PreviousChildName);
	});
	this->RemoveFromRoot();
}
