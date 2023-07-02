// Copyright (c) 2022 Nineva Studios

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "FGRemoteConfigLibrary.generated.h"

class IRemoteConfigLibrary;
class UFGRemoteConfigParameterVariant;

DECLARE_DYNAMIC_DELEGATE(FOnRemoteConfigSuccess);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnRemoteConfigError, FString, ErrorMessage);

UCLASS()
class FIREBASEGOODIES_API UFGRemoteConfigLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Set default settings for Firebase remote config.
	 *
	 * @param MinFetchInterval - minimum interval between successive remote config fetches calls in seconds.
	 * @param FetchTimeout - remote config fetch timeout in seconds.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Remote Config")
	static void SetConfigSettings(int64 MinFetchInterval, int64 FetchTimeout);

	/**
	 * Fetch configs, adhering to the default minimum fetch interval.
	 *
	 * @param OnSuccess - callback triggered after remote config values were fetched successfully.
	 * @param OnError - callback triggered when error occured during remote config values fetching.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Remote Config", meta = (AutoCreateRefTerm = "OnSuccess, OnError"))
	static void Fetch(const FOnRemoteConfigSuccess& OnSuccess, const FOnRemoteConfigError& OnError);

	/**
	 * Fetch configs, adhering to the specified minimum fetch interval.
	 *
	 * @param OnSuccess - callback triggered after remote config values were fetched successfully.
	 * @param OnError - callback triggered when error occured during remote config values fetching.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Remote Config", meta = (AutoCreateRefTerm = "OnSuccess, OnError"))
	static void FetchWithInterval(int64 TimeInterval, const FOnRemoteConfigSuccess& OnSuccess, const FOnRemoteConfigError& OnError);

	/**
	 * Fetch configs and activate them after receiving new values.
	 *
	 * @param OnSuccess - callback triggered after remote config values were fetched and activated successfully.
	 * @param OnError - callback triggered when error occured during remote config values fetching or activation.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Remote Config", meta = (AutoCreateRefTerm = "OnSuccess, OnError"))
	static void FetchAndActivate(const FOnRemoteConfigSuccess& OnSuccess, const FOnRemoteConfigError& OnError);

	/**
	 * Activate the most recently fetched configs, so that the fetched key value pairs take effect.
	 *
	 * @param OnSuccess - callback triggered after remote config values were activated successfully.
	 * @param OnError - callback triggered when error occured during remote config values activation.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Remote Config", meta = (AutoCreateRefTerm = "OnSuccess, OnError"))
	static void Activate(const FOnRemoteConfigSuccess& OnSuccess, const FOnRemoteConfigError& OnError);

	/**
	 * Get boolean remote config value with given key.
	 *
	 * @param Key - key of boolean config value to be read.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Remote Config")
	static bool GetBoolean(FString Key);

	/**
	 * Get float remote config value with given key.
	 *
	 * @param Key - key of float config value to be read.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Remote Config")
	static float GetFloat(FString Key);

	/**
	 * Get long remote config value with given key.
	 *
	 * @param Key - key of long config value to be read.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Remote Config")
	static int64 GetLong(FString Key);

	/**
	 * Get string remote config value with given key.
	 *
	 * @param Key - key of string config value to be read.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Remote Config")
	static FString GetString(FString Key);

	/**
	 * Get string remote config value with given key.
	 *
	 * @param Values - map of default remote config values.
	 * @param OnSuccess - callback triggered after default remote config values were set successfully.
	 * @param OnError - callback triggered when error occured during setting default remote config values.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Remote Config")
	static void SetDefaults(const TMap<FString, UFGRemoteConfigParameterVariant*>& Values,
		const FOnRemoteConfigSuccess& OnSuccess, const FOnRemoteConfigError& OnError);

private:
	static TSharedPtr<IRemoteConfigLibrary> RemoteConfigLibraryImpl;
};
