// Copyright (c) 2022 Nineva Studios

#include "Database/FGTransactionTask.h"

#include "Database/FGDatabaseRef.h"
#include "Database/FGTransactionHandler.h"

#include "UObject/Package.h"
#include "UObject/UObjectGlobals.h"

UFGTransactionTask* UFGTransactionTask::RunTransaction(UFGDatabaseRef* DatabaseReference, TSubclassOf<UFGTransactionHandler> Handler)
{
	UFGTransactionTask* Task = NewObject<UFGTransactionTask>();

	Task->Ref = DatabaseReference;
	Task->Handler = NewObject<UFGTransactionHandler>(GetTransientPackage(), Handler);

	DatabaseReference->BindTransactionTask(Task);

	return Task;
}

void UFGTransactionTask::Activate()
{
	Ref->RunTransaction();
}

int UFGTransactionTask::RunHandler(UFGMutableData* Data)
{
	return static_cast<int>(Handler->DoTransaction(Data));
}
