// Copyright (c) 2022 Nineva Studios

#include "DatabaseRefIOS.h"
#include "DataSnapshotIOS.h"
#include "MutableDataIOS.h"

#include "Database/FGDataSnapshot.h"
#include "Database/FGMutableData.h"

#include "FirebaseGoodiesLog.h"

#include "Database/Callback/FGDatabaseRefCallback.h"

#include "IOS/Utils/FGValueVariantUtils.h"

DatabaseRefIOS::DatabaseRefIOS()
	: DbRef(nullptr)
	, DbQuery(nullptr)
{
}

DatabaseRefIOS::~DatabaseRefIOS()
{
	if (DbRef)
		[DbRef release];
	if (DbQuery)
		[DbQuery release];
}

void DatabaseRefIOS::Init(const FString& DatabaseUrl, const FString& InitialPath)
{
	FIRDatabase* database = DatabaseUrl.IsEmpty() ? [FIRDatabase database] : [FIRDatabase databaseWithURL:DatabaseUrl.GetNSString()];
	if (InitialPath.IsEmpty())
	{
		DbRef = [database reference];
		[DbRef retain];
	}
	else
	{
		DbRef = [database referenceWithPath:InitialPath.GetNSString()];
		[DbRef retain];
	}
}

void DatabaseRefIOS::SetValue(const FFGValueVariant& Value, const FFGValueVariant& Priority)
{
	id ValueObject = ValueVariantUtils::VariantToId(Value);
	id PriorityObject = ValueVariantUtils::VariantToId(Priority);

	[DbRef setValue:ValueObject andPriority:PriorityObject];
}

void DatabaseRefIOS::SetTimestamp()
{
	[DbRef setValue:[FIRServerValue timestamp]];
}

void DatabaseRefIOS::SetPriority(const FFGValueVariant& Priority)
{
	id PriorityObject = ValueVariantUtils::VariantToId(Priority);
	[DbRef setPriority:PriorityObject];
}

FString DatabaseRefIOS::GetKey() const
{
	return FString([DbRef key]);
}

void DatabaseRefIOS::GetValue(const FOnDataChangedDelegate& OnDataReceived, const FOnCancelledDelegate& OnCancelled)
{
	UFGDatabaseRefCallback* NativeCallback = NewObject<UFGDatabaseRefCallback>();
	NativeCallback->BindDataChangedDelegate(OnDataReceived);
	NativeCallback->BindCancelDelegate(OnCancelled);

	typedef void (^TCallbackBlock)(FIRDataSnapshot* _Nonnull);
	typedef void (^TErrorBlock)(NSError* _Nonnull);

	TCallbackBlock CallbackBlock = ^(FIRDataSnapshot* _Nonnull Snapshot) {
		FOnDataChangedDelegate DataChangedDelegate = OnDataReceived;
		DataSnapshotIOS* snapshotPtr = new DataSnapshotIOS([Snapshot retain]);
		UFGDataSnapshot* Data = NewObject<UFGDataSnapshot>();
		Data->Init(MakeShareable(snapshotPtr));
		AsyncTask(ENamedThreads::GameThread, [=]() {
			NativeCallback->ExecuteDataChanged(Data);
		});
	};
	TErrorBlock ErrorBlock = ^(NSError* _Nonnull Error) {
		FOnCancelledDelegate CancelDelegate = OnCancelled;
		const FString ErrorMessage([Error localizedDescription]);
		int ErrorCode = [Error code];

		UE_LOG(LogFirebaseGoodies, Error, TEXT("Database operation failed: %s"), *ErrorMessage);
		AsyncTask(ENamedThreads::GameThread, [=]() {
			NativeCallback->ExecuteCancel(ErrorCode, ErrorMessage);
		});
	};

	[DbRef observeSingleEventOfType:FIRDataEventTypeValue withBlock:CallbackBlock withCancelBlock:ErrorBlock];
}

void DatabaseRefIOS::RemoveValue()
{
	[DbRef removeValue];
}

FString DatabaseRefIOS::Push()
{
	FIRDatabaseReference* NewRef = [DbRef childByAutoId];
	return FString([NewRef key]);
}

