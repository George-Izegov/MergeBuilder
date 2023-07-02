// Copyright (c) 2022 Nineva Studios

#include "RemoteConfigLibraryIOS.h"

#include "FirebaseGoodiesLog.h"

#include "RemoteConfig/FGRemoteConfigParameterVariant.h"
#include "RemoteConfig/Callbacks/FGRemoteConfigCallback.h"

#import <FirebaseCore/FirebaseCore.h>
#import <FirebaseRemoteConfig/FirebaseRemoteConfig.h>

RemoteConfigLibraryIOS::~RemoteConfigLibraryIOS()
{
}

void RemoteConfigLibraryIOS::SetConfigSettings(int64 MinFetchInterval, int64 FetchTimeout)
{
	FIRRemoteConfig* remoteConfig = [FIRRemoteConfig remoteConfig];
	FIRRemoteConfigSettings* remoteConfigSettings = [[FIRRemoteConfigSettings alloc] init];
	remoteConfigSettings.minimumFetchInterval = 0;
	remoteConfig.configSettings = remoteConfigSettings;
}

void RemoteConfigLibraryIOS::Fetch(const FOnRemoteConfigSuccess& OnSuccess, const FOnRemoteConfigError& OnError)
{
	UFGRemoteConfigCallback* nativeCallback = NewObject<UFGRemoteConfigCallback>();
	nativeCallback->BindSuccessDelegate(OnSuccess);
	nativeCallback->BindErrorDelegate(OnError);

	FIRRemoteConfig* remoteConfig = [FIRRemoteConfig remoteConfig];

	[remoteConfig fetchWithCompletionHandler:^(FIRRemoteConfigFetchStatus status, NSError* error) {
		if (status == FIRRemoteConfigFetchStatusSuccess)
		{
			UE_LOG(LogFirebaseGoodies, Log, TEXT("Fetch success!"));
			nativeCallback->ExecuteSuccess();
		}
		else
		{
			FString ErrorStr(error.localizedDescription);
			UE_LOG(LogFirebaseGoodies, Error, TEXT("Fetch error: %s"), *ErrorStr);
			nativeCallback->ExecuteError(ErrorStr);
		}
	}];
}

void RemoteConfigLibraryIOS::FetchWithInterval(int64 TimeInterval, const FOnRemoteConfigSuccess& OnSuccess, const FOnRemoteConfigError& OnError)
{
	UFGRemoteConfigCallback* NativeCallback = NewObject<UFGRemoteConfigCallback>();
	NativeCallback->BindSuccessDelegate(OnSuccess);
	NativeCallback->BindErrorDelegate(OnError);
	
	FIRRemoteConfig* remoteConfig = [FIRRemoteConfig remoteConfig];

	[remoteConfig fetchWithExpirationDuration:TimeInterval completionHandler:^(FIRRemoteConfigFetchStatus status, NSError* error) {
		if (status == FIRRemoteConfigFetchStatusSuccess)
		{
			NSLog(@"FetchWithInterval success!");
			NativeCallback->ExecuteSuccess();
		}
		else
		{
			NSLog(@"FetchWithInterval error: %@", error.localizedDescription);
			FString errorStr(error.localizedDescription);
			NativeCallback->ExecuteError(errorStr);
		}
	}];
}

void RemoteConfigLibraryIOS::FetchAndActivate(const FOnRemoteConfigSuccess& OnSuccess, const FOnRemoteConfigError& OnError)
{
	UFGRemoteConfigCallback* NativeCallback = NewObject<UFGRemoteConfigCallback>();
	NativeCallback->BindSuccessDelegate(OnSuccess);
	NativeCallback->BindErrorDelegate(OnError);

	FIRRemoteConfig* remoteConfig = [FIRRemoteConfig remoteConfig];

	[remoteConfig fetchAndActivateWithCompletionHandler:^(FIRRemoteConfigFetchAndActivateStatus status, NSError* error) {
		if (status == FIRRemoteConfigFetchAndActivateStatusSuccessFetchedFromRemote || status == FIRRemoteConfigFetchAndActivateStatusSuccessUsingPreFetchedData)
		{
			NSLog(@"FetchAndActivate success!");
			NativeCallback->ExecuteSuccess();
		}
		else
		{
			NSLog(@"FetchAndActivate error: %@", error.localizedDescription);
			FString errorStr(error.localizedDescription);
			NativeCallback->ExecuteError(errorStr);
		}
	}];
}

