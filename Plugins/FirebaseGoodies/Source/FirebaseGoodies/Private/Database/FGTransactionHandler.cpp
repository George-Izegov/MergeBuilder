// Copyright (c) 2022 Nineva Studios

#include "Database/FGTransactionHandler.h"

#include "Database/FGMutableData.h"
#include "FirebaseGoodiesLog.h"

ETransactionResult UFGTransactionHandler::DoTransaction_Implementation(UFGMutableData* MutableData)
{
	UE_LOG(LogFirebaseGoodies, Warning, TEXT("Transaction handler was not overriden. Nothing will happen."));
	return ETransactionResult::Success;
}
