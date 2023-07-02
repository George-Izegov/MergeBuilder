// Copyright (c) 2022 Nineva Studios

#pragma once

#include "Common/FGValueVariant.h"

#include "FGMutableData.generated.h"

class IMutableData;

UCLASS()
class FIREBASEGOODIES_API UFGMutableData : public UObject
{
	GENERATED_BODY()

public:
	UFGMutableData();
	virtual ~UFGMutableData() override;

	void Init(TSharedPtr<IMutableData> Data);

	/**
	 * Get the key name for the source location of this snapshot
	 *
	 * @return - Key string value
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Realtime Database")
	FString GetKey() const;

	/**
	 * Get the priority of the data in this snapshot
	 *
	 * @return - Priority variant of this snapshot
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Realtime Database")
	FFGValueVariant GetPriority() const;

	/**
	 * Set priority at the current path
	 *
	 * @param Priority - Priority to set at the current path
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Realtime Database")
	void SetPriority(const FFGValueVariant& Priority);

	/**
	 * Get the values contained in this snapshot
	 *
	 * @return - Value variant of this snapshot
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Realtime Database")
	FFGValueVariant GetValue();

	/**
	 * Set value at the current path
	 *
	 * @param Value - Value to set at the current path
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Realtime Database")
	void SetValue(const FFGValueVariant& Value);

	/**
	 * Get the snapshot at the specified relative path
	 *
	 * @param Path - Path to the new data snapshot
	 * @return - New data snapshot
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Realtime Database")
	UFGMutableData* Child(const FString& Path) const;

	/**
	 * Get access to all of the children of the current snapshot
	 *
	 * @return - Array of data snapshots
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Realtime Database")
	TArray<UFGMutableData*> GetChildren() const;

	/**
	 * Get the number of children the current snapshot contains
	 *
	 * @return - The number of children
	 */
	UFUNCTION(BlueprintPure, Category = "Firebase Goodies|Realtime Database")
	int GetChildrenCount() const;

	/**
	 * Check if the the snapshot has data at a specific location
	 *
	 * @param Path - Relative path to the child location
	 * @return - True if the snapshot contains a child at the provide location
	 */
	UFUNCTION(BlueprintPure, Category = "Firebase Goodies|Realtime Database")
	bool HasChild(const FString& Path) const;

	/**
	 * Indicates whether this snapshot has any children
	 *
	 * @return - True if the snapshot has at least one child
	 */
	UFUNCTION(BlueprintPure, Category = "Firebase Goodies|Realtime Database")
	bool HasChildren() const;

private:
	TSharedPtr<IMutableData> MutableDataImpl;
};
