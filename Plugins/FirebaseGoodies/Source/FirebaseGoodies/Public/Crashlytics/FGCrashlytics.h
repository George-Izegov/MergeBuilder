// Copyright (c) 2022 Nineva Studios

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "FGCrashlytics.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FCrashlyticsBoolDelegate, bool, Result);
DECLARE_DYNAMIC_DELEGATE_OneParam(FCrashlyticsStringDelegate, FString, String);

/**
 * The Firebase Crashlytics API provides methods to annotate and manage fatal and non-fatal reports captured and reported to Firebase Crashlytics.
 * By default, Firebase Crashlytics is automatically initialized.
 */
UCLASS()
class FIREBASEGOODIES_API UFGCrashlytics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
	/**
	 * Records a non-fatal report to send to Crashlytics. Does LogMessage on iOS.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Crashlytics")
	static void RecordException(const FString& Message);
	
	/**
	 * Logs a message that's included in the next fatal or non-fatal report.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Crashlytics")
	static void LogMessage(const FString& Message);
	
	/**
	 * Records a user ID (identifier) that's associated with subsequent fatal and non-fatal reports.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Crashlytics")
	static void SetUserId(const FString& Identifier);
	
	/**
	 * Sets a custom key and boolean value that are associated with subsequent fatal and non-fatal reports.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Crashlytics")
	static void SetCustomBoolKey(const FString& Key, bool Value);
	
	/**
	 * Sets a custom key and float value that are associated with subsequent fatal and non-fatal reports.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Crashlytics")
	static void SetCustomFloatKey(const FString& Key, float Value);
	
	/**
	 * Sets a custom key and integer value that are associated with subsequent fatal and non-fatal reports.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Crashlytics")
	static void SetCustomIntKey(const FString& Key, int Value);
	
	/**
	 * Sets a custom key and long value that are associated with subsequent fatal and non-fatal reports.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Crashlytics")
	static void SetCustomLongKey(const FString& Key, int64 Value);
	
	/**
	 * Sets a custom key and string value that are associated with subsequent fatal and non-fatal reports.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Crashlytics")
	static void SetCustomStringKey(const FString& Key, const FString& Value);
	
	/**
	 * Checks a device for any fatal or non-fatal crash reports that haven't yet been sent to Crashlytics.
	 *
	 * @param OnSuccess - action to be completed with the boolean result of the call.
	 * @param OnError - action to be performed if the operation could not be completed.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Crashlytics")
	static void CheckForUnsentReports(const FCrashlyticsBoolDelegate& OnSuccess, const FCrashlyticsStringDelegate& OnError);
	
	/**
	 * If automatic data collection is disabled, this method queues up all the reports on a device to send to Crashlytics.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Crashlytics")
	static void SendUnsentReports();
	
	/**
	 * If automatic data collection is disabled, this method queues up all the reports on a device for deletion.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Crashlytics")
	static void DeleteUnsentReports();
	
	/**
	 * Checks whether the app crashed on its previous run.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Crashlytics")
	static bool DidCrashOnPreviousExecution();
	
	/**
	 * Enables or disables the automatic data collection configuration for Crashlytics.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Crashlytics")
	static void SetCrashlyticsCollectionEnabled(bool Enabled);
	
	static FCrashlyticsBoolDelegate OnCheckForUnsentReportsSuccess;
	static FCrashlyticsStringDelegate OnCheckForUnsentReportsError;
	
	static const ANSICHAR* FGCrashlyticsClassName;
};
