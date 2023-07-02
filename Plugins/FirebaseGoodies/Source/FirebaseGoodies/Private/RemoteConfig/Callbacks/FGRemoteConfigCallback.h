// Copyright (c) 2022 Nineva Studios

#pragma once

#include "RemoteConfig/FGRemoteConfigLibrary.h"

#include "FGRemoteConfigCallback.generated.h"

UCLASS()
class FIREBASEGOODIES_API UFGRemoteConfigCallback : public UObject
{
	GENERATED_BODY()

public:
	UFGRemoteConfigCallback();

	void BindSuccessDelegate(const FOnRemoteConfigSuccess& OnSuccess);
	void BindErrorDelegate(const FOnRemoteConfigError& OnError);

	void ExecuteSuccess();
	void ExecuteError(FString ErrorMessage);

private:
	UPROPERTY()
	FOnRemoteConfigSuccess OnConfigSuccessDelegate;
	UPROPERTY()
	FOnRemoteConfigError OnConfigFetchErrorDelegate;
};
