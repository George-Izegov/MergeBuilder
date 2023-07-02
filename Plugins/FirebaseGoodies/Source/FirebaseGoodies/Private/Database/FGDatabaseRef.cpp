// Copyright (c) 2022 Nineva Studios

#include "Database/FGDatabaseRef.h"

#include "Async/Async.h"
#include "Database/FGDataSnapshot.h"
#include "Database/FGMutableData.h"
#include "Database/FGTransactionHandler.h"
#include "Database/FGTransactionTask.h"
#include "FirebaseGoodiesLog.h"

#include "Interface/IDatabaseRef.h"

#if PLATFORM_ANDROID
#include "Android/DatabaseRefAndroid.h"
#endif

#if PLATFORM_IOS
#include "IOS/DatabaseRefIOS.h"
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
#include "Desktop/DatabaseRefDesktop.h"
#endif


UFGDatabaseRef::UFGDatabaseRef()
{
	if (StaticClass()->GetDefaultObject() != this)
	{
#if PLATFORM_ANDROID
		DatabaseRefImpl = MakeShareable(new DatabaseRefAndroid());
#endif
#if PLATFORM_IOS
		DatabaseRefImpl = MakeShareable(new DatabaseRefIOS());
#endif
#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
		DatabaseRefImpl = MakeShareable(new DatabaseRefDesktop());
#endif
#if (PLATFORM_WINDOWS || PLATFORM_MAC) && !FG_ENABLE_EDITOR_SUPPORT
		DatabaseRefImpl = nullptr;
#endif
	}
}

void UFGDatabaseRef::InitPlatformData(const FString& DatabaseUrl, const FString& InitialPath)
{
	if(DatabaseRefImpl)
		DatabaseRefImpl->Init(DatabaseUrl, InitialPath);

	RefPath = InitialPath.Right(1) == TEXT("/") ? InitialPath.LeftChop(InitialPath.Len() - 1) : InitialPath;
}

UFGDatabaseRef* UFGDatabaseRef::MakeDatabaseRootRef()
{
	UFGDatabaseRef* dbRef = NewObject<UFGDatabaseRef>();
	dbRef->InitPlatformData("", "");
	return dbRef;
}

UFGDatabaseRef* UFGDatabaseRef::MakeDatabaseRefFromPath(const FString& Path)
{
	UFGDatabaseRef* dbRef = NewObject<UFGDatabaseRef>();
	dbRef->InitPlatformData("", Path);
	return dbRef;
}

UFGDatabaseRef* UFGDatabaseRef::MakeDatabaseReference(const FString& DatabaseUrl, const FString& Path)
{
	UFGDatabaseRef* dbRef = NewObject<UFGDatabaseRef>();
	dbRef->InitPlatformData(DatabaseUrl, Path);
	return dbRef;
}

UFGDatabaseRef* UFGDatabaseRef::Child(const FString& Path) const
{
	FString NewPath = RefPath / Path;
	return MakeDatabaseRefFromPath(NewPath);
}

UFGDatabaseRef* UFGDatabaseRef::Parent() const
{
	return MakeDatabaseRefFromPath(GetPathToParent());
}

UFGDatabaseRef* UFGDatabaseRef::Root() const
{
	return MakeDatabaseRootRef();
}

void UFGDatabaseRef::SetValue(const FFGValueVariant& Value, const FFGValueVariant& Priority)
{
	if (Priority.Type == EValueType::Array || Priority.Type == EValueType::Map || Priority.Type == EValueType::Bool)
	{
		UE_LOG(LogFirebaseGoodies, Error, TEXT("Priority cannot be a collection or boolean. Has to be a numerical or string value"));
		return;
	}

	if(DatabaseRefImpl)
		DatabaseRefImpl->SetValue(Value, Priority);
}

void UFGDatabaseRef::SetTimestamp()
{
	if(DatabaseRefImpl)
		DatabaseRefImpl->SetTimestamp();
}

void UFGDatabaseRef::SetPriority(const FFGValueVariant& Priority)
{
	if (Priority.Type == EValueType::Array || Priority.Type == EValueType::Map || Priority.Type == EValueType::Bool)
	{
		UE_LOG(LogFirebaseGoodies, Error, TEXT("Priority cannot be a collection or boolean. Has to be a numerical or string value"));
		return;
	}

	if(DatabaseRefImpl)
		DatabaseRefImpl->SetPriority(Priority);
}

FString UFGDatabaseRef::GetKey() const
{
	if(DatabaseRefImpl)
		return DatabaseRefImpl->GetKey();

	return FString();
}

void UFGDatabaseRef::GetValue(const FOnDataChangedDelegate& OnDataReceived, const FOnCancelledDelegate& OnCancelled)
{
	if(DatabaseRefImpl)
		DatabaseRefImpl->GetValue(OnDataReceived, OnCancelled);
}

UFGDatabaseRef* UFGDatabaseRef::Push()
{
	FString ChildString;

	if(DatabaseRefImpl)
	{
		ChildString = DatabaseRefImpl->Push();
	}

	return Child(ChildString);
}

void UFGDatabaseRef::RemoveValue()
{
	if(DatabaseRefImpl)
		DatabaseRefImpl->RemoveValue();
}

void UFGDatabaseRef::UpdateChildren(const TMap<FString, FFGValueVariant>& Update)
{
	if(DatabaseRefImpl)
		DatabaseRefImpl->UpdateChildren(Update);
}

