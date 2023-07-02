// Copyright (c) 2022 Nineva Studios

#pragma once

#include "Common/FGValueVariant.h"

class IMutableData
{
public:
	virtual ~IMutableData() = default;

	virtual FString GetKey() const = 0;

	virtual FFGValueVariant GetPriority() const = 0;
	virtual void SetPriority(const FFGValueVariant& Priority) = 0;
	virtual FFGValueVariant GetValue() = 0;
	virtual void SetValue(const FFGValueVariant& Value) = 0;

	virtual TSharedPtr<IMutableData> Child(const FString& Path) const = 0;
	virtual TArray<TSharedPtr<IMutableData>> GetChildren() const = 0;
	virtual int GetChildrenCount() const = 0;
	virtual bool HasChild(const FString& Path) const = 0;
	virtual bool HasChildren() const = 0;
};