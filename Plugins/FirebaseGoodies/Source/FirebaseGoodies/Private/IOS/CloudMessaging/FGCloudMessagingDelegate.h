// Copyright (c) 2022 Nineva Studios

#pragma once

#import <Foundation/Foundation.h>

@interface FGCloudMessagingDelegate : NSObject <FIRMessagingDelegate>

@property (nonatomic, copy) void (^onTokenReceived)(NSString* token);

@end
