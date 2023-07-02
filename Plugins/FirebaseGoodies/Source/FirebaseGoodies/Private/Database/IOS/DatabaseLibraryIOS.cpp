// Copyright (c) 2022 Nineva Studios

#include "DatabaseLibraryIOS.h"

#include "IOS/Utils/FGValueVariantUtils.h"

#import <FirebaseDatabase/FirebaseDatabase.h>

void DatabaseLibraryIOS::GoOffline()
{
	[[FIRDatabase database] goOffline];
}

void DatabaseLibraryIOS::GoOnline()
{
	[[FIRDatabase database] goOnline];
}

void DatabaseLibraryIOS::PurgeOutstandingWrites()
{
	[[FIRDatabase database] purgeOutstandingWrites];
}

FFGValueVariant DatabaseLibraryIOS::RealtimeDatabaseTimestamp()
{
	return ValueVariantUtils::IdToVariant([FIRServerValue timestamp]);
}

void DatabaseLibraryIOS::SetPersistenceCacheSizeBytes(int SizeInBytes)
{
	[FIRDatabase database].persistenceCacheSizeBytes = SizeInBytes;
}

void DatabaseLibraryIOS::SetPersistenceEnabled(bool bEnabled)
{
	[FIRDatabase database].persistenceEnabled = bEnabled;
}
