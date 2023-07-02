// Copyright (c) 2022 Nineva Studios

#pragma once

#include "Common/FGValueVariant.h"

class IDatabaseLibrary
{
public:
	virtual ~IDatabaseLibrary() = default;

	virtual void GoOffline() = 0;
	virtual void GoOnline() = 0;
	virtual void PurgeOutstandingWrites() = 0;
	virtual FFGValueVariant RealtimeDatabaseTimestamp() = 0;
	virtual void SetPersistenceCacheSizeBytes(int SizeInBytes) = 0;
	virtual void SetPersistenceEnabled(bool bEnabled) = 0;
};