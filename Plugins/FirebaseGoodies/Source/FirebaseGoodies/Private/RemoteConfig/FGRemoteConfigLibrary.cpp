// Copyright (c) 2022 Nineva Studios

#include "RemoteConfig/FGRemoteConfigLibrary.h"
#include "RemoteConfig/FGRemoteConfigParameterVariant.h"

#include "Interface/IRemoteConfigLibrary.h"

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
#include "Desktop/RemoteConfigLibraryDesktop.h"
#endif

#if PLATFORM_ANDROID
#include "Android/RemoteConfigLibraryAndroid.h"
#endif

#if PLATFORM_IOS
#include "IOS/RemoteConfigLibraryIOS.h"
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && !FG_ENABLE_EDITOR_SUPPORT
TSharedPtr<IRemoteConfigLibrary> UFGRemoteConfigLibrary::RemoteConfigLibraryImpl = nullptr;
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
TSharedPtr<IRemoteConfigLibrary> UFGRemoteConfigLibrary::RemoteConfigLibraryImpl = MakeShareable(new RemoteConfigLibraryDesktop());;
#endif

#if PLATFORM_ANDROID
TSharedPtr<IRemoteConfigLibrary> UFGRemoteConfigLibrary::RemoteConfigLibraryImpl = MakeShareable(new RemoteConfigLibraryAndroid());;
#endif

#if PLATFORM_IOS
TSharedPtr<IRemoteConfigLibrary> UFGRemoteConfigLibrary::RemoteConfigLibraryImpl = MakeShareable(new RemoteConfigLibraryIOS());;
#endif

void UFGRemoteConfigLibrary::SetConfigSettings(int64 MinFetchInterval, int64 FetchTimeout)
{
	if(!RemoteConfigLibraryImpl)
		return;

	RemoteConfigLibraryImpl->SetConfigSettings(MinFetchInterval, FetchTimeout);
}

void UFGRemoteConfigLibrary::Fetch(const FOnRemoteConfigSuccess& OnSuccess, const FOnRemoteConfigError& OnError)
{
	if(!RemoteConfigLibraryImpl)
		return;

	RemoteConfigLibraryImpl->Fetch(OnSuccess, OnError);
}

void UFGRemoteConfigLibrary::FetchWithInterval(int64 TimeInterval, const FOnRemoteConfigSuccess& OnSuccess, const FOnRemoteConfigError& OnError)
{
	if(!RemoteConfigLibraryImpl)
		return;

	RemoteConfigLibraryImpl->FetchWithInterval(TimeInterval, OnSuccess, OnError);
}

void UFGRemoteConfigLibrary::FetchAndActivate(const FOnRemoteConfigSuccess& OnSuccess, const FOnRemoteConfigError& OnError)
{
	if(!RemoteConfigLibraryImpl)
		return;

	RemoteConfigLibraryImpl->FetchAndActivate(OnSuccess, OnError);
}

void UFGRemoteConfigLibrary::Activate(const FOnRemoteConfigSuccess& OnSuccess, const FOnRemoteConfigError& OnError)
{
	if(!RemoteConfigLibraryImpl)
		return;

	RemoteConfigLibraryImpl->Activate(OnSuccess, OnError);
}

bool UFGRemoteConfigLibrary::GetBoolean(FString Key)
{
	if(!RemoteConfigLibraryImpl)
		return false;

	return RemoteConfigLibraryImpl->GetBoolean(Key);
}

float UFGRemoteConfigLibrary::GetFloat(FString Key)
{
	if(!RemoteConfigLibraryImpl)
		return 0.0f;

	return RemoteConfigLibraryImpl->GetFloat(Key);
}

int64 UFGRemoteConfigLibrary::GetLong(FString Key)
{
	if(!RemoteConfigLibraryImpl)
		return int64();

	return RemoteConfigLibraryImpl->GetLong(Key);
}

FString UFGRemoteConfigLibrary::GetString(FString Key)
{
	if(!RemoteConfigLibraryImpl)
		return FString();

	return RemoteConfigLibraryImpl->GetString(Key);
}

void UFGRemoteConfigLibrary::SetDefaults(const TMap<FString, UFGRemoteConfigParameterVariant*>& Values,
	const FOnRemoteConfigSuccess& OnSuccess, const FOnRemoteConfigError& OnError)
{
	if(!RemoteConfigLibraryImpl)
		return;

	RemoteConfigLibraryImpl->SetDefaults(Values, OnSuccess, OnError);
}
