// Copyright (c) 2022 Nineva Studios

#pragma once

#include "Common/FGValueVariant.h"

class IDataSnapshot
{
public:
	virtual ~IDataSnapshot() = default;

	virtual FString GetKey() const = 0;
	virtual FFGValueVariant GetPriority() const = 0;
	virtual FFGValueVariant GetValue() = 0;

	virtual bool Exists() const = 0;

	virtual TSharedPtr<IDataSnapshot> Child(const FString& Path) const = 0;
	virtual TArray<TSharedPtr<IDataSnapshot>> GetChildren() const = 0;
	virtual int GetChildrenCount() const = 0;
	virtual bool HasChild(const FString& Path) const = 0;
	virtual bool HasChildren() const = 0;
};