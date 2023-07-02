// Copyright (c) 2022 Nineva Studios

#pragma once

#include "Database/Interface/IMutableData.h"

@class FIRMutableData;

class MutableDataIOS : public IMutableData
{
public:
	MutableDataIOS(FIRMutableData* data);
	virtual ~MutableDataIOS() override;

	virtual FString GetKey() const override;
	virtual FFGValueVariant GetPriority() const override;
	virtual void SetPriority(const FFGValueVariant& Priority) override;
	virtual FFGValueVariant GetValue() override;
	virtual void SetValue(const FFGValueVariant& Value) override;
	virtual TSharedPtr<IMutableData> Child(const FString& Path) const override;
	virtual TArray<TSharedPtr<IMutableData>> GetChildren() const override;
	virtual int GetChildrenCount() const override;
	virtual bool HasChild(const FString& Path) const override;
	virtual bool HasChildren() const override;

private:
	FIRMutableData* MutableData;
};
