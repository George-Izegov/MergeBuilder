// Copyright (c) 2022 Nineva Studios

#pragma once

#include "Misc/Variant.h"

#include "FGAnalyticsParameter.generated.h"

UENUM()
enum class EParameterType : uint8
{
	Undefined,
	Integer,
	Float,
	String
};

UCLASS(BlueprintType)
class FIREBASEGOODIES_API UFGAnalyticsParameter : public UObject
{
	GENERATED_BODY()

public:
	/**
	* Set the name of the parameter
	*
	* @param NewName - parameter's name.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Analytics")
	void SetName(const FString& NewName);

	/**
	* Set parameter value to an Integer. This locks the parameter type to an Integer.
	*
	* @param NewValue - parameter's value.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Analytics")
	void SetInt(int64 NewValue);

	/**
	* Set parameter value to an Float. This locks the parameter type to an Float.
	*
	* @param NewValue - parameter's value.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Analytics")
	void SetFloat(float NewValue);

	/**
	* Set parameter value to an String. This locks the parameter type to an String.
	*
	* @param NewValue - parameter's value.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Analytics")
	void SetString(const FString& NewValue);

	/**
	* Get parameter's name
	*
	* @return - parameter's name.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Analytics")
	FString GetName();

	/**
	* Get parameter's value
	*
	* @return - parameter's value.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Analytics")
	int64 GetInt();

	/**
	* Get parameter's value
	*
	* @return - parameter's value.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Analytics")
	float GetFloat();

	/**
	* Get parameter's value
	*
	* @return - parameter's value.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Analytics")
	FString GetString();

	/**
	* Get parameter's type
	*
	* @return - parameter's type.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Analytics")
	EParameterType GetType();

private:
	FString Name;
	FVariant Value;
};
