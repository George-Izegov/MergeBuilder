// Copyright (c) 2022 Nineva Studios

#pragma once

#include "Database/Interface/IDataSnapshot.h"

#include "Android/AndroidJNI.h"

class DataSnapshotAndroid : public IDataSnapshot
{
public:
	DataSnapshotAndroid(jobject DataSnapshot);
	virtual ~DataSnapshotAndroid() override;

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
	jobject JDataSnapshot;

	jmethodID GetKeyMethod;
	jmethodID GetPriorityMethod;
	jmethodID GetValueMethod;
	jmethodID ExistsMethod;
	jmethodID ChildMethod;
	jmethodID GetChildrenMethod;
	jmethodID GetChildrenCountMethod;
	jmethodID HasChildMethod;
	jmethodID HasChildrenMethod;
};
