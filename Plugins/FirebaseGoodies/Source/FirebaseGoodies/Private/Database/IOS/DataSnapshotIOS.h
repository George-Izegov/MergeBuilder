// Copyright (c) 2022 Nineva Studios

#pragma once

#include "Database/Interface/IDataSnapshot.h"

#include "IOS/Utils/FGValueVariantUtils.h"

@class FIRDataSnapshot;

class DataSnapshotIOS : public IDataSnapshot
{
public:
	DataSnapshotIOS(FIRDataSnapshot* Snapshot);
	virtual ~DataSnapshotIOS() override;

	virtual FString GetKey() const override;
	virtual FFGValueVariant GetPriority() const override;
	virtual FFGValueVariant GetValue() override;
	virtual bool Exists() const override;
	virtual TSharedPtr<IDataSnapshot> Child(const FString& Path) const override;
	virtual TArray<TSharedPtr<IDataSnapshot>> GetChildren() const override;
	virtual int GetChildrenCount() const override;
	virtual bool HasChild(const FString& Path) const override;
	virtual bool HasChildren() const override;

private:
	FIRDataSnapshot* DataSnapshot;
};
