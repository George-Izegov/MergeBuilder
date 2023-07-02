// Copyright (c) 2021 Nineva Studios

#include "DynamicLinksHelper.h"

bool DynamicLinksHelper::WasAppOpenViaAppLink = false;
bool DynamicLinksHelper::WasAppOpenViaUrl = false;
NSURL* DynamicLinksHelper::URL = nil;
FIRDynamicLink* DynamicLinksHelper::DynamicLink = nil;
