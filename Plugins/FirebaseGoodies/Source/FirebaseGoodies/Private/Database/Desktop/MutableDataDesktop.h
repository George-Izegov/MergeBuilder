// Copyright (c) 2022 Nineva Studios

#pragma once

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT

#include "Database/Interface/IMutableData.h"

#include "firebase/database.h"

class MutableDataDesktop : public IMutableData
{
public:
	MutableDataDesktop(firebase::database::MutableData* data);

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
	firebase::database::MutableData* MutableData;
};

#endif
