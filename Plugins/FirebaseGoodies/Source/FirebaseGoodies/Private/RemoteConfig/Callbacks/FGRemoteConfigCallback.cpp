// Copyright (c) 2022 Nineva Studios

#include "FGRemoteConfigCallback.h"

#include "Async/Async.h"

UFGRemoteConfigCallback::UFGRemoteConfigCallback()
{
	if (StaticClass()->GetDefaultObject() != this)
	{
		AddToRoot();
	}
}

void UFGRemoteConfigCallback::BindSuccessDelegate(const FOnRemoteConfigSuccess& OnSuccess)
{
	OnConfigSuccessDelegate = OnSuccess;
}

void UFGRemoteConfigCallback::BindErrorDelegate(const FOnRemoteConfigError& OnError)
{
	OnConfigFetchErrorDelegate = OnError;
}

void UFGRemoteConfigCallback::ExecuteSuccess()
{
	AsyncTask(ENamedThreads::GameThread, [=]() {
		OnConfigSuccessDelegate.ExecuteIfBound();
	});
	this->RemoveFromRoot();
}

void UFGRemoteConfigCallback::ExecuteError(FString ErrorMessage)
{
	AsyncTask(ENamedThreads::GameThread, [=]() {
		OnConfigFetchErrorDelegate.ExecuteIfBound(ErrorMessage);
	});
	this->RemoveFromRoot();
}
