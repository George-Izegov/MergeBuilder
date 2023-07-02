// Copyright (c) 2022 Nineva Studios

#include "DataSnapshotIOS.h"

#import <FirebaseDatabase/FirebaseDatabase.h>

DataSnapshotIOS::DataSnapshotIOS(FIRDataSnapshot* Snapshot)
{
	DataSnapshot = Snapshot;
}

DataSnapshotIOS::~DataSnapshotIOS()
{
	if (DataSnapshot)
		[DataSnapshot release];
}

FString DataSnapshotIOS::GetKey() const
{
	return FString([DataSnapshot key]);;
}

FFGValueVariant DataSnapshotIOS::GetPriority() const
{
	id Priority = [DataSnapshot priority];
	return ValueVariantUtils::IdToVariant(Priority);
}

FFGValueVariant DataSnapshotIOS::GetValue()
{
	id Value = [DataSnapshot value];
	return ValueVariantUtils::IdToVariant(Value);
}

bool DataSnapshotIOS::Exists() const
{
	return [DataSnapshot exists];
}

TSharedPtr<IDataSnapshot> DataSnapshotIOS::Child(const FString& Path) const
{
	DataSnapshotIOS* snapshot = new DataSnapshotIOS([DataSnapshot childSnapshotForPath:Path.GetNSString()]);
	return MakeShareable(snapshot);
}

TArray<TSharedPtr<IDataSnapshot>> DataSnapshotIOS::GetChildren() const
{
	TArray<TSharedPtr<IDataSnapshot>> children;

	for (FIRDataSnapshot* Child in [DataSnapshot children])
	{
		DataSnapshotIOS* snapshot = new DataSnapshotIOS([Child retain]);
		children.Add(MakeShareable(snapshot));
	}

	return children;
}

int DataSnapshotIOS::GetChildrenCount() const
{
	return [DataSnapshot childrenCount];
}

bool DataSnapshotIOS::HasChild(const FString& Path) const
{
	return [DataSnapshot hasChild:Path.GetNSString()];
}

bool DataSnapshotIOS::HasChildren() const
{
	return [DataSnapshot hasChildren];
}
