// Copyright 2015-2022 MY.GAMES. All Rights Reserved.

#pragma once

#include "PsIronSourceSettings.generated.h"

UCLASS(config = Engine, defaultconfig)
class UPsIronSourceSettings : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	/** Whether IronSource is enabled */
	UPROPERTY(Config, EditAnywhere, Category="IronSource")
	bool bIronSourceEnable;

	/** IronSource iOS app Key */
	UPROPERTY(Config, EditAnywhere, Category="IronSource")
	FString IronSourceIOSAppKey;

	/** AdMob app id (iOS) */
	UPROPERTY(Config, EditAnywhere, Category="IronSource")
	FString AdMobIOSAppId;

	/** IronSource Android app Key */
	UPROPERTY(Config, EditAnywhere, Category="IronSource")
	FString IronSourceAndroidAppKey;

	/** AdMob app id (Android) */
	UPROPERTY(Config, EditAnywhere, Category="IronSource")
	FString AdMobAndroidAppId;
};
