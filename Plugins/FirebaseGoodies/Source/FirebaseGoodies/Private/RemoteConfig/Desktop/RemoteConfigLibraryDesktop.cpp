// Copyright (c) 2022 Nineva Studios

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT

#include "RemoteConfigLibraryDesktop.h"

#include "FirebaseGoodiesLog.h"

#include "RemoteConfig/FGRemoteConfigParameterVariant.h"
#include "RemoteConfig/Callbacks/FGRemoteConfigCallback.h"

#include "Desktop/FGUtils.h"

#include "firebase/remote_config.h"

RemoteConfigLibraryDesktop::~RemoteConfigLibraryDesktop()
{
}

void RemoteConfigLibraryDesktop::SetConfigSettings(int64 MinFetchInterval, int64 FetchTimeout)
{
	firebase::App* FirebaseApp = firebase::App::GetInstance();

	firebase::remote_config::RemoteConfig* RemoteConfig = firebase::remote_config::RemoteConfig::GetInstance(FirebaseApp);

	firebase::remote_config::ConfigSettings RemoteConfigSettings;
	RemoteConfigSettings.minimum_fetch_interval_in_milliseconds = MinFetchInterval;
	RemoteConfigSettings.fetch_timeout_in_milliseconds = FetchTimeout;

	RemoteConfig->SetConfigSettings(RemoteConfigSettings);
}

void RemoteConfigLibraryDesktop::Fetch(const FOnRemoteConfigSuccess& OnSuccess, const FOnRemoteConfigError& OnError)
{
	UFGRemoteConfigCallback* nativeCallback = NewObject<UFGRemoteConfigCallback>();
	nativeCallback->BindSuccessDelegate(OnSuccess);
	nativeCallback->BindErrorDelegate(OnError);

	firebase::App* FirebaseApp = firebase::App::GetInstance();

	firebase::remote_config::RemoteConfig* RemoteConfig = firebase::remote_config::RemoteConfig::GetInstance(FirebaseApp);

	const auto FetchResultFuture = RemoteConfig->Fetch();

	FetchResultFuture.OnCompletion([=](const firebase::Future<void>&)
	{
		if (FetchResultFuture.status() == firebase::kFutureStatusInvalid)
		{
			UE_LOG(LogFirebaseGoodies, Error, TEXT("Fetch error"));
			nativeCallback->ExecuteError(TEXT("Fetch error"));
			return;
		}

		UE_LOG(LogFirebaseGoodies, Log, TEXT("Fetch success!"));
		nativeCallback->ExecuteSuccess();
	});
}

void RemoteConfigLibraryDesktop::FetchWithInterval(int64 TimeInterval, const FOnRemoteConfigSuccess& OnSuccess, const FOnRemoteConfigError& OnError)
{
	UFGRemoteConfigCallback* NativeCallback = NewObject<UFGRemoteConfigCallback>();
	NativeCallback->BindSuccessDelegate(OnSuccess);
	NativeCallback->BindErrorDelegate(OnError);

	firebase::App* FirebaseApp = firebase::App::GetInstance();

	firebase::remote_config::RemoteConfig* RemoteConfig = firebase::remote_config::RemoteConfig::GetInstance(FirebaseApp);

	const auto FetchResultFuture = RemoteConfig->Fetch(TimeInterval);

	FetchResultFuture.OnCompletion([=](const firebase::Future<void>&)
	{
		if (FetchResultFuture.status() == firebase::kFutureStatusInvalid)
		{
			UE_LOG(LogFirebaseGoodies, Error, TEXT("FetchWithInterval error"));
			NativeCallback->ExecuteError(TEXT("FetchWithInterval error"));
			return;
		}

		UE_LOG(LogFirebaseGoodies, Log, TEXT("FetchWithInterval success!"));
		NativeCallback->ExecuteSuccess();
	});
}

void RemoteConfigLibraryDesktop::FetchAndActivate(const FOnRemoteConfigSuccess& OnSuccess, const FOnRemoteConfigError& OnError)
{
	UFGRemoteConfigCallback* NativeCallback = NewObject<UFGRemoteConfigCallback>();
	NativeCallback->BindSuccessDelegate(OnSuccess);
	NativeCallback->BindErrorDelegate(OnError);

	firebase::App* FirebaseApp = firebase::App::GetInstance();

	firebase::remote_config::RemoteConfig* RemoteConfig = firebase::remote_config::RemoteConfig::GetInstance(FirebaseApp);

	const auto FetchResultFuture = RemoteConfig->FetchAndActivate();

	FetchResultFuture.OnCompletion([=](const firebase::Future<bool>&)
	{
		if (FetchResultFuture.status() == firebase::kFutureStatusInvalid)
		{
			UE_LOG(LogFirebaseGoodies, Error, TEXT("FetchAndActivate error"));
			NativeCallback->ExecuteError(TEXT("FetchAndActivate error"));
			return;
		}

		UE_LOG(LogFirebaseGoodies, Log, TEXT("FetchAndActivate success!"));
		NativeCallback->ExecuteSuccess();
	});
}

