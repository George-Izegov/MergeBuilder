// Copyright (c) 2022 Nineva Studios

#pragma once

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT

#include "RemoteConfig/Interface/IRemoteConfigLibrary.h"

class RemoteConfigLibraryDesktop : public IRemoteConfigLibrary
{
public:
	virtual ~RemoteConfigLibraryDesktop() override;

	virtual void SetConfigSettings(int64 MinFetchInterval, int64 FetchTimeout) override;
	virtual void Fetch(const FOnRemoteConfigSuccess& OnSuccess, const FOnRemoteConfigError& OnError) override;
	virtual void FetchWithInterval(int64 TimeInterval, const FOnRemoteConfigSuccess& OnSuccess, const FOnRemoteConfigError& OnError) override;
	virtual void FetchAndActivate(const FOnRemoteConfigSuccess& OnSuccess, const FOnRemoteConfigError& OnError) override;
	virtual void Activate(const FOnRemoteConfigSuccess& OnSuccess, const FOnRemoteConfigError& OnError) override;

	virtual bool GetBoolean(FString Key) override;
	virtual float GetFloat(FString Key) override;
	virtual int64 GetLong(FString Key) override;
	virtual FString GetString(FString Key) override;

	virtual void SetDefaults(const TMap<FString, UFGRemoteConfigParameterVariant*>& Values,
		const FOnRemoteConfigSuccess& OnSuccess, const FOnRemoteConfigError& OnError) override;
};

#endif