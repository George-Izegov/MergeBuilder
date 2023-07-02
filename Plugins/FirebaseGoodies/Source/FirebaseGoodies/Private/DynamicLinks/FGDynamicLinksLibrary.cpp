// Copyright (c) 2021 Nineva Studios

#include "DynamicLinks/FGDynamicLinksLibrary.h"

#if PLATFORM_ANDROID
#include "Android/Utils/FGJavaConvertor.h"
#include "Android/Utils/FGMethodCallUtils.h"
#endif

#if PLATFORM_IOS
#include "IOS/DynamicLinksHelper.h"
#endif

const ANSICHAR* UFGDynamicLinksLibrary::FGDynamicLinksClassName = "com/ninevastudios/unrealfirebase/FGDynamicLinks";

FString UFGDynamicLinksLibrary::GetDynamicLinkUrl()
{
	FString Result = "";

#if PLATFORM_ANDROID
	Result = FGMethodCallUtils::CallStaticStringMethod(FGDynamicLinksClassName, "getDynamicLinkUrl", "()Ljava/lang/String;");
#elif PLATFORM_IOS
	if (DynamicLinksHelper::DynamicLink != nil && DynamicLinksHelper::DynamicLink.url != nil) {
		Result = [DynamicLinksHelper::DynamicLink.url absoluteString];
	}
#endif

	return Result;
}

int UFGDynamicLinksLibrary::GetMinimumAppVersionAndroid()
{
	int Result = 0;

#if PLATFORM_ANDROID
	Result = FGMethodCallUtils::CallStaticIntMethod(FGDynamicLinksClassName, "getMinimumAppVersion", "()I");
#endif

	return Result;
}

FString UFGDynamicLinksLibrary::GetMinimumAppVersionIos()
{
	FString Result = "";

#if PLATFORM_IOS
	if (DynamicLinksHelper::DynamicLink != nil && DynamicLinksHelper::DynamicLink.minimumAppVersion != nil)
		Result = DynamicLinksHelper::DynamicLink.minimumAppVersion;
#endif

	return Result;
}

EDynamicLinksMatchType UFGDynamicLinksLibrary::GetMatchTypeIos()
{
	EDynamicLinksMatchType Result = EDynamicLinksMatchType::None;

#if PLATFORM_IOS
	if (DynamicLinksHelper::DynamicLink != nil)
		Result = (EDynamicLinksMatchType) DynamicLinksHelper::DynamicLink.matchType;
#endif

	return Result;
}

void UFGDynamicLinksLibrary::ClearDynamicLinkData()
{
#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(FGDynamicLinksClassName, "clearDynamicLink", "()V");
#elif PLATFORM_IOS
	DynamicLinksHelper::DynamicLink = nil;
	DynamicLinksHelper::WasAppOpenViaAppLink = false;
	DynamicLinksHelper::WasAppOpenViaUrl = false;
	DynamicLinksHelper::URL = nil;
#endif
}

bool UFGDynamicLinksLibrary::WasAppOpenViaDynamicLink()
{
	bool Result = false;
	
#if PLATFORM_ANDROID
	Result = FGMethodCallUtils::CallStaticBoolMethod(FGDynamicLinksClassName, "wasAppOpenViaDynamicLink", "()Z");
#elif PLATFORM_IOS
	Result = DynamicLinksHelper::WasAppOpenViaAppLink || DynamicLinksHelper::WasAppOpenViaUrl;
#endif
	
	return Result;
}
