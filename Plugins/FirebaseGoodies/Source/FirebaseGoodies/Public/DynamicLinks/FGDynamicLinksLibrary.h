// Copyright (c) 2021 Nineva Studios

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "FGDynamicLinksLibrary.generated.h"

/**
* The match type of the Dynamic Link.
*/
UENUM(BlueprintType)
enum class EDynamicLinksMatchType : uint8
{
	/* The match has not been achieved. */
	None = 0,
	/* The match between the Dynamic Link and this device may not be perfect, hence you should not reveal any personal information related to the Dynamic Link. */
	Weak = 1,
	/* The match between the Dynamic Link and this device has high confidence but small possibility of error still exist. */
	Default = 2,
	/* The match between the Dynamic Link and this device is exact, hence you may reveal personal information related to the Dynamic Link. */
	Unique = 3
};

UCLASS()
class FIREBASEGOODIES_API UFGDynamicLinksLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/// <summary>
	/// Returns the processed URL from the Dynamic Link received by the application.
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Dynamic Links")
	static FString GetDynamicLinkUrl();

	/// <summary>
	/// Returns the minimum Android application version configured in the Dynamic Link.
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Dynamic Links")
	static int GetMinimumAppVersionAndroid();

	/// <summary>
	/// Returns the minimum iOS application version configured in the Dynamic Link.
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Dynamic Links")
	static FString GetMinimumAppVersionIos();

	/// <summary>
	/// Returns the URL match type.
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Dynamic Links")
	static EDynamicLinksMatchType GetMatchTypeIos();
	
	/// <summary>
	/// Clears the saved Dynamic Link data. Should be called after your application has processed the link.
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Dynamic Links")
	static void ClearDynamicLinkData();
	
	/// <summary>
	/// Whether the application was open via Dynamic Link. Returns false after ClearDynamicLinkData has been called.
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Dynamic Links")
	static bool WasAppOpenViaDynamicLink();

private:

	static const ANSICHAR* FGDynamicLinksClassName;
};
