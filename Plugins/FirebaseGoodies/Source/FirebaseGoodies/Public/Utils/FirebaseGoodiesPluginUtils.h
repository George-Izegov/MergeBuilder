// Copyright (c) 2022 Nineva Studios

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "Interfaces/IPluginManager.h"
#include "PluginDescriptor.h"

#include "FirebaseGoodiesPluginUtils.generated.h"

UCLASS()
class UFirebaseGoodiesPluginUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/**
	* Get version name
	*
	* @param PluginName - name of the plugin to get info from
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Plugin Utils")
		static FString GetPluginVersionName(FString PluginName);
};