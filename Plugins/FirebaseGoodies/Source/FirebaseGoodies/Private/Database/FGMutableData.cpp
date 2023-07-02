// Copyright (c) 2022 Nineva Studios

#include "Database/FGMutableData.h"

#include "FirebaseGoodiesLog.h"

#include "Interface/IMutableData.h"

UFGMutableData::UFGMutableData()
	: MutableDataImpl(nullptr)
{
}

UFGMutableData::~UFGMutableData()
{
}

void UFGMutableData::Init(TSharedPtr<IMutableData> Data)
{
	MutableDataImpl = Data;
}

FString UFGMutableData::GetKey() const
{
	if(!MutableDataImpl)
		return FString();

	return MutableDataImpl->GetKey();
}

FFGValueVariant UFGMutableData::GetPriority() const
{
	if(!MutableDataImpl)
		return FFGValueVariant();

	return MutableDataImpl->GetPriority();
}

void UFGMutableData::SetPriority(const FFGValueVariant& Priority)
{
	if (Priority.Type == EValueType::Array || Priority.Type == EValueType::Map || Priority.Type == EValueType::Bool)
	{
		UE_LOG(LogFirebaseGoodies, Error, TEXT("Priority cannot be a collection or boolean. Has to be a numerical or string value"));
		return;
	}

	if(!MutableDataImpl)
		return;

	return MutableDataImpl->SetPriority(Priority);
}

FFGValueVariant UFGMutableData::GetValue()
{
	if(!MutableDataImpl)
		return FFGValueVariant();

	return MutableDataImpl->GetValue();
}

void UFGMutableData::SetValue(const FFGValueVariant& Value)
{
	if(!MutableDataImpl)
		return;

	return MutableDataImpl->SetValue(Value);
}

UFGMutableData* UFGMutableData::Child(const FString& Path) const
{
	if(!MutableDataImpl)
		return nullptr;

	UFGMutableData* Child = NewObject<UFGMutableData>();
	Child->Init(MutableDataImpl->Child(Path));

	return Child;
}

TArray<UFGMutableData*> UFGMutableData::GetChildren() const
{
	TArray<UFGMutableData*> Children;

	if(!MutableDataImpl)
		return TArray<UFGMutableData*>();

	for (const auto& child : MutableDataImpl->GetChildren())
	{
		UFGMutableData* ChildSnapshot = NewObject<UFGMutableData>();
		ChildSnapshot->Init(child);
		Children.Add(ChildSnapshot);
	}

	return Children;
}

int UFGMutableData::GetChildrenCount() const
{
	if(!MutableDataImpl)
		return 0;

	return MutableDataImpl->GetChildrenCount();
}

bool UFGMutableData::HasChild(const FString& Path) const
{
	if(!MutableDataImpl)
		return false;

	return MutableDataImpl->HasChild(Path);
}

bool UFGMutableData::HasChildren() const
{
	if(!MutableDataImpl)
		return false;

	return MutableDataImpl->HasChildren();
}
