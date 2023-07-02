// Copyright (c) 2022 Nineva Studios

#include "FGFirestoreCallbacks.h"

UFGDocumentCallback::UFGDocumentCallback()
{
	if (StaticClass()->GetDefaultObject() != this)
	{
		AddToRoot();
	}
}

void UFGDocumentCallback::BindDelegate(const FFirestoreDocumentDelegate& InDelegate)
{
	Delegate = InDelegate;
}

void UFGDocumentCallback::ExecuteDelegate(UFGDocumentReference* Document)
{
	AsyncTask(ENamedThreads::GameThread, [=]()
	{
		Delegate.ExecuteIfBound(Document);
	});

	RemoveFromRoot();
}

UFGDocumentSnapshotCallback::UFGDocumentSnapshotCallback()
{
	if (StaticClass()->GetDefaultObject() != this)
	{
		AddToRoot();
	}
}

void UFGDocumentSnapshotCallback::BindDelegate(const FFirestoreDocumentSnapshotDelegate& InDelegate)
{
	Delegate = InDelegate;
}

void UFGDocumentSnapshotCallback::ExecuteDelegate(UFGDocumentSnapshot* DocumentSnapshot)
{
	AsyncTask(ENamedThreads::GameThread, [=]()
	{
		Delegate.ExecuteIfBound(DocumentSnapshot);
	});

	RemoveFromRoot();
}

UFGQuerySnapshotCallback::UFGQuerySnapshotCallback()
{
	if (StaticClass()->GetDefaultObject() != this)
	{
		AddToRoot();
	}
}

void UFGQuerySnapshotCallback::BindDelegate(const FFirestoreQuerySnapshotDelegate& InDelegate)
{
	Delegate = InDelegate;
}

void UFGQuerySnapshotCallback::ExecuteDelegate(UFGQuerySnapshot* QuerySnapshot)
{
	AsyncTask(ENamedThreads::GameThread, [=]()
	{
		Delegate.ExecuteIfBound(QuerySnapshot);
	});

	RemoveFromRoot();
}

UFGFirestoreStringCallback::UFGFirestoreStringCallback()
{
	if (StaticClass()->GetDefaultObject() != this)
	{
		AddToRoot();
	}
}

void UFGFirestoreStringCallback::BindDelegate(const FFirestoreStringDelegate& InDelegate)
{
	Delegate = InDelegate;
}

void UFGFirestoreStringCallback::ExecuteDelegate(const FString& String)
{
	AsyncTask(ENamedThreads::GameThread, [=]()
	{
		Delegate.ExecuteIfBound(String);
	});

	RemoveFromRoot();
}

UFGFirestoreVoidCallback::UFGFirestoreVoidCallback()
{
	if (StaticClass()->GetDefaultObject() != this)
	{
		AddToRoot();
	}
}

void UFGFirestoreVoidCallback::BindDelegate(const FFirestoreVoidDelegate& InDelegate)
{
	Delegate = InDelegate;
}

void UFGFirestoreVoidCallback::ExecuteDelegate()
{
	AsyncTask(ENamedThreads::GameThread, [=]()
	{
		Delegate.ExecuteIfBound();
	});

	RemoveFromRoot();
}
