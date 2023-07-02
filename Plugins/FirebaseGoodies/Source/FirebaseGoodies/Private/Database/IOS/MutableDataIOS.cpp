// Copyright (c) 2022 Nineva Studios	

#include "MutableDataIOS.h"

#include "IOS/Utils/FGValueVariantUtils.h"

#import <FirebaseDatabase/FirebaseDatabase.h>

MutableDataIOS::MutableDataIOS(FIRMutableData* data)
{
	MutableData = data;
}

MutableDataIOS::~MutableDataIOS()
{
	if (MutableData)
		[MutableData release];
}

FString MutableDataIOS::GetKey() const
{
	return FString([MutableData key]);
}

FFGValueVariant MutableDataIOS::GetPriority() const
{
	id Priority = [MutableData priority];
	return ValueVariantUtils::IdToVariant(Priority);
}

void MutableDataIOS::SetPriority(const FFGValueVariant& Priority)
{
	id PriorityObject = ValueVariantUtils::VariantToId(Priority);
	MutableData.priority = PriorityObject;
}

FFGValueVariant MutableDataIOS::GetValue()
{
	id Value = [MutableData value];
	return ValueVariantUtils::IdToVariant(Value);
}

void MutableDataIOS::SetValue(const FFGValueVariant& Value)
{
	id ValueObject = ValueVariantUtils::VariantToId(Value);
	MutableData.value = ValueObject;
}

TSharedPtr<IMutableData> MutableDataIOS::Child(const FString& Path) const
{
	MutableDataIOS* snapshot = new MutableDataIOS([MutableData childDataByAppendingPath:Path.GetNSString()]);
	return MakeShareable(snapshot);
}

TArray<TSharedPtr<IMutableData>> MutableDataIOS::GetChildren() const
{
	TArray<TSharedPtr<IMutableData>> children;

	for (FIRMutableData* Child in [MutableData children])
	{
		MutableDataIOS* data = new MutableDataIOS([Child retain]);
		children.Add(MakeShareable(data));
	}

	return children;
}

int MutableDataIOS::GetChildrenCount() const
{
	return [MutableData childrenCount];
}

bool MutableDataIOS::HasChild(const FString& Path) const
{
	return [MutableData hasChildAtPath:Path.GetNSString()];
}

bool MutableDataIOS::HasChildren() const
{
	return [MutableData hasChildren];
}
