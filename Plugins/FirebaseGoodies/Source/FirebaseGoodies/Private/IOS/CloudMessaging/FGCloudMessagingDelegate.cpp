// Copyright (c) 2022 Nineva Studios

#include "FGCloudMessagingDelegate.h"

@implementation FGCloudMessagingDelegate

- (void) messaging:(FIRMessaging*)messaging didReceiveRegistrationToken:(NSString*)fcmToken {
	_onTokenReceived(fcmToken);
}

@end