void DatabaseRefIOS::UpdateChildren(const TMap<FString, FFGValueVariant>& Update)
{
	FFGValueVariant MapWrapper;
	MapWrapper.Type = EValueType::Map;
	MapWrapper.Value = Update;

	id UpdateDict = ValueVariantUtils::VariantToId(MapWrapper);
	[DbRef updateChildValues:UpdateDict];
}

void DatabaseRefIOS::AddValueListener(const FOnDataChangedDelegate& OnDataChanged, const FOnCancelledDelegate& OnCancelled)
{
	UFGDatabaseRefCallback* NativeCallback = NewObject<UFGDatabaseRefCallback>();
	NativeCallback->BindDataChangedDelegate(OnDataChanged);
	NativeCallback->BindCancelDelegate(OnCancelled);

	typedef void (^TCallbackBlock)(FIRDataSnapshot* _Nonnull);
	typedef void (^TErrorBlock)(NSError* _Nonnull);

	TCallbackBlock CallbackBlock = ^(FIRDataSnapshot* _Nonnull Snapshot) {
		FOnDataChangedDelegate DataChangedDelegate = OnDataChanged;
		DataSnapshotIOS* snapshotPtr = new DataSnapshotIOS([Snapshot retain]);
		UFGDataSnapshot* Data = NewObject<UFGDataSnapshot>();
		Data->Init(MakeShareable(snapshotPtr));
		AsyncTask(ENamedThreads::GameThread, [=]() {
			NativeCallback->ExecuteDataChanged(Data);
		});
	};
	TErrorBlock ErrorBlock = ^(NSError* _Nonnull Error) {
		FOnCancelledDelegate CancelDelegate = OnCancelled;
		const FString ErrorMessage([Error localizedDescription]);
		int ErrorCode = [Error code];

		UE_LOG(LogFirebaseGoodies, Error, TEXT("Database operation failed: %s"), *ErrorMessage);
		AsyncTask(ENamedThreads::GameThread, [=]() {
			NativeCallback->ExecuteCancel(ErrorCode, ErrorMessage);
		});
	};

	RemoveValueListener();
	FIRDatabaseQuery* Query = DbQuery ? DbQuery : DbRef;
	ValueChangedEventHandle = [Query observeEventType:FIRDataEventTypeValue withBlock:CallbackBlock	withCancelBlock:ErrorBlock];
}

void DatabaseRefIOS::RemoveValueListener()
{
	if (ValueChangedEventHandle > 0)
	{
		FIRDatabaseQuery* Query = DbQuery ? DbQuery : DbRef;
		[Query removeObserverWithHandle:ValueChangedEventHandle];
		ValueChangedEventHandle = 0;
	}
}

