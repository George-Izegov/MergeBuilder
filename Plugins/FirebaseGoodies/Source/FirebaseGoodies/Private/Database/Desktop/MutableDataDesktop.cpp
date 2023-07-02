// Copyright (c) 2022 Nineva Studios

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT

#include "MutableDataDesktop.h"

#include "Desktop/FGVariantUtils.h"

MutableDataDesktop::MutableDataDesktop(firebase::database::MutableData* data)
{
	MutableData = data;
}

FString MutableDataDesktop::GetKey() const
{
	return MutableData->key();
}

FFGValueVariant MutableDataDesktop::GetPriority() const
{
	return FGVariantUtils::FirebaseVariantToValueVariant(MutableData->priority());
}

void MutableDataDesktop::SetPriority(const FFGValueVariant& Priority)
{
	MutableData->set_priority(FGVariantUtils::ValueVariantToFirebaseVariant(Priority));
}

FFGValueVariant MutableDataDesktop::GetValue()
{
	return FGVariantUtils::FirebaseVariantToValueVariant(MutableData->value());
}

void MutableDataDesktop::SetValue(const FFGValueVariant& Value)
{
	MutableData->set_value(FGVariantUtils::ValueVariantToFirebaseVariant(Value));
}

TSharedPtr<IMutableData> MutableDataDesktop::Child(const FString& Path) const
{
	firebase::database::MutableData* childPtr = new firebase::database::MutableData(MutableData->Child(TCHAR_TO_ANSI(*Path)));
	MutableDataDesktop* data = new MutableDataDesktop(childPtr);
	return MakeShareable(data);
}

TArray<TSharedPtr<IMutableData>> MutableDataDesktop::GetChildren() const
{
	TArray<TSharedPtr<IMutableData>> children;

	for (auto& child : MutableData->children())
	{
		firebase::database::MutableData* childPtr = new firebase::database::MutableData(MoveTemp(child));
		MutableDataDesktop* data = new MutableDataDesktop(childPtr);
		children.Add(MakeShareable(data));
	}

	return children;
}

int MutableDataDesktop::GetChildrenCount() const
{
	return MutableData->children_count();;
}

bool MutableDataDesktop::HasChild(const FString& Path) const
{
	return MutableData->HasChild(TCHAR_TO_ANSI(*Path));
}

bool MutableDataDesktop::HasChildren() const
{
	return GetChildrenCount() > 0;
}

#endif
