// Copyright (c) 2022 Nineva Studios

#include "Database/FGDataSnapshot.h"

#include "Interface/IDataSnapshot.h"

UFGDataSnapshot::UFGDataSnapshot()
	: DataSnapshotImpl(nullptr)
{
}

UFGDataSnapshot::~UFGDataSnapshot()
{
}

void UFGDataSnapshot::Init(TSharedPtr<IDataSnapshot> Snapshot)
{
	DataSnapshotImpl = Snapshot;
}

FString UFGDataSnapshot::GetKey() const
{
	if(!DataSnapshotImpl)
		return FString();

	return DataSnapshotImpl->GetKey();
}

FFGValueVariant UFGDataSnapshot::GetPriority() const
{
	if(!DataSnapshotImpl)
		return FFGValueVariant();

	return DataSnapshotImpl->GetPriority();
}

FFGValueVariant UFGDataSnapshot::GetValue()
{
	if(!DataSnapshotImpl)
		return FFGValueVariant();

	return DataSnapshotImpl->GetValue();
}

bool UFGDataSnapshot::Exists() const
{
	if(!DataSnapshotImpl)
		return false;

	return DataSnapshotImpl->Exists();
}

UFGDataSnapshot* UFGDataSnapshot::Child(const FString& Path) const
{
	if(!DataSnapshotImpl)
		return nullptr;

	UFGDataSnapshot* Child = NewObject<UFGDataSnapshot>();
	Child->Init(DataSnapshotImpl->Child(Path));

	return Child;
}

TArray<UFGDataSnapshot*> UFGDataSnapshot::GetChildren() const
{
	TArray<UFGDataSnapshot*> Children;

	if(!DataSnapshotImpl)
		return TArray<UFGDataSnapshot*>();

	for (const auto& child : DataSnapshotImpl->GetChildren())
	{
		UFGDataSnapshot* ChildSnapshot = NewObject<UFGDataSnapshot>();
		ChildSnapshot->Init(child);
		Children.Add(ChildSnapshot);
	}

	return Children;
}

int UFGDataSnapshot::GetChildrenCount() const
{
	if(!DataSnapshotImpl)
		return 0;

	return DataSnapshotImpl->GetChildrenCount();
}

bool UFGDataSnapshot::HasChild(const FString& Path) const
{
	if(!DataSnapshotImpl)
		return false;

	return DataSnapshotImpl->HasChild(Path);
}

bool UFGDataSnapshot::HasChildren() const
{
	if(!DataSnapshotImpl)
		return false;

	return DataSnapshotImpl->HasChildren();
}
