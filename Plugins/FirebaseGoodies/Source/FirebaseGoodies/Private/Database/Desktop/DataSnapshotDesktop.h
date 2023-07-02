// Copyright (c) 2022 Nineva Studios

#pragma once

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT

#include "Database/Interface/IDataSnapshot.h"

#include "firebase/database.h"

class DataSnapshotDesktop : public IDataSnapshot
{
public:
	DataSnapshotDesktop(firebase::database::DataSnapshot Snapshot);

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
	firebase::database::DataSnapshot DataSnapshot;
};

#endif