// Copyright (c) 2022 Nineva Studios

#include "FirebaseGoodies.h"

#include "Developer/Settings/Public/ISettingsModule.h"
#include "Engine/Engine.h"
#include "FirebaseGoodiesLog.h"
#include "FirebaseGoodiesSettings.h"
#include "CloudMessaging/FGCloudMessaging.h"
#include "CloudMessaging/FGRemoteMessage.h"
#include "Utils/FirebaseGoodiesPluginUtils.h"
#include "Async/Async.h"

#if PLATFORM_IOS
#include "IOSAppDelegate.h"
#include "IOS/CloudMessaging/FGCloudMessagingDelegate.h"
#include "Misc/CoreDelegates.h"
#include "IOS/DynamicLinksHelper.h"

#import <FirebaseCore/FirebaseCore.h>
#import <FirebaseMessaging/FirebaseMessaging.h>
#import <GoogleSignIn/GoogleSignIn.h>
#if DYNAMIC_LINKS_IOS
#import "FirebaseDynamicLinks/FirebaseDynamicLinks.h"
#endif
#endif

#define LOCTEXT_NAMESPACE "FFirebaseGoodiesModule"

#if PLATFORM_IOS
FGCloudMessagingDelegate* MessagingDelegate;
#endif

void FFirebaseGoodiesModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FirebaseGoodiesSettings = NewObject<UFirebaseGoodiesSettings>(GetTransientPackage(), "FirebaseGoodiesSettings", RF_Standalone);
	FirebaseGoodiesSettings->AddToRoot();

	// Register settings
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "FirebaseGoodies",
			LOCTEXT("RuntimeSettingsName", "Firebase Goodies"),
			LOCTEXT("RuntimeSettingsDescription", "Configure Firebase Goodies"),
			FirebaseGoodiesSettings);
	}

	const FString PluginName = "FirebaseGoodies";
	const FString PluginVersionName = UFirebaseGoodiesPluginUtils::GetPluginVersionName(PluginName);
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("[%s] - Version: %s"), *PluginName, *PluginVersionName);
    
#if WITH_EDITOR
	FirebaseGoodiesSettings->HandleAPNState();
#endif
    
#if PLATFORM_IOS
	__block FIROptions* options = [[FIROptions alloc] initWithGoogleAppID:FirebaseGoodiesSettings->GoogleAppId.GetNSString() GCMSenderID:FirebaseGoodiesSettings->GcmSenderId.GetNSString()];
	options.APIKey = FirebaseGoodiesSettings->ApiKey.GetNSString();
	options.bundleID = FirebaseGoodiesSettings->BundleId.GetNSString();
	options.clientID = FirebaseGoodiesSettings->ClientId.GetNSString();
	options.projectID = FirebaseGoodiesSettings->ProjectId.GetNSString();
	options.androidClientID = FirebaseGoodiesSettings->AndroidClientId.GetNSString();
	options.storageBucket = FirebaseGoodiesSettings->StorageBucket.GetNSString();
	
	NSString* DatabaseUrl = FirebaseGoodiesSettings->DatabaseUrl.GetNSString();
	if ([DatabaseUrl length] != 0)
	{
		options.databaseURL = DatabaseUrl;
	}
	
	dispatch_async(dispatch_get_main_queue(), ^{
		[FIRApp configureWithOptions:options];
		
		if (DynamicLinksHelper::WasAppOpenViaAppLink) {
			UE_LOG(LogFirebaseGoodies, Verbose, TEXT("Application was open from Universal link."));
			[[FIRDynamicLinks dynamicLinks] handleUniversalLink:DynamicLinksHelper::URL completion:^(FIRDynamicLink* _Nullable dynamicLink, NSError* _Nullable error) {
				NSLog(@"handleUniversalLink:completion:");
				if (dynamicLink != nil) {
					NSLog(@"Dynamic Link URL: %@", [dynamicLink.url absoluteString]);
					DynamicLinksHelper::DynamicLink = (FIRDynamicLink*)CFBridgingRetain(dynamicLink);
				}

				if (error != nil) {
					NSLog(@"Dynamic Link parse error: %@", error.localizedDescription);
				}
			}];
		}
		
		if (DynamicLinksHelper::WasAppOpenViaUrl) {
			UE_LOG(LogFirebaseGoodies, Verbose, TEXT("Application was open from URL."));
			FIRDynamicLink* link = [[FIRDynamicLinks dynamicLinks] dynamicLinkFromCustomSchemeURL:DynamicLinksHelper::URL];
			if (link != nil)
				DynamicLinksHelper::DynamicLink = (FIRDynamicLink*)CFBridgingRetain(link);
		}
	});

	FIOSCoreDelegates::OnOpenURL.AddStatic([](UIApplication* application, NSURL* url, NSString* sourceApplication, id annotation) {
		[[GIDSignIn sharedInstance] handleURL:url];
	});

	if (FirebaseGoodiesSettings->bEnableAPNForIOS)
	{
		dispatch_async(dispatch_get_main_queue(), ^{
			MessagingDelegate = [FGCloudMessagingDelegate new];

			MessagingDelegate.onTokenReceived = ^(NSString* token) {
				FString Token = FString(token);

				AsyncTask(ENamedThreads::GameThread, [=]() {
					UFGCloudMessaging::OnNewTokenCallback.ExecuteIfBound(Token);
				});
			};

			[FIRMessaging messaging].delegate = MessagingDelegate;

			//Register for notifications
			UNUserNotificationCenter* notificationCenter = [UNUserNotificationCenter currentNotificationCenter];

			[notificationCenter getNotificationSettingsWithCompletionHandler:^(UNNotificationSettings* settings) {
				if (settings.authorizationStatus == UNAuthorizationStatusDenied ||
					settings.authorizationStatus == UNAuthorizationStatusNotDetermined)
				{
					UNAuthorizationOptions authOptions = UNAuthorizationOptionAlert | UNAuthorizationOptionSound | UNAuthorizationOptionBadge;
					[notificationCenter requestAuthorizationWithOptions:authOptions
													  completionHandler:^(BOOL granted, NSError* _Nullable error) {
														  // ...
													  }];
				}
			}];

			[[UIApplication sharedApplication] registerForRemoteNotifications];
		});

		FCoreDelegates::ApplicationReceivedRemoteNotificationDelegate.AddStatic([](FString json, int state) {
			UFGCloudMessaging::OnRemoteNotificationReceivedCallback.ExecuteIfBound(json, state);
		});
	}
#endif
}

void FFirebaseGoodiesModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "FirebaseGoodies");
	}

	if (!GExitPurge)
	{
		// If we're in exit purge, this object has already been destroyed
		FirebaseGoodiesSettings->RemoveFromRoot();
	}
	else
	{
		FirebaseGoodiesSettings = nullptr;
	}
}

UFirebaseGoodiesSettings* FFirebaseGoodiesModule::GetSettings() const
{
	return FirebaseGoodiesSettings;
}

FString FFirebaseGoodiesModule::GetPluginDirAbsPath() const
{
	const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("FirebaseGoodies"));

	if(!Plugin)
	{
		return FString();
	}

	return FPaths::ConvertRelativePathToFull(Plugin->GetBaseDir());
}

#undef LOCTEXT_NAMESPACE

DEFINE_LOG_CATEGORY(LogFirebaseGoodies);

IMPLEMENT_MODULE(FFirebaseGoodiesModule, FirebaseGoodies)
