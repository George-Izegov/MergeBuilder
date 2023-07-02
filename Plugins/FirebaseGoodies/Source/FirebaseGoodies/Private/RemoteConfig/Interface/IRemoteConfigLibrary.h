// Copyright (c) 2022 Nineva Studios

#pragma once

#include "RemoteConfig/FGRemoteConfigLibrary.h"

class IRemoteConfigLibrary
{
public:
	virtual ~IRemoteConfigLibrary() = default;

	virtual void SetConfigSettings(int64 MinFetchInterval, int64 FetchTimeout) = 0;
	virtual void Fetch(const FOnRemoteConfigSuccess& OnSuccess, const FOnRemoteConfigError& OnError) = 0;
	virtual void FetchWithInterval(int64 TimeInterval, const FOnRemoteConfigSuccess& OnSuccess, const FOnRemoteConfigError& OnError) = 0;
	virtual void FetchAndActivate(const FOnRemoteConfigSuccess& OnSuccess, const FOnRemoteConfigError& OnError) = 0;
	virtual void Activate(const FOnRemoteConfigSuccess& OnSuccess, const FOnRemoteConfigError& OnError) = 0;

	virtual bool GetBoolean(FString Key) = 0;
	virtual float GetFloat(FString Key) = 0;
	virtual int64 GetLong(FString Key) = 0;
	virtual FString GetString(FString Key) = 0;

	virtual void SetDefaults(const TMap<FString, UFGRemoteConfigParameterVariant*>& Values,
		const FOnRemoteConfigSuccess& OnSuccess, const FOnRemoteConfigError& OnError) = 0;
};
