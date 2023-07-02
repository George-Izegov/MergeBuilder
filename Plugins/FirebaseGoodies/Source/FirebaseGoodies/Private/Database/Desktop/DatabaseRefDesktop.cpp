// Copyright (c) 2022 Nineva Studios

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT

#include "DatabaseRefDesktop.h"
#include "DataSnapshotDesktop.h"
#include "MutableDataDesktop.h"
#include "DatabaseChildListener.h"
#include "DatabaseValueListener.h"

#include "Database/FGDataSnapshot.h"
#include "Database/FGMutableData.h"
#include "Database/FGTransactionHandler.h"
#include "Database/FGTransactionTask.h"

#include "FirebaseGoodiesLog.h"

#include "Desktop/FGVariantUtils.h"

DatabaseRefDesktop::DatabaseRefDesktop()
	: ValueListener(nullptr)
	, ChildListener(nullptr)
	, TransactionTask(nullptr)
{
}

DatabaseRefDesktop::~DatabaseRefDesktop()
{
	if (ValueListener)
		delete(ValueListener);
	if (ChildListener)
		delete(ChildListener);
}

void DatabaseRefDesktop::Init(const FString& DatabaseUrl, const FString& InitialPath)
{
	firebase::database::Database* database;

	if (DatabaseUrl.IsEmpty())
	{
		database = firebase::database::Database::GetInstance(firebase::App::GetInstance());
	}
	else
	{
		database = firebase::database::Database::GetInstance(firebase::App::GetInstance(), TCHAR_TO_ANSI(*DatabaseUrl));
	}

	if (InitialPath.IsEmpty())
	{
		DbRef = database->GetReference();
	}
	else
	{
		DbRef = database->GetReference(TCHAR_TO_ANSI(*InitialPath));
	}
}

void DatabaseRefDesktop::SetValue(const FFGValueVariant& Value, const FFGValueVariant& Priority)
{
	firebase::Future<void> task;
	if (Priority.Type == EValueType::Null)
	{
		task = DbRef.SetValue(FGVariantUtils::ValueVariantToFirebaseVariant(Value));
	}
	else
	{
		task = DbRef.SetValueAndPriority(FGVariantUtils::ValueVariantToFirebaseVariant(Value), FGVariantUtils::ValueVariantToFirebaseVariant(Priority));
	}
	
	task.AddOnCompletion([=](const ::firebase::Future<void>& callback) {
		if (callback.status() == firebase::FutureStatus::kFutureStatusComplete)
		{
			UE_LOG(LogFirebaseGoodies, Verbose, TEXT("Set Value success!"));
		}
		else
		{
			FString ErrorMessage = FString(callback.error_message());
			UE_LOG(LogFirebaseGoodies, Error, TEXT("Set Value error: %s"), *ErrorMessage);
		}
	});
}

void DatabaseRefDesktop::SetTimestamp()
{
	auto task = DbRef.SetValue(firebase::database::ServerTimestamp());
	task.AddOnCompletion([=](const ::firebase::Future<void>& callback) {
		if (callback.status() == firebase::FutureStatus::kFutureStatusComplete)
		{
			UE_LOG(LogFirebaseGoodies, Verbose, TEXT("Set Timestamp success!"));
		}
		else
		{
			FString ErrorMessage = FString(callback.error_message());
			UE_LOG(LogFirebaseGoodies, Error, TEXT("Set Timestamp error: %s"), *ErrorMessage);
		}
	});
}

void DatabaseRefDesktop::SetPriority(const FFGValueVariant& Priority)
{
	auto task = DbRef.SetPriority(FGVariantUtils::ValueVariantToFirebaseVariant(Priority));
	task.AddOnCompletion([=](const ::firebase::Future<void>& callback) {
		if (callback.status() == firebase::FutureStatus::kFutureStatusComplete)
		{
			UE_LOG(LogFirebaseGoodies, Verbose, TEXT("Set Priority success!"));
		}
		else
		{
			FString ErrorMessage = FString(callback.error_message());
			UE_LOG(LogFirebaseGoodies, Error, TEXT("Set Priority error: %s"), *ErrorMessage);
		}
	});
}

FString DatabaseRefDesktop::GetKey() const
{
	return DbRef.key();;
}

void DatabaseRefDesktop::GetValue(const FOnDataChangedDelegate& OnDataReceived, const FOnCancelledDelegate& OnCancelled)
{
	auto task = DbRef.GetValue();
	task.AddOnCompletion([=](const ::firebase::Future<firebase::database::DataSnapshot>& callback) {
		if (callback.status() == firebase::FutureStatus::kFutureStatusComplete)
		{
			DataSnapshotDesktop* snapshot = new DataSnapshotDesktop(*callback.result());

			UFGDataSnapshot* Data = NewObject<UFGDataSnapshot>();
			Data->Init(MakeShareable(snapshot));

			AsyncTask(ENamedThreads::GameThread, [=]() {
				OnDataReceived.ExecuteIfBound(Data);
			});
		}
		else
		{
			FString ErrorMessage = FString(callback.error_message());
			int ErrorCode = callback.error();

			UE_LOG(LogFirebaseGoodies, Error, TEXT("Database operation failed: %s"), *ErrorMessage);
			AsyncTask(ENamedThreads::GameThread, [=]() {
				OnCancelled.ExecuteIfBound(ErrorCode, ErrorMessage);
			});
		}
	});
}

