// Copyright (c) 2021 Nineva Studios

#pragma once

#include "firebase/auth.h"

#include "Auth/FGAuthLibrary.h"
#include "FirebaseGoodiesLog.h"

#include "Async/Async.h"

class FGPhoneListener : public firebase::auth::PhoneAuthProvider::Listener {
public:
    ~FGPhoneListener() override {}

    void OnVerificationCompleted(firebase::auth::Credential credential) override {
        UE_LOG(LogFirebaseGoodies, Verbose, TEXT("Verify phone number OnVerificationCompleted"));
        UFGAuthCredentials* result = NewObject<UFGAuthCredentials>();
        result->Init(credential);

        AsyncTask(ENamedThreads::GameThread, [=]() {
            UFGAuthLibrary::PhoneVerificationSuccessCallback.ExecuteIfBound(result);
        });
    }

    void OnVerificationFailed(const std::string& error) override {
        FString ErrorMessage = error.c_str();
        UE_LOG(LogFirebaseGoodies, Error, TEXT("Verify phone number error: %s"), *ErrorMessage);

        AsyncTask(ENamedThreads::GameThread, [=]() {
            UFGAuthLibrary::AuthErrorCallback.ExecuteIfBound(ErrorMessage);
        });
    }

    void OnCodeSent(const std::string& verification_id,
        const firebase::auth::PhoneAuthProvider::ForceResendingToken&
        force_resending_token) override {
        UE_LOG(LogFirebaseGoodies, Verbose, TEXT("Verify phone number OnCodeSent"));
        FString verificationIDString = verification_id.c_str();
        AsyncTask(ENamedThreads::GameThread, [=]() {
            UFGAuthLibrary::PhoneVerificationSentCallback.ExecuteIfBound(verificationIDString);
        });
    }

    void OnCodeAutoRetrievalTimeOut(const std::string& verification_id) override {
        UE_LOG(LogFirebaseGoodies, Verbose, TEXT("Verify phone number OnCodeAutoRetrievalTimeOut"));
        FString verificationIDString = verification_id.c_str();
        AsyncTask(ENamedThreads::GameThread, [=]() {
            UFGAuthLibrary::PhoneVerificationTimeoutCallback.ExecuteIfBound(verificationIDString);
        });
    }
};