void UFGDatabaseRef::AddValueListener(const FOnDataChangedDelegate& OnDataChanged, const FOnCancelledDelegate& OnCancelled)
{
	if(DatabaseRefImpl)
		DatabaseRefImpl->AddValueListener(OnDataChanged, OnCancelled);
}

void UFGDatabaseRef::RemoveValueListener()
{
	if(DatabaseRefImpl)
		DatabaseRefImpl->RemoveValueListener();
}

void UFGDatabaseRef::AddChildListener(const FOnChildEventDelegate& OnChildEvent, const FOnCancelledDelegate& OnCancelled)
{
	if(DatabaseRefImpl)
		DatabaseRefImpl->AddChildListener(OnChildEvent, OnCancelled);
}

void UFGDatabaseRef::RemoveChildListener()
{
	if(DatabaseRefImpl)
		DatabaseRefImpl->RemoveChildListener();
}

void UFGDatabaseRef::KeepSynced(bool Sync)
{
	if(DatabaseRefImpl)
		DatabaseRefImpl->KeepSynced(Sync);
}

UFGDatabaseRef* UFGDatabaseRef::OrderByChild(const FString& Path)
{
	if(DatabaseRefImpl)
		DatabaseRefImpl->OrderByChild(Path);

	return this;
}

UFGDatabaseRef* UFGDatabaseRef::OrderByKey()
{
	if(DatabaseRefImpl)
		DatabaseRefImpl->OrderByKey();

	return this;
}

UFGDatabaseRef* UFGDatabaseRef::OrderByPriority()
{
	if(DatabaseRefImpl)
		DatabaseRefImpl->OrderByPriority();

	return this;
}

UFGDatabaseRef* UFGDatabaseRef::OrderByValue()
{
	if(DatabaseRefImpl)
		DatabaseRefImpl->OrderByValue();

	return this;
}

UFGDatabaseRef* UFGDatabaseRef::EndAtFloat(float Value, const FString& Key)
{
	if(DatabaseRefImpl)
		DatabaseRefImpl->EndAtFloat(Value, Key);

	return this;
}

UFGDatabaseRef* UFGDatabaseRef::EndAtString(const FString& Value, const FString& Key)
{
	if(DatabaseRefImpl)
		DatabaseRefImpl->EndAtString(Value, Key);

	return this;
}

UFGDatabaseRef* UFGDatabaseRef::StartAtFloat(float Value, const FString& Key)
{
	if(DatabaseRefImpl)
		DatabaseRefImpl->StartAtFloat(Value, Key);

	return this;
}

UFGDatabaseRef* UFGDatabaseRef::StartAtString(const FString& Value, const FString& Key)
{
	if(DatabaseRefImpl)
		DatabaseRefImpl->StartAtString(Value, Key);

	return this;
}

UFGDatabaseRef* UFGDatabaseRef::EqualToFloat(float Value, const FString& Key)
{
	if(DatabaseRefImpl)
		DatabaseRefImpl->EqualToFloat(Value, Key);

	return this;
}

UFGDatabaseRef* UFGDatabaseRef::EqualToBool(bool Value, const FString& Key)
{
	if(DatabaseRefImpl)
		DatabaseRefImpl->EqualToBool(Value, Key);

	return this;
}

UFGDatabaseRef* UFGDatabaseRef::EqualToString(const FString& Value, const FString& Key)
{
	if(DatabaseRefImpl)
		DatabaseRefImpl->EqualToString(Value, Key);

	return this;
}

UFGDatabaseRef* UFGDatabaseRef::LimitToFirst(int Limit)
{
	if(DatabaseRefImpl)
		DatabaseRefImpl->LimitToFirst(Limit);

	return this;
}

UFGDatabaseRef* UFGDatabaseRef::LimitToLast(int Limit)
{
	if(DatabaseRefImpl)
		DatabaseRefImpl->LimitToLast(Limit);

	return this;
}

void UFGDatabaseRef::OnDisconnectSetValue(const FFGValueVariant& Value)
{
	if(DatabaseRefImpl)
		DatabaseRefImpl->OnDisconnectSetValue(Value);
}

void UFGDatabaseRef::OnDisconnectSetTimestamp()
{
	if(DatabaseRefImpl)
		DatabaseRefImpl->OnDisconnectSetTimestamp();
}

void UFGDatabaseRef::OnDisconnectRemoveValue()
{
	if(DatabaseRefImpl)
		DatabaseRefImpl->OnDisconnectRemoveValue();
}

void UFGDatabaseRef::OnDisconnectUpdateChildren(const TMap<FString, FFGValueVariant>& Update)
{
	if(DatabaseRefImpl)
		DatabaseRefImpl->OnDisconnectUpdateChildren(Update);
}

void UFGDatabaseRef::CancelDisconnectOperations()
{
	if(DatabaseRefImpl)
		DatabaseRefImpl->CancelDisconnectOperations();
}

void UFGDatabaseRef::BindTransactionTask(UFGTransactionTask* Task)
{
	if(DatabaseRefImpl)
		DatabaseRefImpl->BindTransactionTask(Task);
}

void UFGDatabaseRef::RunTransaction()
{
	if(DatabaseRefImpl)
		DatabaseRefImpl->RunTransaction();
}

FString UFGDatabaseRef::GetPathToParent() const
{
	if (RefPath.IsEmpty())
		return RefPath;

	int SlashPos = RefPath.FindLastCharByPredicate([](TCHAR C) { return C == TEXT('/'); });
	return RefPath.LeftChop(RefPath.Len() - SlashPos);
}