void RemoteConfigLibraryDesktop::Activate(const FOnRemoteConfigSuccess& OnSuccess, const FOnRemoteConfigError& OnError)
{
	UFGRemoteConfigCallback* NativeCallback = NewObject<UFGRemoteConfigCallback>();
	NativeCallback->BindSuccessDelegate(OnSuccess);
	NativeCallback->BindErrorDelegate(OnError);

	firebase::App* FirebaseApp = firebase::App::GetInstance();

	firebase::remote_config::RemoteConfig* RemoteConfig = firebase::remote_config::RemoteConfig::GetInstance(FirebaseApp);

	const auto ActivateResultFuture = RemoteConfig->Activate();

	ActivateResultFuture.OnCompletion([=](const firebase::Future<bool>&)
	{
		if (ActivateResultFuture.status() == firebase::kFutureStatusInvalid)
		{
			UE_LOG(LogFirebaseGoodies, Error, TEXT("Activate error"));
			NativeCallback->ExecuteError(TEXT("Activate error"));
			return;
		}

		UE_LOG(LogFirebaseGoodies, Log, TEXT("Activate success!"));
		NativeCallback->ExecuteSuccess();
	});
}

bool RemoteConfigLibraryDesktop::GetBoolean(FString Key)
{
	bool value = false;

	firebase::App* firebaseApp = firebase::App::GetInstance();

	firebase::remote_config::RemoteConfig* remoteConfig = firebase::remote_config::RemoteConfig::GetInstance(firebaseApp);

	value = remoteConfig->GetBoolean(TCHAR_TO_ANSI(*Key));

	return value;
}

float RemoteConfigLibraryDesktop::GetFloat(FString Key)
{
	float Value = 0.0f;

	firebase::App* FirebaseApp = firebase::App::GetInstance();

	firebase::remote_config::RemoteConfig* RemoteConfig = firebase::remote_config::RemoteConfig::GetInstance(FirebaseApp);

	Value = static_cast<float>(RemoteConfig->GetDouble(TCHAR_TO_ANSI(*Key)));

	return Value;
}

int64 RemoteConfigLibraryDesktop::GetLong(FString Key)
{
	int64 Value = int64();

	firebase::App* FirebaseApp = firebase::App::GetInstance();

	firebase::remote_config::RemoteConfig* RemoteConfig = firebase::remote_config::RemoteConfig::GetInstance(FirebaseApp);

	Value = RemoteConfig->GetLong(TCHAR_TO_ANSI(*Key));

	return Value;
}

FString RemoteConfigLibraryDesktop::GetString(FString Key)
{
	FString Value;

	firebase::App* FirebaseApp = firebase::App::GetInstance();

	firebase::remote_config::RemoteConfig* RemoteConfig = firebase::remote_config::RemoteConfig::GetInstance(FirebaseApp);

	Value = UTF8_TO_TCHAR(RemoteConfig->GetString(TCHAR_TO_ANSI(*Key)).c_str());

	return Value;
}

void RemoteConfigLibraryDesktop::SetDefaults(const TMap<FString, UFGRemoteConfigParameterVariant*>& Values,
                                             const FOnRemoteConfigSuccess& OnSuccess, const FOnRemoteConfigError& OnError)
{
	UFGRemoteConfigCallback* NativeCallback = NewObject<UFGRemoteConfigCallback>();
	NativeCallback->BindSuccessDelegate(OnSuccess);
	NativeCallback->BindErrorDelegate(OnError);

	firebase::App* FirebaseApp = firebase::App::GetInstance();

	firebase::remote_config::RemoteConfig* RemoteConfig = firebase::remote_config::RemoteConfig::GetInstance(FirebaseApp);

	TArray<firebase::remote_config::ConfigKeyValueVariant> Defaults;

	for (const auto& DataPair : Values)
	{
		firebase::Variant Value;

		switch (DataPair.Value->Type)
		{
		case ERemoteConfigParameterValueType::Float:
			{
				Value = DataPair.Value->GetValue<float>();
				break;
			}
		case ERemoteConfigParameterValueType::Bool:
			{
				Value = DataPair.Value->GetValue<bool>();
				break;
			}
		case ERemoteConfigParameterValueType::Long:
			{
				Value = DataPair.Value->GetValue<int64>();
				break;
			}
		case ERemoteConfigParameterValueType::String:
			{
				Value = FGUtils::GetStringCopy(DataPair.Value->GetValue<FString>());
				break;
			}
		}

		Defaults.Add({FGUtils::GetStringCopy(DataPair.Key), Value});
	}

	const auto SetDefaultsResultFuture = RemoteConfig->SetDefaults(Defaults.GetData(), Defaults.Num());

	SetDefaultsResultFuture.OnCompletion([=](const firebase::Future<void>&)
	{
		if (SetDefaultsResultFuture.status() == firebase::kFutureStatusInvalid)
		{
			AsyncTask(ENamedThreads::GameThread, [=]()
			{
				UE_LOG(LogFirebaseGoodies, Error, TEXT("SetDefaults error"));
				NativeCallback->ExecuteError(TEXT("Activate error"));
			});
			return;
		}

		AsyncTask(ENamedThreads::GameThread, [=]()
		{
			UE_LOG(LogFirebaseGoodies, Log, TEXT("SetDefaults success!"));
			NativeCallback->ExecuteSuccess();
		});

		FGUtils::ClearStringCopies();
	});
}

#endif