void DatabaseRefIOS::AddChildListener(const FOnChildEventDelegate& OnChildEvent, const FOnCancelledDelegate& OnCancelled)
{
	UFGDatabaseRefCallback* NativeCallback = NewObject<UFGDatabaseRefCallback>();
	NativeCallback->BindOnChildEventDelegate(OnChildEvent);
	NativeCallback->BindCancelDelegate(OnCancelled);
	
	typedef void (^TCallbackBlock)(FIRDataSnapshot* _Nonnull, NSString* _Nullable);
	typedef void (^TErrorBlock)(NSError* _Nonnull);

	TCallbackBlock ChildAddedBlock = ^(FIRDataSnapshot* _Nonnull Snapshot, NSString* _Nullable PrevKey) {
		DataSnapshotIOS* snapshotPtr = new DataSnapshotIOS([Snapshot retain]);
		UFGDataSnapshot* Data = NewObject<UFGDataSnapshot>();
		Data->Init(MakeShareable(snapshotPtr));

		FString PrevKeyString = PrevKey ? FString(PrevKey) : FString();
		AsyncTask(ENamedThreads::GameThread, [=]() {
			NativeCallback->ExecuteOnChildEvent(EChileEventType::Added, Data, PrevKeyString);
		});
	};
	TCallbackBlock ChildChangedBlock = ^(FIRDataSnapshot* _Nonnull Snapshot, NSString* _Nullable PrevKey) {
		DataSnapshotIOS* snapshotPtr = new DataSnapshotIOS([Snapshot retain]);
		UFGDataSnapshot* Data = NewObject<UFGDataSnapshot>();
		Data->Init(MakeShareable(snapshotPtr));

		FString PrevKeyString = PrevKey ? FString(PrevKey) : FString();
		AsyncTask(ENamedThreads::GameThread, [=]() {
			NativeCallback->ExecuteOnChildEvent(EChileEventType::Changed, Data, PrevKeyString);
		});
	};
	TCallbackBlock ChildRemovedBlock = ^(FIRDataSnapshot* _Nonnull Snapshot, NSString* _Nullable PrevKey) {
		DataSnapshotIOS* snapshotPtr = new DataSnapshotIOS([Snapshot retain]);
		UFGDataSnapshot* Data = NewObject<UFGDataSnapshot>();
		Data->Init(MakeShareable(snapshotPtr));

		FString PrevKeyString = PrevKey ? FString(PrevKey) : FString();
		AsyncTask(ENamedThreads::GameThread, [=]() {
			NativeCallback->ExecuteOnChildEvent(EChileEventType::Removed, Data, PrevKeyString);
		});
	};
	TCallbackBlock ChildMovedBlock = ^(FIRDataSnapshot* _Nonnull Snapshot, NSString* _Nullable PrevKey) {
		DataSnapshotIOS* snapshotPtr = new DataSnapshotIOS([Snapshot retain]);
		UFGDataSnapshot* Data = NewObject<UFGDataSnapshot>();
		Data->Init(MakeShareable(snapshotPtr));

		FString PrevKeyString = PrevKey ? FString(PrevKey) : FString();
		AsyncTask(ENamedThreads::GameThread, [=]() {
			NativeCallback->ExecuteOnChildEvent(EChileEventType::Moved, Data, PrevKeyString);
		});
	};
	TErrorBlock ErrorBlock = ^(NSError* _Nonnull Error) {
		const FString ErrorMessage([Error localizedDescription]);
		int ErrorCode = [Error code];

		UE_LOG(LogFirebaseGoodies, Error, TEXT("Database operation failed: %s"), *ErrorMessage);
		AsyncTask(ENamedThreads::GameThread, [=]() {
			NativeCallback->ExecuteCancel(ErrorCode, ErrorMessage);
		});
	};

	RemoveChildListener();

	FIRDatabaseQuery* Query = DbQuery ? DbQuery : DbRef;

	ChildAddedEventHandle = [Query observeEventType:FIRDataEventTypeChildAdded andPreviousSiblingKeyWithBlock:ChildAddedBlock withCancelBlock:ErrorBlock];
	ChildChangedEventHandle = [Query observeEventType:FIRDataEventTypeChildChanged andPreviousSiblingKeyWithBlock:ChildChangedBlock	withCancelBlock:ErrorBlock];
	ChildRemovedEventHandle = [Query observeEventType:FIRDataEventTypeChildRemoved andPreviousSiblingKeyWithBlock:ChildRemovedBlock withCancelBlock:ErrorBlock];
	ChildMovedEventHandle = [Query observeEventType:FIRDataEventTypeChildMoved andPreviousSiblingKeyWithBlock:ChildMovedBlock withCancelBlock:ErrorBlock];
}

void DatabaseRefIOS::RemoveChildListener()
{
	FIRDatabaseQuery* Query = DbQuery ? DbQuery : DbRef;
	
	if (ChildAddedEventHandle > 0)
		[Query removeObserverWithHandle:ChildAddedEventHandle];
	if (ChildChangedEventHandle > 0)
		[Query removeObserverWithHandle:ChildChangedEventHandle];
	if (ChildRemovedEventHandle > 0)
		[Query removeObserverWithHandle:ChildRemovedEventHandle];
	if (ChildMovedEventHandle > 0)
		[Query removeObserverWithHandle:ChildMovedEventHandle];

	ChildAddedEventHandle = 0;
	ChildChangedEventHandle = 0;
	ChildRemovedEventHandle = 0;
	ChildMovedEventHandle = 0;
}

