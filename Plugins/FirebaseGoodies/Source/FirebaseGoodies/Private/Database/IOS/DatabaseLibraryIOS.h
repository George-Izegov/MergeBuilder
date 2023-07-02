// Copyright (c) 2022 Nineva Studios

#pragma once

#include "Database/Interface/IDatabaseLibrary.h"

class DatabaseLibraryIOS : public IDatabaseLibrary
{
public:
	virtual void GoOffline() override;
	virtual void GoOnline() override;
	virtual void PurgeOutstandingWrites() override;
	virtual FFGValueVariant RealtimeDatabaseTimestamp() override;
	virtual void SetPersistenceCacheSizeBytes(int SizeInBytes) override;
	virtual void SetPersistenceEnabled(bool bEnabled) override;
};
