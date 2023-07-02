// Copyright (c) 2022 Nineva Studios

#pragma once

#include "Engine/EngineTypes.h"
#include "DynamicLinks/FGAndroidDynamicLinkEntry.h"

#include "FirebaseGoodiesSettings.generated.h"

UCLASS(Config = Engine, DefaultConfig)
class FIREBASEGOODIES_API UFirebaseGoodiesSettings : public UObject
{
	GENERATED_BODY()

public:
	/** Path to google-services.json file required in order to properly configure Firebase for Android app */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Android", meta=(EditCondition = "!bIsGoogleServicesJsonInThePluginRootFolder", EditConditionHides))
	FString GoogleServicesJsonPath;

	/** Set to to 'true' if you decide to put your google-services.json file into your plugin root directory (Next to FirebaseGoodies_Android_UPL.xml in Plugins/FirebaseGoodies/Source/FirebaseGoodies). */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Meta = (DisplayName = "My google-services.json file is in my plugin root directory."), Category = "Android")
	bool bIsGoogleServicesJsonInThePluginRootFolder = false;
	
	/** Whether to generate and upload crashlytics debug symbols for debug Android builds. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Crashlytics|Android", Meta = (DisplayName = "Upload Symbols for Debug Build"))
	bool bUploadSymbolsDebugAndroid = false;
	
	/** Whether to generate and upload crashlytics debug symbols for distribution Android builds. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Crashlytics|Android", Meta = (DisplayName = "Upload Symbols for Distribution Build"))
	bool bUploadSymbolsReleaseAndroid = false;

	/** If you wish to temporarily disable Analytics collection, such as to get end-user consent before collecting data, you can set the value to false. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Analytics")
	bool bAnalyticsCollectionEnabled = true;

	/** If you need to deactivate Analytics collection permanently in a version of your app, set to true. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Analytics")
	bool bAnalyticsCollectionDeactivated = false;

	/** If you wish to disable collection of the Advertising ID in your Android app, you can set the value to false. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Analytics")
	bool bAdvertisingIdCollectionEnabled = true;

	/** To set the default personalized advertising behavior set the value to false. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Analytics")
	bool bAllowAdPersonalizationSignals = true;

	/** If you wish to disable collection of SSAID (Settings.Secure.ANDROID_ID) in your Android app, you can set the value to false. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Analytics|Android", Meta = (DisplayName = "SSAID collection enabled"))
	bool bSSAIDCollectionEnabled = true;

	/** If you wish to disable collection of the IDFV (Identifier for Vendor) in your iOS app, set the value to false. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Analytics|iOS", Meta = (DisplayName = "IDFV collection enabled"))
	bool bIDFVCollectionEnabled = true;

	/** Path to GoogleService-Info.plist file required in order to properly configure Firebase for iOS app */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "iOS")
	FFilePath GoogleServicesPlistPath;
	
	/** Whether to add the Apple Push Notifications capability to iOS builds. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Meta = (DisplayName = "Enable Apple Push Notifications for iOS"), Category = "iOS")
	bool bEnableAPNForIOS = false;

	/** Specifies if the database cache will be written to device storage or only persist in memory. By default it is only written to memory and is lost between application restarts */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Meta = (DisplayName = "Enable Data Persistence"), Category = "Realtime Database")
	bool bIsPersistenceEnabled = false;

	/** Change the cache size. Default is 10 MB. Cache sizes smaller than 1 MB or greater than 100 MB are not supported */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Meta = (DisplayName = "Cache Size (KB)", ClampMin = 1024, ClampMax = 102400), Category = "Realtime Database")
	int DatabaseCacheSize = 10240;

	/** Whether to use google-services.json for Editor and desktop platforms. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Desktop", Meta = (DisplayName = "Whether to use google-services.json for Editor. False will use the iOS .plist file."))
	bool bUseGoogleServicesJsonForDesktop = true;

	/** Whether to enable editor support. Requires plugin to be placed in the project directory. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Desktop", Meta = (DisplayName = "Enable editor support. Requires plugin to be placed in the project directory. Plugin needs to be rebuilt after changing this."))
	bool bEnableEditorSupport = false;

	/** Settings parsed from google-services.json */
	UPROPERTY(Config, VisibleAnywhere, AdvancedDisplay, BlueprintReadOnly, Meta = (DisplayName = "Client Id"), Category = "Android")
	FString AndroidOauthClientID;

	/** Settings parsed from GoogleService-Info.plist */
	UPROPERTY(Config, VisibleAnywhere, AdvancedDisplay, BlueprintReadOnly, Category = "iOS")
	FString ApiKey;

	/** Settings parsed from GoogleService-Info.plist */
	UPROPERTY(Config, VisibleAnywhere, AdvancedDisplay, BlueprintReadOnly, Category = "iOS")
	FString BundleId;

	/** Settings parsed from GoogleService-Info.plist */
	UPROPERTY(Config, VisibleAnywhere, AdvancedDisplay, BlueprintReadOnly, Category = "iOS")
	FString ClientId;

	/** Settings parsed from GoogleService-Info.plist */
	UPROPERTY(Config, VisibleAnywhere, AdvancedDisplay, BlueprintReadOnly, Category = "iOS")
	FString ReversedClientId;

	/** Settings parsed from GoogleService-Info.plist */
	UPROPERTY(Config, VisibleAnywhere, AdvancedDisplay, BlueprintReadOnly, Category = "iOS")
	FString GcmSenderId;

	/** Settings parsed from GoogleService-Info.plist */
	UPROPERTY(Config, VisibleAnywhere, AdvancedDisplay, BlueprintReadOnly, Category = "iOS")
	FString ProjectId;

	/** Settings parsed from GoogleService-Info.plist */
	UPROPERTY(Config, VisibleAnywhere, AdvancedDisplay, BlueprintReadOnly, Category = "iOS")
	FString AndroidClientId;

	/** Settings parsed from GoogleService-Info.plist */
	UPROPERTY(Config, VisibleAnywhere, AdvancedDisplay, BlueprintReadOnly, Category = "iOS")
	FString GoogleAppId;

	/** Settings parsed from GoogleService-Info.plist */
	UPROPERTY(Config, VisibleAnywhere, AdvancedDisplay, BlueprintReadOnly, Category = "iOS")
	FString DatabaseUrl;

	/** Settings parsed from GoogleService-Info. plist */
	UPROPERTY(Config, VisibleAnywhere, AdvancedDisplay, BlueprintReadOnly, Category = "iOS")
	FString StorageBucket;

	/* Intent filters to be added to Android Manifest. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Dynamic Links|Android")
	TArray<FFGAndroidDynamicLinkEntry> AndroidDynamicLinks;

	/* Associated Domains to be added to entitlements. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Dynamic Links|iOS")
	TArray<FString> AssociatedDomains;

	/* Whether to enable Dynamic Links support for iOS. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Dynamic Links|iOS")
	bool EnableDynamicLinksForIos = false;

	/* Custom URL Schemes to be added to Info.plist. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Dynamic Links|iOS")
	TArray<FString> URLSchemes;

	UPROPERTY(Config, BlueprintReadOnly, AdvancedDisplay, Category = "Dynamic Links|iOS")
	FString URLSchemesString = "";

	/* Custom Domains to be added to Info.plist for Firebase to process Universal Links. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Dynamic Links|iOS")
	TArray<FString> FirebaseDynamicLinksCustomDomains;

	UPROPERTY(Config, BlueprintReadOnly, AdvancedDisplay, Category = "Dynamic Links|iOS")
	FString FirebaseDynamicLinksCustomDomainsString = "";

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	void ParseJson();
	void ParsePlist();
	void HandleAPNState();
	void ParseAndSaveXmlAndroid();
	void AddApplicationTags(const FString& TagsString);
	void ParseAndSaveUrlSchemesIos();
	void ParseAndSaveFirebaseDynamicLinksCustomDomains();
#endif
};
