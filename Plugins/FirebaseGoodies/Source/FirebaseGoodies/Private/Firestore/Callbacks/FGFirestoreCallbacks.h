// Copyright (c) 2022 Nineva Studios

#pragma once

#include "Firestore/FGFirestoreLibrary.h"

#include "FGFirestoreCallbacks.generated.h"

UCLASS()
class FIREBASEGOODIES_API UFGDocumentCallback : public UObject
{
	GENERATED_BODY()

public:
	UFGDocumentCallback();

	void BindDelegate(const FFirestoreDocumentDelegate& InDelegate);
	void ExecuteDelegate(UFGDocumentReference* Document);

private:
	UPROPERTY()
	FFirestoreDocumentDelegate Delegate;
};

UCLASS()
class FIREBASEGOODIES_API UFGDocumentSnapshotCallback : public UObject
{
	GENERATED_BODY()

public:
	UFGDocumentSnapshotCallback();

	void BindDelegate(const FFirestoreDocumentSnapshotDelegate& InDelegate);
	void ExecuteDelegate(UFGDocumentSnapshot* DocumentSnapshot);

private:
	UPROPERTY()
	FFirestoreDocumentSnapshotDelegate Delegate;
};

UCLASS()
class FIREBASEGOODIES_API UFGQuerySnapshotCallback : public UObject
{
	GENERATED_BODY()

public:
	UFGQuerySnapshotCallback();

	void BindDelegate(const FFirestoreQuerySnapshotDelegate& InDelegate);
	void ExecuteDelegate(UFGQuerySnapshot* QuerySnapshot);

private:
	UPROPERTY()
	FFirestoreQuerySnapshotDelegate Delegate;
};

UCLASS()
class FIREBASEGOODIES_API UFGFirestoreStringCallback : public UObject
{
	GENERATED_BODY()

public:
	UFGFirestoreStringCallback();

	void BindDelegate(const FFirestoreStringDelegate& InDelegate);
	void ExecuteDelegate(const FString& String);

private:
	UPROPERTY()
	FFirestoreStringDelegate Delegate;
};

UCLASS()
class FIREBASEGOODIES_API UFGFirestoreVoidCallback : public UObject
{
	GENERATED_BODY()

public:
	UFGFirestoreVoidCallback();

	void BindDelegate(const FFirestoreVoidDelegate& InDelegate);
	void ExecuteDelegate();

private:
	UPROPERTY()
	FFirestoreVoidDelegate Delegate;
};
