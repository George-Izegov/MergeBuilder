// Copyright (c) 2022 Nineva Studios

#pragma once

#include "FGTransactionHandler.generated.h"

class UFGMutableData;

UENUM(BlueprintType)
enum class ETransactionResult : uint8
{
	Success = 0,
	Abort
};

UCLASS(Blueprintable)
class FIREBASEGOODIES_API UFGTransactionHandler : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	ETransactionResult DoTransaction(UFGMutableData* MutableData);
};
