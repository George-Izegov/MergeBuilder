// Copyright (c) 2021 Nineva Studios

#pragma once

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT

#include "DataSnapshotDesktop.h"

#include "firebase/database.h"

#include "Async/Async.h"

#include "Database/FGDatabaseRef.h"
#include "Database/FGDataSnapshot.h"

#include "FirebaseGoodiesLog.h"

class DatabaseValueListener : public firebase::database::ValueListener {
public:
	FOnDataChangedDelegate OnValueChangedDelegate;
	FOnCancelledDelegate OnCancelledDelegate;

	virtual void OnValueChanged(const firebase::database::DataSnapshot& snapshot) override
	{
		DataSnapshotDesktop* dataSnapshot = new DataSnapshotDesktop(snapshot);

		UFGDataSnapshot* Data = NewObject<UFGDataSnapshot>();
		Data->Init(MakeShareable(dataSnapshot));

		AsyncTask(ENamedThreads::GameThread, [=]() {
			OnValueChangedDelegate.ExecuteIfBound(Data);
		});
	};

	virtual void OnCancelled(const firebase::database::Error& error, const char* error_message) override
	{
		const FString ErrorMessage(error_message);
		int ErrorCode = (int) error;

		UE_LOG(LogFirebaseGoodies, Error, TEXT("Database operation failed: %s"), *ErrorMessage);
		AsyncTask(ENamedThreads::GameThread, [=]() {
			OnCancelledDelegate.ExecuteIfBound(ErrorCode, ErrorMessage);
		});
	};
};

#endif