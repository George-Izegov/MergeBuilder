// Copyright (c) 2022 Nineva Studios

#pragma once

#include "Kismet/BlueprintAsyncActionBase.h"
#include "Templates/SubclassOf.h"

#include "FGTransactionTask.generated.h"

class UFGDatabaseRef;
class UFGTransactionHandler;
class UFGMutableData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTransactionOutputPin, FString, ErrorMessage);

UCLASS()
class FIREBASEGOODIES_API UFGTransactionTask : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	/**
	 * Run a transaction.
	 *
	 * @param DatabaseReference - location of where the transaction is run
	 * @param Handler - Custom handler that performs the transaction
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "Firebase Goodies|Realtime Database")
	static UFGTransactionTask* RunTransaction(UFGDatabaseRef* DatabaseReference, TSubclassOf<UFGTransactionHandler> Handler);

	virtual void Activate() override;
	int RunHandler(UFGMutableData* Data);

	UPROPERTY(BlueprintAssignable)
	FOnTransactionOutputPin Complete;
	UPROPERTY(BlueprintAssignable)
	FOnTransactionOutputPin Abort;

private:
	UPROPERTY()
	UFGDatabaseRef* Ref;
	UPROPERTY()
	UFGTransactionHandler* Handler;
};
