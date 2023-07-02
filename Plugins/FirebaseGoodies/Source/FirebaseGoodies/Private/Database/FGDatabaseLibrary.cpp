// Copyright (c) 2022 Nineva Studios

#include "Database/FGDatabaseLibrary.h"

#include "Interface/IDatabaseLibrary.h"

#if PLATFORM_ANDROID
#include "Android/DatabaseLibraryAndroid.h"
#endif

#if PLATFORM_IOS
#include "IOS/DatabaseLibraryIOS.h"
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
#include "Desktop/DatabaseLibraryDesktop.h"
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && !FG_ENABLE_EDITOR_SUPPORT
TSharedPtr<IDatabaseLibrary> UFGDatabaseLibrary::DatabaseLibraryImpl = nullptr;
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
TSharedPtr<IDatabaseLibrary> UFGDatabaseLibrary::DatabaseLibraryImpl = MakeShareable(new DatabaseLibraryDesktop());;
#endif

#if PLATFORM_ANDROID
TSharedPtr<IDatabaseLibrary> UFGDatabaseLibrary::DatabaseLibraryImpl = MakeShareable(new DatabaseLibraryAndroid());;
#endif

#if PLATFORM_IOS
TSharedPtr<IDatabaseLibrary> UFGDatabaseLibrary::DatabaseLibraryImpl = MakeShareable(new DatabaseLibraryIOS());;
#endif

void UFGDatabaseLibrary::GoOffline()
{
	if(!DatabaseLibraryImpl)
		return;

	DatabaseLibraryImpl->GoOffline();
}

void UFGDatabaseLibrary::GoOnline()
{
	if(!DatabaseLibraryImpl)
		return;

	DatabaseLibraryImpl->GoOnline();
}

void UFGDatabaseLibrary::PurgeOutstandingWrites()
{
	if(!DatabaseLibraryImpl)
		return;

	DatabaseLibraryImpl->PurgeOutstandingWrites();
}

void UFGDatabaseLibrary::SetPersistenceCacheSizeBytes(int SizeInBytes)
{
	if(!DatabaseLibraryImpl)
		return;

	DatabaseLibraryImpl->SetPersistenceCacheSizeBytes(SizeInBytes);
}

void UFGDatabaseLibrary::SetPersistenceEnabled(bool bEnabled)
{
	if(!DatabaseLibraryImpl)
		return;

	DatabaseLibraryImpl->SetPersistenceEnabled(bEnabled);
}

FFGValueVariant UFGDatabaseLibrary::RealtimeDatabaseTimestamp()
{
	if(!DatabaseLibraryImpl)
		return FFGValueVariant();

	return DatabaseLibraryImpl->RealtimeDatabaseTimestamp();
}
