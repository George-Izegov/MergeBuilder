// Copyright (c) 2021 Nineva Studios

#pragma once

#include "firebase/storage.h"
#include "CloudStorage/FGCloudStorage.h"

class FGStorageOperationListener : public firebase::storage::Listener {
public:

    FCloudStorageIntDelegate OnProgressCallback;

    void OnProgress(firebase::storage::Controller* controller) override {
        int progress = (int) ((float) controller->bytes_transferred() / (float) controller->total_byte_count() * 100);

        OnProgressCallback.ExecuteIfBound(progress);
    }

    void OnPaused(firebase::storage::Controller* controller) override {
    }
};