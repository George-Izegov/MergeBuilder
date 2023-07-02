// Copyright (c) 2021 Nineva Studios

#pragma once

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT

#include "DataSnapshotDesktop.h"

#include "firebase/database.h"

#include "Async/Async.h"

#include "Database/FGDatabaseRef.h"
#include "Database/FGDataSnapshot.h"

#include "FirebaseGoodiesLog.h"

class DatabaseChildListener : public firebase::database::ChildListener
{
public:
	FOnChildEventDelegate OnChildEventCallback;
	FOnCancelledDelegate OnCancelledCallback;

	virtual void OnChildAdded(const firebase::database::DataSnapshot& snapshot, const char* previous_sibling_key) override
	{
		DataSnapshotDesktop* dataSnapshot = new DataSnapshotDesktop(snapshot);

		UFGDataSnapshot* Data = NewObject<UFGDataSnapshot>();
		Data->Init(MakeShareable(dataSnapshot));

		FString PrevKeyString = previous_sibling_key ? FString(previous_sibling_key) : FString();
		AsyncTask(ENamedThreads::GameThread, [=]()
		{
			OnChildEventCallback.ExecuteIfBound(EChileEventType::Added, Data, PrevKeyString);
		});
	}

	virtual void OnChildChanged(const firebase::database::DataSnapshot& snapshot, const char* previous_sibling_key) override
	{
		DataSnapshotDesktop* dataSnapshot = new DataSnapshotDesktop(snapshot);

		UFGDataSnapshot* Data = NewObject<UFGDataSnapshot>();
		Data->Init(MakeShareable(dataSnapshot));

		FString PrevKeyString = previous_sibling_key ? FString(previous_sibling_key) : FString();
		AsyncTask(ENamedThreads::GameThread, [=]()
		{
			OnChildEventCallback.ExecuteIfBound(EChileEventType::Changed, Data, PrevKeyString);
		});
	}

	virtual void OnChildMoved(const firebase::database::DataSnapshot& snapshot, const char* previous_sibling_key) override
	{
		DataSnapshotDesktop* dataSnapshot = new DataSnapshotDesktop(snapshot);

		UFGDataSnapshot* Data = NewObject<UFGDataSnapshot>();
		Data->Init(MakeShareable(dataSnapshot));

		FString PrevKeyString = previous_sibling_key ? FString(previous_sibling_key) : FString();
		AsyncTask(ENamedThreads::GameThread, [=]()
		{
			OnChildEventCallback.ExecuteIfBound(EChileEventType::Moved, Data, PrevKeyString);
		});
	}

	virtual void OnChildRemoved(const firebase::database::DataSnapshot& snapshot) override
	{
		DataSnapshotDesktop* dataSnapshot = new DataSnapshotDesktop(snapshot);

		UFGDataSnapshot* Data = NewObject<UFGDataSnapshot>();
		Data->Init(MakeShareable(dataSnapshot));

		AsyncTask(ENamedThreads::GameThread, [=]()
		{
			OnChildEventCallback.ExecuteIfBound(EChileEventType::Added, Data, TEXT(""));
		});
	}

	virtual void OnCancelled(const firebase::database::Error& error, const char* error_message) override
	{
		const FString ErrorMessage(error_message);
		int ErrorCode = (int)error;

		UE_LOG(LogFirebaseGoodies, Error, TEXT("Database operation failed: %s"), *ErrorMessage);
		AsyncTask(ENamedThreads::GameThread, [=]()
		{
			OnCancelledCallback.ExecuteIfBound(ErrorCode, ErrorMessage);
		});
	}
};

#endif
