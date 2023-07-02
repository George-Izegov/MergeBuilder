// Copyright (c) 2022 Nineva Studios

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "FGAnalytics.generated.h"

class UFGAnalyticsParameter;

UCLASS()
class FIREBASEGOODIES_API UFGAnalytics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	* Sets whether analytics collection is enabled for this app on this device. 
	*
	* @param bEnabled - analytics enabled state.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Analytics")
	static void SetAnalyticsCollectionEnabled(bool bEnabled);

	/**
	* Clears all analytics data for this app from the device and resets the app instance id. 
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Analytics")
	static void ResetAnalyticsData();

	/**
	* Sets the duration of inactivity that terminates the current session. The default value is 1800000 (30 minutes). 
	*
	* @param Milliseconds - session duration.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Analytics")
	static void SetSessionTimeoutDuration(int64 Milliseconds);

	/**
	* Set the current user's ID
	*
	* @param Id - ID to assign to the user (max 256 characters). Setting to an empty string removes the user ID.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Analytics")
	static void SetUserId(const FString& Id);

	/**
	* Set a property for the current user
	*
	* @param Name - name of the property (max 24 characters). The "firebase_", "google_" and "ga_" prefixes are reserved.
	* @param Value - the value of the property (max 36 characters). Setting to an empty string removes the user property.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Analytics")
	static void SetUserProperty(const FString& Name, const FString& Value);

	/**
	* Set the name of the current screen
	*
	* @param ScreenName - the name of the current screen. Set to an empty string to clear the current screen name.
	* @param ScreenClassName - the name of the screen class. Set to an empty string to use default class name.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Analytics")
	static void SetCurrentScreen(const FString& ScreenName, const FString& ScreenClassName);

	/**
	* Create an analytics event parameter and set its type to Integer
	*
	* @param Name - parameter's name.
	* @param Value - parameter's values.
	* @return - pointer to created parameter.
	*/
	UFUNCTION(BlueprintPure, Category = "Firebase Goodies|Analytics")
	static UFGAnalyticsParameter* CreateIntegerParameter(const FString& Name, int64 Value);

	/**
	* Create an analytics event parameter and set its type to Float
	*
	* @param Name - parameter's name.
	* @param Value - parameter's values.
	* @return - pointer to created parameter.
	*/
	UFUNCTION(BlueprintPure, Category = "Firebase Goodies|Analytics")
	static UFGAnalyticsParameter* CreateFloatParameter(const FString& Name, float Value);

	/**
	* Create an analytics event parameter and set its type to String
	*
	* @param Name - parameter's name.
	* @param Value - parameter's values.
	* @return - pointer to created parameter.
	*/
	UFUNCTION(BlueprintPure, Category = "Firebase Goodies|Analytics")
	static UFGAnalyticsParameter* CreateStringParameter(const FString& Name, const FString& Value);

	/**
	* Log an event with the provided name
	*
	* @param EventName - event's name.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Analytics")
	static void LogEvent(const FString& EventName);

	/**
	* Log an event with the provided name and parameter
	*
	* @param EventName - event's name.
	* @param Parameter - event's parameter.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Analytics")
	static void LogEventWithParameter(const FString& EventName, UFGAnalyticsParameter* Parameter);

	/**
	* Log an event with the provided name and parameters
	*
	* @param EventName - event's name.
	* @param Parameters - event's parameters.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Analytics")
	static void LogEventWithParameters(const FString& EventName, const TArray<UFGAnalyticsParameter*>& Parameters);
};
