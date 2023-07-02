// Copyright (c) 2021 Nineva Studios

#pragma once

#include "firebase/messaging.h"

#include "CloudMessaging/FGCloudMessaging.h"
#include "CloudMessaging/FGRemoteMessage.h"

#include "Async/Async.h"

class FGMessagingListener : public firebase::messaging::Listener {
public:
	virtual void OnMessage(const firebase::messaging::Message& message) override
	{
		UFGRemoteMessage* Message = NewObject<UFGRemoteMessage>();
		Message->Init(message);

		AsyncTask(ENamedThreads::GameThread, [=]() {
			UFGCloudMessaging::OnMessageReceivedCallback.ExecuteIfBound(Message);
		});
	}

	virtual void OnTokenReceived(const char* token) override
	{
		FString Token(token);

		AsyncTask(ENamedThreads::GameThread, [=]() {
			UFGCloudMessaging::OnNewTokenCallback.ExecuteIfBound(Token);
		});
	}
};