void DatabaseRefIOS::KeepSynced(bool Sync)
{
	[DbRef keepSynced:Sync];
}

void DatabaseRefIOS::OrderByChild(const FString& Path)
{
	FIRDatabaseQuery* Query = DbQuery ? DbQuery :DbRef;

	if (DbQuery)
		[DbQuery release];

	DbQuery = [Query queryOrderedByChild:Path.GetNSString()];
	[DbQuery retain];
}

void DatabaseRefIOS::OrderByKey()
{
	FIRDatabaseQuery* Query = DbQuery ? DbQuery : DbRef;

	if (DbQuery)
		[DbQuery release];

	DbQuery = [Query queryOrderedByKey];
	[DbQuery retain];
}

void DatabaseRefIOS::OrderByPriority()
{
	FIRDatabaseQuery* Query = DbQuery ? DbQuery : DbRef;

	if (DbQuery)
		[DbQuery release];

	DbQuery = [Query queryOrderedByPriority];
	[DbQuery retain];
}

void DatabaseRefIOS::OrderByValue()
{
	FIRDatabaseQuery* Query = DbQuery ? DbQuery : DbRef;

	if (DbQuery)
		[DbQuery release];

	DbQuery = [Query queryOrderedByValue];
	[DbQuery retain];
}

void DatabaseRefIOS::EndAtFloat(float Value, const FString& Key)
{
	FIRDatabaseQuery* Query = DbQuery ? DbQuery : DbRef;

	if (DbQuery)
		[DbQuery release];

	if (Key.IsEmpty())
	{
		DbQuery = [Query queryEndingAtValue:[NSNumber numberWithFloat:Value]];
	}
	else
	{
		DbQuery = [Query queryEndingAtValue:[NSNumber numberWithFloat:Value] childKey:Key.GetNSString()];
	}

	[DbQuery retain];
}

void DatabaseRefIOS::EndAtString(const FString& Value, const FString& Key)
{
	FIRDatabaseQuery* Query = DbQuery ? DbQuery : DbRef;

	if (DbQuery)
		[DbQuery release];

	if (Key.IsEmpty())
	{
		DbQuery = [Query queryEndingAtValue:Value.GetNSString()];
	}
	else
	{
		DbQuery = [Query queryEndingAtValue:Value.GetNSString() childKey:Key.GetNSString()];
	}

	[DbQuery retain];
}

void DatabaseRefIOS::StartAtFloat(float Value, const FString& Key)
{
	FIRDatabaseQuery* Query = DbQuery ? DbQuery : DbRef;

	if (DbQuery)
		[DbQuery release];
	
	if (Key.IsEmpty())
	{
		DbQuery = [Query queryStartingAtValue:[NSNumber numberWithFloat:Value]];
	}
	else
	{
		DbQuery = [Query queryStartingAtValue:[NSNumber numberWithFloat:Value] childKey:Key.GetNSString()];
	}

	[DbQuery retain];
}

void DatabaseRefIOS::StartAtString(const FString& Value, const FString& Key)
{
	FIRDatabaseQuery* Query = DbQuery ? DbQuery : DbRef;

	if (DbQuery)
		[DbQuery release];

	if (Key.IsEmpty())
	{
		DbQuery = [Query queryStartingAtValue:Value.GetNSString()];
	}
	else
	{
		DbQuery = [Query queryStartingAtValue:Value.GetNSString() childKey:Key.GetNSString()];
	}

	[DbQuery retain];
}

void DatabaseRefIOS::EqualToFloat(float Value, const FString& Key)
{
	FIRDatabaseQuery* Query = DbQuery ? DbQuery : DbRef;

	if (DbQuery)
		[DbQuery release];

	if (Key.IsEmpty())
	{
		DbQuery = [Query queryEqualToValue:[NSNumber numberWithFloat:Value]];
	}
	else
	{
		DbQuery = [Query queryEqualToValue:[NSNumber numberWithFloat:Value] childKey:Key.GetNSString()];
	}

	[DbQuery retain];
}