void DatabaseRefDesktop::RemoveValue()
{
	DbRef.RemoveValue();
}

FString DatabaseRefDesktop::Push()
{
	firebase::database::DatabaseReference NewRef = DbRef.PushChild();
	return NewRef.key();
}

void DatabaseRefDesktop::UpdateChildren(const TMap<FString, FFGValueVariant>& Update)
{
	auto task = DbRef.UpdateChildren(FGVariantUtils::ValueVariantMapToFirebaseVariant(Update));
	task.AddOnCompletion([=](const ::firebase::Future<void>& callback) {
		if (callback.status() == firebase::FutureStatus::kFutureStatusComplete)
		{
			UE_LOG(LogFirebaseGoodies, Verbose, TEXT("Update Children success!"));
		}
		else
		{
			FString ErrorMessage = FString(callback.error_message());
			UE_LOG(LogFirebaseGoodies, Error, TEXT("Update Children error: %s"), *ErrorMessage);
		}
	});
}

void DatabaseRefDesktop::AddValueListener(const FOnDataChangedDelegate& OnDataChanged, const FOnCancelledDelegate& OnCancelled)
{
	DatabaseValueListener* Listener = new DatabaseValueListener();
	Listener->OnValueChangedDelegate = OnDataChanged;
	Listener->OnCancelledDelegate = OnCancelled;

	ValueListener = Listener;

	firebase::database::DatabaseReference::Query& query = Query.is_valid() ? Query : DbRef;
	query.AddValueListener(ValueListener);
}

void DatabaseRefDesktop::RemoveValueListener()
{
	firebase::database::DatabaseReference::Query& query = Query.is_valid() ? Query : DbRef;
	query.RemoveValueListener(ValueListener);

	delete(ValueListener);
	ValueListener = nullptr;
}

void DatabaseRefDesktop::AddChildListener(const FOnChildEventDelegate& OnChildEvent, const FOnCancelledDelegate& OnCancelled)
{
	DatabaseChildListener* Listener = new DatabaseChildListener();
	Listener->OnChildEventCallback = OnChildEvent;
	Listener->OnCancelledCallback = OnCancelled;

	ChildListener = Listener;

	firebase::database::DatabaseReference::Query& query = Query.is_valid() ? Query : DbRef;
	query.AddChildListener(ChildListener);
}

void DatabaseRefDesktop::RemoveChildListener()
{
	firebase::database::DatabaseReference::Query& query = Query.is_valid() ? Query : DbRef;
	query.RemoveChildListener(ChildListener);

	delete(ChildListener);
	ChildListener = nullptr;
}

void DatabaseRefDesktop::KeepSynced(bool Sync)
{
	DbRef.SetKeepSynchronized(Sync);
}

void DatabaseRefDesktop::OrderByChild(const FString& Path)
{
	firebase::database::DatabaseReference::Query query = Query.is_valid() ? Query : DbRef;

	Query = query.OrderByChild(TCHAR_TO_ANSI(*Path));
}

void DatabaseRefDesktop::OrderByKey()
{
	firebase::database::DatabaseReference::Query query = Query.is_valid() ? Query : DbRef;

	Query = query.OrderByKey();
}

void DatabaseRefDesktop::OrderByPriority()
{
	firebase::database::DatabaseReference::Query query = Query.is_valid() ? Query : DbRef;

	Query = query.OrderByPriority();
}

void DatabaseRefDesktop::OrderByValue()
{
	firebase::database::DatabaseReference::Query query = Query.is_valid() ? Query : DbRef;

	Query = query.OrderByValue();
}

void DatabaseRefDesktop::EndAtFloat(float Value, const FString& Key)
{
	firebase::database::DatabaseReference::Query query = Query.is_valid() ? Query : DbRef;

	if (Key.IsEmpty())
	{
		Query = query.EndAt(firebase::Variant::FromDouble(Value));
	}
	else
	{
		Query = query.EndAt(firebase::Variant::FromDouble(Value), TCHAR_TO_ANSI(*Key));
	}
}

void DatabaseRefDesktop::EndAtString(const FString& Value, const FString& Key)
{
	firebase::database::DatabaseReference::Query query = Query.is_valid() ? Query : DbRef;

	if (Key.IsEmpty())
	{
		Query = query.EndAt(firebase::Variant::FromStaticString(TCHAR_TO_ANSI(*Value)));
	}
	else
	{
		Query = query.EndAt(firebase::Variant::FromStaticString(TCHAR_TO_ANSI(*Value)), TCHAR_TO_ANSI(*Key));
	}
}