void RemoteConfigLibraryIOS::Activate(const FOnRemoteConfigSuccess& OnSuccess, const FOnRemoteConfigError& OnError)
{
	UFGRemoteConfigCallback* NativeCallback = NewObject<UFGRemoteConfigCallback>();
	NativeCallback->BindSuccessDelegate(OnSuccess);
	NativeCallback->BindErrorDelegate(OnError);

	FIRRemoteConfig* remoteConfig = [FIRRemoteConfig remoteConfig];

	[remoteConfig activateWithCompletion:^(BOOL changed, NSError* error) {
		if (error == nil)
		{
			NSLog(@"Activate success!");
			NativeCallback->ExecuteSuccess();
		}
		else
		{
			NSLog(@"Activate error %@", error.localizedDescription);
			FString errorStr(error.localizedDescription);
			NativeCallback->ExecuteError(errorStr);
		}
	}];
}

bool RemoteConfigLibraryIOS::GetBoolean(FString Key)
{
	FIRRemoteConfig* remoteConfig = [FIRRemoteConfig remoteConfig];
	FIRRemoteConfigValue* val = [remoteConfig configValueForKey:Key.GetNSString()];
	return val.boolValue;
}

float RemoteConfigLibraryIOS::GetFloat(FString Key)
{
	FIRRemoteConfig* remoteConfig = [FIRRemoteConfig remoteConfig];
	FIRRemoteConfigValue* val = [remoteConfig configValueForKey:Key.GetNSString()];
	return (float) ([val.numberValue floatValue]);
}

int64 RemoteConfigLibraryIOS::GetLong(FString Key)
{
	FIRRemoteConfig* remoteConfig = [FIRRemoteConfig remoteConfig];
	FIRRemoteConfigValue* val = [remoteConfig configValueForKey:Key.GetNSString()];
	return (int64) ([val.numberValue longValue]);
}

FString RemoteConfigLibraryIOS::GetString(FString Key)
{
	FIRRemoteConfig* remoteConfig = [FIRRemoteConfig remoteConfig];
	FIRRemoteConfigValue* val = [remoteConfig configValueForKey:Key.GetNSString()];
	return FString(val.stringValue);
}

void RemoteConfigLibraryIOS::SetDefaults(const TMap<FString, UFGRemoteConfigParameterVariant*>& Values,
	const FOnRemoteConfigSuccess& OnSuccess, const FOnRemoteConfigError& OnError)
{
	UFGRemoteConfigCallback* NativeCallback = NewObject<UFGRemoteConfigCallback>();
	NativeCallback->BindSuccessDelegate(OnSuccess);
	NativeCallback->BindErrorDelegate(OnError);

	NSMutableDictionary<NSString*, NSObject*>* dict = [NSMutableDictionary dictionary];

	for (const auto& DataPair : Values)
	{
		NSString* Key = DataPair.Key.GetNSString();
		switch (DataPair.Value->Type)
		{
		case ERemoteConfigParameterValueType::Float:
			{
				float Value = DataPair.Value->GetValue<float>();
				dict[Key] = @(Value);
				break;
			}
		case ERemoteConfigParameterValueType::Bool:
			{
				bool Value = DataPair.Value->GetValue<bool>();
				dict[Key] = @(Value);
				break;
			}
		case ERemoteConfigParameterValueType::Long:
			{
				int64 Value = DataPair.Value->GetValue<int64>();
				dict[Key] = @(Value);
				break;
			}
		case ERemoteConfigParameterValueType::String:
			{
				NSString* Value = DataPair.Value->GetValue<FString>().GetNSString();
				dict[Key] = Value;
				break;
			}
		}
	}

	FIRRemoteConfig* remoteConfig = [FIRRemoteConfig remoteConfig];
	[remoteConfig setDefaults:dict];
	NativeCallback->ExecuteSuccess();
}