void DatabaseRefIOS::EqualToBool(bool Value, const FString& Key)
{
	FIRDatabaseQuery* Query = DbQuery ? DbQuery : DbRef;

	if (DbQuery)
		[DbQuery release];

	if (Key.IsEmpty())
	{
		DbQuery = [Query queryEqualToValue:[NSNumber numberWithBool:Value]];
	}
	else
	{
		DbQuery = [Query queryEqualToValue:[NSNumber numberWithBool:Value] childKey:Key.GetNSString()];
	}

	[DbQuery retain];
}

void DatabaseRefIOS::EqualToString(const FString& Value, const FString& Key)
{
	FIRDatabaseQuery* Query = DbQuery ? DbQuery : DbRef;

	if (DbQuery)
		[DbQuery release];

	if (Key.IsEmpty())
	{
		DbQuery = [Query queryEqualToValue:Value.GetNSString()];
	}
	else
	{
		DbQuery = [Query queryEqualToValue:Value.GetNSString() childKey:Key.GetNSString()];
	}

	[DbQuery retain];
}

void DatabaseRefIOS::LimitToFirst(int Limit)
{
	FIRDatabaseQuery* Query = DbQuery ? DbQuery : DbRef;

	if (DbQuery)
		[DbQuery release];

	DbQuery = [Query queryLimitedToFirst:Limit];

	[DbQuery retain];
}

void DatabaseRefIOS::LimitToLast(int Limit)
{
	FIRDatabaseQuery* Query = DbQuery ? DbQuery : DbRef;

	if (DbQuery)
		[DbQuery release];

	DbQuery = [Query queryLimitedToLast:Limit];

	[DbQuery retain];
}

void DatabaseRefIOS::OnDisconnectSetValue(const FFGValueVariant& Value)
{
	id ValueObject = ValueVariantUtils::VariantToId(Value);
	[DbRef onDisconnectSetValue:ValueObject];
}

void DatabaseRefIOS::OnDisconnectSetTimestamp()
{
	[DbRef onDisconnectSetValue:[FIRServerValue timestamp]];
}

void DatabaseRefIOS::OnDisconnectRemoveValue()
{
	[DbRef onDisconnectRemoveValue];
}

void DatabaseRefIOS::OnDisconnectUpdateChildren(const TMap<FString, FFGValueVariant>& Update)
{
	FFGValueVariant MapWrapper;
	MapWrapper.Type = EValueType::Map;
	MapWrapper.Value = Update;

	id UpdateDict = ValueVariantUtils::VariantToId(MapWrapper);
	[DbRef onDisconnectUpdateChildValues:UpdateDict];
}

void DatabaseRefIOS::CancelDisconnectOperations()
{
	[DbRef cancelDisconnectOperations];
}

void DatabaseRefIOS::BindTransactionTask(UFGTransactionTask* Task)
{
	TransactionTask = Task;
}

void DatabaseRefIOS::RunTransaction()
{
	[DbRef runTransactionBlock:^FIRTransactionResult* _Nonnull(FIRMutableData* _Nonnull Data) {
		id Value = Data.value;
		if (!Value || [Value isEqual:[NSNull null]])
			return [FIRTransactionResult successWithValue:Data];

		MutableDataIOS* data = new MutableDataIOS([Data retain]);
		UFGMutableData* MutableData = NewObject<UFGMutableData>();
		MutableData->Init(MakeShareable(data));

		ETransactionResult Result = static_cast<ETransactionResult>(TransactionTask->RunHandler(MutableData));

		return Result == ETransactionResult::Success ? [FIRTransactionResult successWithValue:Data] : [FIRTransactionResult abort];
	}
	andCompletionBlock:^(NSError* _Nullable Error, BOOL Commited, FIRDataSnapshot* _Nullable Snapshot) {
		if (Commited)
		{
			AsyncTask(ENamedThreads::GameThread, [=]() {
				TransactionTask->Complete.Broadcast(TEXT(""));
			});
		}
		else
		{
			const FString ErrorMessage([Error localizedDescription]);
			AsyncTask(ENamedThreads::GameThread, [=]() {
				TransactionTask->Abort.Broadcast(ErrorMessage);
			});
		}
	}];
}