void DatabaseRefDesktop::StartAtFloat(float Value, const FString& Key)
{
	firebase::database::DatabaseReference::Query query = Query.is_valid() ? Query : DbRef;

	if (Key.IsEmpty())
	{
		Query = query.StartAt(firebase::Variant::FromDouble(Value));
	}
	else
	{
		Query = query.StartAt(firebase::Variant::FromDouble(Value), TCHAR_TO_ANSI(*Key));
	}
}

void DatabaseRefDesktop::StartAtString(const FString& Value, const FString& Key)
{
	firebase::database::DatabaseReference::Query query = Query.is_valid() ? Query : DbRef;

	if (Key.IsEmpty())
	{
		Query = query.StartAt(firebase::Variant::FromStaticString(TCHAR_TO_ANSI(*Value)));
	}
	else
	{
		Query = query.StartAt(firebase::Variant::FromStaticString(TCHAR_TO_ANSI(*Value)), TCHAR_TO_ANSI(*Key));
	}
}

void DatabaseRefDesktop::EqualToFloat(float Value, const FString& Key)
{
	firebase::database::DatabaseReference::Query query = Query.is_valid() ? Query : DbRef;

	if (Key.IsEmpty())
	{
		Query = query.EqualTo(firebase::Variant::FromDouble(Value));
	}
	else
	{
		Query = query.EqualTo(firebase::Variant::FromDouble(Value), TCHAR_TO_ANSI(*Key));
	}
}

void DatabaseRefDesktop::EqualToBool(bool Value, const FString& Key)
{
	firebase::database::DatabaseReference::Query query = Query.is_valid() ? Query : DbRef;

	if (Key.IsEmpty())
	{
		Query = query.EqualTo(firebase::Variant::FromBool(Value));
	}
	else
	{
		Query = query.EqualTo(firebase::Variant::FromBool(Value), TCHAR_TO_ANSI(*Key));
	}
}

void DatabaseRefDesktop::EqualToString(const FString& Value, const FString& Key)
{
	firebase::database::DatabaseReference::Query query = Query.is_valid() ? Query : DbRef;

	if (Key.IsEmpty())
	{
		Query = query.EqualTo(firebase::Variant::FromStaticString(TCHAR_TO_ANSI(*Value)));
	}
	else
	{
		Query = query.EqualTo(firebase::Variant::FromStaticString(TCHAR_TO_ANSI(*Value)), TCHAR_TO_ANSI(*Key));
	}
}

void DatabaseRefDesktop::LimitToFirst(int Limit)
{
	firebase::database::DatabaseReference::Query query = Query.is_valid() ? Query : DbRef;

	Query = query.LimitToFirst(Limit);
}

void DatabaseRefDesktop::LimitToLast(int Limit)
{
	firebase::database::DatabaseReference::Query query = Query.is_valid() ? Query : DbRef;

	Query = query.LimitToLast(Limit);
}

void DatabaseRefDesktop::OnDisconnectSetValue(const FFGValueVariant& Value)
{
	DbRef.OnDisconnect()->SetValue(FGVariantUtils::ValueVariantToFirebaseVariant(Value));
}

void DatabaseRefDesktop::OnDisconnectSetTimestamp()
{
	DbRef.OnDisconnect()->SetValue(firebase::database::ServerTimestamp());
}

void DatabaseRefDesktop::OnDisconnectRemoveValue()
{
	DbRef.OnDisconnect()->RemoveValue();
}

void DatabaseRefDesktop::OnDisconnectUpdateChildren(const TMap<FString, FFGValueVariant>& Update)
{
	DbRef.OnDisconnect()->UpdateChildren(FGVariantUtils::ValueVariantMapToFirebaseVariant(Update));
}

void DatabaseRefDesktop::CancelDisconnectOperations()
{
	DbRef.OnDisconnect()->Cancel();
}

void DatabaseRefDesktop::BindTransactionTask(UFGTransactionTask* Task)
{
	TransactionTask = Task;
}

void DatabaseRefDesktop::RunTransaction()
{
	DbRef.RunTransaction([=](firebase::database::MutableData* mutableData) {
		MutableDataDesktop* data = new MutableDataDesktop(mutableData);

		UFGMutableData* MutableData = NewObject<UFGMutableData>();
		MutableData->Init(MakeShareable(data));

		ETransactionResult Result = static_cast<ETransactionResult>(TransactionTask->RunHandler(MutableData));

		if(Result == ETransactionResult::Success)
		{
			TransactionTask->Complete.Broadcast(TEXT(""));
			return firebase::database::kTransactionResultSuccess;
		}
		else
		{
			TransactionTask->Abort.Broadcast(TEXT("Transaction failed"));
			return firebase::database::kTransactionResultAbort;
		}
	});
}

#endif
