// Copyright (c) 2022 Nineva Studios

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT

#include "DatabaseLibraryDesktop.h"

#include "Desktop/FGVariantUtils.h"
#include "FirebaseGoodiesLog.h"

#include "firebase/database.h"

void DatabaseLibraryDesktop::GoOffline()
{
	firebase::database::Database* database = firebase::database::Database::GetInstance(firebase::App::GetInstance());

	database->GoOffline();
}

void DatabaseLibraryDesktop::GoOnline()
{
	firebase::database::Database* database = firebase::database::Database::GetInstance(firebase::App::GetInstance());

	database->GoOnline();
}

void DatabaseLibraryDesktop::PurgeOutstandingWrites()
{
	firebase::database::Database* database = firebase::database::Database::GetInstance(firebase::App::GetInstance());

	database->PurgeOutstandingWrites();
}

FFGValueVariant DatabaseLibraryDesktop::RealtimeDatabaseTimestamp()
{
	return FGVariantUtils::FirebaseVariantToValueVariant(firebase::database::ServerTimestamp());
}

void DatabaseLibraryDesktop::SetPersistenceCacheSizeBytes(int SizeInBytes)
{
	UE_LOG(LogFirebaseGoodies, Error, TEXT("Method is not supported for current platform"));
}

void DatabaseLibraryDesktop::SetPersistenceEnabled(bool bEnabled)
{
	firebase::database::Database* database = firebase::database::Database::GetInstance(firebase::App::GetInstance());

	database->set_persistence_enabled(bEnabled);
}

#endif
