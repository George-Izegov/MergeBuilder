// Copyright (c) 2022 Nineva Studios

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT

#include "DataSnapshotDesktop.h"

#include "Desktop/FGVariantUtils.h"

DataSnapshotDesktop::DataSnapshotDesktop(firebase::database::DataSnapshot Snapshot)
{
	DataSnapshot = Snapshot;
}

FString DataSnapshotDesktop::GetKey() const
{
	return DataSnapshot.key();
}

FFGValueVariant DataSnapshotDesktop::GetPriority() const
{
	return FGVariantUtils::FirebaseVariantToValueVariant(DataSnapshot.priority());
}

FFGValueVariant DataSnapshotDesktop::GetValue()
{
	return FGVariantUtils::FirebaseVariantToValueVariant(DataSnapshot.value());
}

bool DataSnapshotDesktop::Exists() const
{
	return DataSnapshot.exists();
}

TSharedPtr<IDataSnapshot> DataSnapshotDesktop::Child(const FString& Path) const
{
	DataSnapshotDesktop* snapshot = new DataSnapshotDesktop(DataSnapshot.Child(TCHAR_TO_ANSI(*Path)));
	return MakeShareable(snapshot);
}

TArray<TSharedPtr<IDataSnapshot>> DataSnapshotDesktop::GetChildren() const
{
	TArray<TSharedPtr<IDataSnapshot>> children;

	for (const auto& child : DataSnapshot.children())
	{
		DataSnapshotDesktop* snapshot = new DataSnapshotDesktop(child);
		children.Add(MakeShareable(snapshot));
	}

	return children;
}

int DataSnapshotDesktop::GetChildrenCount() const
{
	return DataSnapshot.children_count();
}

bool DataSnapshotDesktop::HasChild(const FString& Path) const
{
	return DataSnapshot.HasChild(TCHAR_TO_ANSI(*Path));;
}

bool DataSnapshotDesktop::HasChildren() const
{
	return DataSnapshot.has_children();
}

#endif
