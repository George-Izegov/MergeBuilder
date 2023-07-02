// Copyright (c) 2021 Nineva Studios

#pragma once

#import <Foundation/Foundation.h>
#import "FirebaseDynamicLinks/FirebaseDynamicLinks.h"

class DynamicLinksHelper
{
public:
	static bool WasAppOpenViaAppLink;
	static bool WasAppOpenViaUrl;
	static NSURL* URL;
	static FIRDynamicLink* DynamicLink;
};
