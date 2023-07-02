// Copyright (c) 2022 Nineva Studios

#include "Auth/FGFirebaseUser.h"

#include "Auth/FGAuthLibrary.h"
#include "Auth/FGFirebaseUserInfo.h"
#include "FirebaseGoodiesLog.h"

#include "Async/Async.h"

#if PLATFORM_ANDROID
#include "Android/Utils/FGJavaConvertor.h"
#include "Android/Utils/FGMethodCallUtils.h"
#endif

FOnUserVoidTaskCompleted UFGFirebaseUser::OnUserVoidTaskCompletedCallback;
FOnUserUpdated UFGFirebaseUser::OnUserUpdatedCallback;
FOnUserOperationError UFGFirebaseUser::OnUserOperationErrorCallback;
FOnUserStringTaskCompleted UFGFirebaseUser::OnUserStringTaskCompletedCallback;

UFGFirebaseUser::~UFGFirebaseUser()
{
#if PLATFORM_ANDROID
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	Env->DeleteGlobalRef(JavaFirebaseUserObject);
	JavaFirebaseUserObject = nullptr;
#elif PLATFORM_IOS
	(FIRUser*)CFBridgingRelease(iosUser);
	iosUser = nil;
#endif
}

bool UFGFirebaseUser::IsAnonymous()
{
	bool result = false;

	if (!IsUserValid())
	{
		return result;
	}

#if PLATFORM_ANDROID
	result = FGMethodCallUtils::CallBoolMethod(JavaFirebaseUserObject, "isAnonymous", "()Z");
#elif PLATFORM_IOS
	result = [iosUser isAnonymous];
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	result = desktopUser->is_anonymous();
#endif

	return result;
}

bool UFGFirebaseUser::IsUserValid()
{
	bool bIsUserValid = false;
#if PLATFORM_ANDROID
	bIsUserValid = JavaFirebaseUserObject != nullptr;
#elif PLATFORM_IOS
	bIsUserValid = iosUser != nil;
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	bIsUserValid = desktopUser != nullptr;
#endif
	if (!bIsUserValid)
	{
		UE_LOG(LogFirebaseGoodies, Error, TEXT("User is not valid."));
	}
	return bIsUserValid;
}

void UFGFirebaseUser::Delete(const FOnUserVoidTaskCompleted& OnSuccess, const FOnUserOperationError& OnError)
{
	OnUserVoidTaskCompletedCallback = OnSuccess;
	OnUserOperationErrorCallback = OnError;

	if (!IsUserValid())
	{
		return;
	}

#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(UFGAuthLibrary::FGAuthClassName, "deleteUser", "(Lcom/google/firebase/auth/FirebaseUser;)V", JavaFirebaseUserObject);
#elif PLATFORM_IOS
	[iosUser deleteWithCompletion:^(NSError* _Nullable error) {
		if (error != nil)
		{
			FString errorString = FString(error.localizedDescription);
			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGFirebaseUser::OnUserOperationErrorCallback.ExecuteIfBound(GetUid(), errorString);
			});
			return;
		}

		UFGFirebaseUser::OnUserVoidTaskCompletedCallback.ExecuteIfBound(GetUid());
	}];
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	auto task = desktopUser->Delete(); 
	task.AddOnCompletion([=](const ::firebase::Future<void>& callback) {
		if (callback.status() == firebase::FutureStatus::kFutureStatusComplete)
		{
			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGFirebaseUser::OnUserVoidTaskCompletedCallback.ExecuteIfBound(GetUid());
			});
		}
		else
		{
			FString ErrorMessage = FString(callback.error_message());
			UE_LOG(LogFirebaseGoodies, Error, TEXT("Failed to delete user: %s"), *ErrorMessage);

			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGFirebaseUser::OnUserOperationErrorCallback.ExecuteIfBound(GetUid(), ErrorMessage);
			});
		}
	});
#endif
}

void UFGFirebaseUser::LinkWithCredentials(UFGAuthCredentials* credentials, const FOnUserUpdated& OnSuccess, const FOnUserOperationError& OnError)
{
	OnUserUpdatedCallback = OnSuccess;
	OnUserOperationErrorCallback = OnError;

	if (!IsUserValid() || !credentials->AreValid())
	{
		return;
	}

#if PLATFORM_ANDROID
	if (credentials != nullptr)
	{
		FGMethodCallUtils::CallStaticVoidMethod(UFGAuthLibrary::FGAuthClassName, "linkUserWithCredential",
			"(Lcom/google/firebase/auth/FirebaseUser;Lcom/google/firebase/auth/AuthCredential;)V",
			JavaFirebaseUserObject, credentials->JavaAuthCredentialObject);
	}
	else
	{
		return;
	}
	
#elif PLATFORM_IOS
	[iosUser linkWithCredential:credentials->iosCredential
					 completion:^(FIRAuthDataResult* _Nullable result, NSError* _Nullable error) {
						 if (error != nil)
						 {
							 FString errorString = FString(error.localizedDescription);
							 AsyncTask(ENamedThreads::GameThread, [=]() {
								 UFGFirebaseUser::OnUserOperationErrorCallback.ExecuteIfBound(GetUid(), errorString);
							 });
							 return;
						 }

						 UFGFirebaseUser* user = NewObject<UFGFirebaseUser>();
						 user->Init(result.user);

						 AsyncTask(ENamedThreads::GameThread, [=]() {
							 UFGFirebaseUser::OnUserUpdatedCallback.ExecuteIfBound(user);
						 });
					 }];
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	auto task = desktopUser->LinkWithCredential(credentials->desktopCredential);
	task.AddOnCompletion([=](const ::firebase::Future<firebase::auth::User*>& callback) {
		if (callback.status() == firebase::FutureStatus::kFutureStatusComplete)
		{
			UFGFirebaseUser* user = NewObject<UFGFirebaseUser>();
			user->Init(*callback.result());

			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGFirebaseUser::OnUserUpdatedCallback.ExecuteIfBound(user);
			});
		}
		else
		{
			FString ErrorMessage = FString(callback.error_message());
			UE_LOG(LogFirebaseGoodies, Error, TEXT("Link With Credentials error: %s"), *ErrorMessage);

			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGFirebaseUser::OnUserOperationErrorCallback.ExecuteIfBound(GetUid(), ErrorMessage);
			});
		}
	});
#endif
}

void UFGFirebaseUser::Reauthenticate(UFGAuthCredentials* credentials, const FOnUserVoidTaskCompleted& OnSuccess, const FOnUserOperationError& OnError)
{
	OnUserVoidTaskCompletedCallback = OnSuccess;
	OnUserOperationErrorCallback = OnError;

	if (!IsUserValid() || !credentials->AreValid())
	{
		return;
	}

#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(UFGAuthLibrary::FGAuthClassName, "reauthenticateUser",
		"(Lcom/google/firebase/auth/FirebaseUser;Lcom/google/firebase/auth/AuthCredential;)V",
		JavaFirebaseUserObject, credentials->JavaAuthCredentialObject);
#elif PLATFORM_IOS
	[iosUser reauthenticateWithCredential:credentials->iosCredential
							   completion:^(FIRAuthDataResult* _Nullable result, NSError* _Nullable error) {
								   if (error != nil)
								   {
									   FString errorString = FString(error.localizedDescription);
									   AsyncTask(ENamedThreads::GameThread, [=]() {
										   UFGFirebaseUser::OnUserOperationErrorCallback.ExecuteIfBound(GetUid(), errorString);
									   });
									   return;
								   }

								   AsyncTask(ENamedThreads::GameThread, [=]() {
									   UFGFirebaseUser::OnUserVoidTaskCompletedCallback.ExecuteIfBound(GetUid());
								   });
							   }];
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	auto task = desktopUser->Reauthenticate(credentials->desktopCredential);
	task.AddOnCompletion([=](const ::firebase::Future<void>& callback) {
		if (callback.status() == firebase::FutureStatus::kFutureStatusComplete)
		{
			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGFirebaseUser::OnUserVoidTaskCompletedCallback.ExecuteIfBound(GetUid());
			});
		}
		else
		{
			FString ErrorMessage = FString(callback.error_message());
			UE_LOG(LogFirebaseGoodies, Error, TEXT("Failed to reauthenticate user: %s"), *ErrorMessage);

			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGFirebaseUser::OnUserOperationErrorCallback.ExecuteIfBound(GetUid(), ErrorMessage);
			});
		}
	});
#endif
}

void UFGFirebaseUser::Reload(const FOnUserVoidTaskCompleted& OnSuccess, const FOnUserOperationError& OnError)
{
	OnUserVoidTaskCompletedCallback = OnSuccess;
	OnUserOperationErrorCallback = OnError;

	if (!IsUserValid())
	{
		return;
	}

#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(UFGAuthLibrary::FGAuthClassName, "reloadUser", "(Lcom/google/firebase/auth/FirebaseUser;)V", JavaFirebaseUserObject);
#elif PLATFORM_IOS
	[iosUser reloadWithCompletion:^(NSError* _Nullable error) {
		if (error != nil)
		{
			FString errorString = FString(error.localizedDescription);
			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGFirebaseUser::OnUserOperationErrorCallback.ExecuteIfBound(GetUid(), errorString);
			});
			return;
		}

		AsyncTask(ENamedThreads::GameThread, [=]() {
			UFGFirebaseUser::OnUserVoidTaskCompletedCallback.ExecuteIfBound(GetUid());
		});
	}];
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	auto task = desktopUser->Reload();
	task.AddOnCompletion([=](const ::firebase::Future<void>& callback) {
		if (callback.status() == firebase::FutureStatus::kFutureStatusComplete)
		{
			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGFirebaseUser::OnUserVoidTaskCompletedCallback.ExecuteIfBound(GetUid());
			});
		}
		else
		{
			FString ErrorMessage = FString(callback.error_message());
			UE_LOG(LogFirebaseGoodies, Error, TEXT("Failed to reload user: %s"), *ErrorMessage);

			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGFirebaseUser::OnUserOperationErrorCallback.ExecuteIfBound(GetUid(), ErrorMessage);
			});
		}
	});
#endif
}

void UFGFirebaseUser::SendEmailVerification(const FOnUserVoidTaskCompleted& OnSuccess, const FOnUserOperationError& OnError)
{
	OnUserVoidTaskCompletedCallback = OnSuccess;
	OnUserOperationErrorCallback = OnError;

	if (!IsUserValid())
	{
		return;
	}

#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(UFGAuthLibrary::FGAuthClassName, "sendEmailVerificationForUser", "(Lcom/google/firebase/auth/FirebaseUser;)V", JavaFirebaseUserObject);
#elif PLATFORM_IOS
	[iosUser sendEmailVerificationWithCompletion:^(NSError* _Nullable error) {
		if (error != nil)
		{
			FString errorString = FString(error.localizedDescription);
			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGFirebaseUser::OnUserOperationErrorCallback.ExecuteIfBound(GetUid(), errorString);
			});
			return;
		}

		AsyncTask(ENamedThreads::GameThread, [=]() {
			UFGFirebaseUser::OnUserVoidTaskCompletedCallback.ExecuteIfBound(GetUid());
		});
	}];
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	auto task = desktopUser->SendEmailVerification();
	task.AddOnCompletion([=](const ::firebase::Future<void>& callback) {
		if (callback.status() == firebase::FutureStatus::kFutureStatusComplete)
		{
			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGFirebaseUser::OnUserVoidTaskCompletedCallback.ExecuteIfBound(GetUid());
			});
		}
		else
		{
			FString ErrorMessage = FString(callback.error_message());
			UE_LOG(LogFirebaseGoodies, Error, TEXT("Failed to Send Email Verification for user: %s"), *ErrorMessage);

			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGFirebaseUser::OnUserOperationErrorCallback.ExecuteIfBound(GetUid(), ErrorMessage);
			});
		}
	});
#endif
}

void UFGFirebaseUser::UnlinkProvider(const FString& provider, const FOnUserUpdated& OnSuccess, const FOnUserOperationError& OnError)
{
	OnUserUpdatedCallback = OnSuccess;
	OnUserOperationErrorCallback = OnError;

	if (!IsUserValid())
	{
		return;
	}

#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(UFGAuthLibrary::FGAuthClassName, "unlinkProviderForUser",
		"(Lcom/google/firebase/auth/FirebaseUser;Ljava/lang/String;)V", JavaFirebaseUserObject, *FJavaClassObject::GetJString(provider));
#elif PLATFORM_IOS
	[iosUser unlinkFromProvider:provider.GetNSString()
					 completion:^(FIRUser* _Nullable result, NSError* _Nullable error) {
						 if (error != nil)
						 {
							 AsyncTask(ENamedThreads::GameThread, [=]() {
								 UFGFirebaseUser::OnUserOperationErrorCallback.ExecuteIfBound(GetUid(), FString(error.localizedDescription));
							 });
							 return;
						 }

						 UFGFirebaseUser* user = NewObject<UFGFirebaseUser>();
						 user->Init(result);

						 AsyncTask(ENamedThreads::GameThread, [=]() {
							 UFGFirebaseUser::OnUserUpdatedCallback.ExecuteIfBound(user);
						 });
					 }];
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	auto task = desktopUser->Unlink(TCHAR_TO_ANSI(*provider));
	task.AddOnCompletion([=](const ::firebase::Future<firebase::auth::User*>& callback) {
		if (callback.status() == firebase::FutureStatus::kFutureStatusComplete)
		{
			UFGFirebaseUser* user = NewObject<UFGFirebaseUser>();
			user->Init(*callback.result());

			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGFirebaseUser::OnUserUpdatedCallback.ExecuteIfBound(user);
			});
		}
		else
		{
			FString ErrorMessage = FString(callback.error_message());
			UE_LOG(LogFirebaseGoodies, Error, TEXT("Link With Credentials error: %s"), *ErrorMessage);

			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGFirebaseUser::OnUserOperationErrorCallback.ExecuteIfBound(GetUid(), ErrorMessage);
			});
		}
	});
#endif
}

void UFGFirebaseUser::UpdateEmail(const FString& email, const FOnUserVoidTaskCompleted& OnSuccess, const FOnUserOperationError& OnError)
{
	OnUserVoidTaskCompletedCallback = OnSuccess;
	OnUserOperationErrorCallback = OnError;

	if (!IsUserValid())
	{
		return;
	}

#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(UFGAuthLibrary::FGAuthClassName, "updateEmailForUser",
		"(Lcom/google/firebase/auth/FirebaseUser;Ljava/lang/String;)V", JavaFirebaseUserObject, *FJavaClassObject::GetJString(email));
#elif PLATFORM_IOS
	[iosUser updateEmail:email.GetNSString()
			  completion:^(NSError* _Nullable error) {
				  if (error != nil)
				  {
					  FString errorString = FString(error.localizedDescription);
					  AsyncTask(ENamedThreads::GameThread, [=]() {
						  UFGFirebaseUser::OnUserOperationErrorCallback.ExecuteIfBound(GetUid(), errorString);
					  });
					  return;
				  }

				  AsyncTask(ENamedThreads::GameThread, [=]() {
					  UFGFirebaseUser::OnUserVoidTaskCompletedCallback.ExecuteIfBound(GetUid());
				  });
			  }];
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	auto task = desktopUser->UpdateEmail(TCHAR_TO_ANSI(*email));
	task.AddOnCompletion([=](const ::firebase::Future<void>& callback) {
		if (callback.status() == firebase::FutureStatus::kFutureStatusComplete)
		{
			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGFirebaseUser::OnUserVoidTaskCompletedCallback.ExecuteIfBound(GetUid());
			});
		}
		else
		{
			FString ErrorMessage = FString(callback.error_message());
			UE_LOG(LogFirebaseGoodies, Error, TEXT("Failed to Send Email Verification for user: %s"), *ErrorMessage);

			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGFirebaseUser::OnUserOperationErrorCallback.ExecuteIfBound(GetUid(), ErrorMessage);
			});
		}
	});
#endif
}

void UFGFirebaseUser::UpdatePassword(const FString& password, const FOnUserVoidTaskCompleted& OnSuccess, const FOnUserOperationError& OnError)
{
	OnUserVoidTaskCompletedCallback = OnSuccess;
	OnUserOperationErrorCallback = OnError;

	if (!IsUserValid())
	{
		return;
	}

#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(UFGAuthLibrary::FGAuthClassName, "updatePasswordForUser",
		"(Lcom/google/firebase/auth/FirebaseUser;Ljava/lang/String;)V", JavaFirebaseUserObject, *FJavaClassObject::GetJString(password));
#elif PLATFORM_IOS
	[iosUser updatePassword:password.GetNSString()
				 completion:^(NSError* _Nullable error) {
					 if (error != nil)
					 {
						 FString errorString = FString(error.localizedDescription);
						 AsyncTask(ENamedThreads::GameThread, [=]() {
							 UFGFirebaseUser::OnUserOperationErrorCallback.ExecuteIfBound(GetUid(), errorString);
						 });
						 return;
					 }

					 AsyncTask(ENamedThreads::GameThread, [=]() {
						 UFGFirebaseUser::OnUserVoidTaskCompletedCallback.ExecuteIfBound(GetUid());
					 });
				 }];
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	auto task = desktopUser->UpdatePassword(TCHAR_TO_ANSI(*password));
	task.AddOnCompletion([=](const ::firebase::Future<void>& callback) {
		if (callback.status() == firebase::FutureStatus::kFutureStatusComplete)
		{
			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGFirebaseUser::OnUserVoidTaskCompletedCallback.ExecuteIfBound(GetUid());
			});
		}
		else
		{
			FString ErrorMessage = FString(callback.error_message());
			UE_LOG(LogFirebaseGoodies, Error, TEXT("Failed to Send Email Verification for user: %s"), *ErrorMessage);

			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGFirebaseUser::OnUserOperationErrorCallback.ExecuteIfBound(GetUid(), ErrorMessage);
			});
		}
	});
#endif
}

void UFGFirebaseUser::UpdatePhoneNumber(UFGAuthCredentials* credentials, const FOnUserVoidTaskCompleted& OnSuccess,
	const FOnUserOperationError& OnError)
{
	OnUserVoidTaskCompletedCallback = OnSuccess;
	OnUserOperationErrorCallback = OnError;

	if (!IsUserValid() || !credentials->AreValid())
	{
		return;
	}

#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(UFGAuthLibrary::FGAuthClassName, "updatePhoneNumberForUser",
		"(Lcom/google/firebase/auth/FirebaseUser;Lcom/google/firebase/auth/AuthCredential;)V",
		JavaFirebaseUserObject, credentials->JavaAuthCredentialObject);
#elif PLATFORM_IOS
	[iosUser updatePhoneNumberCredential:(FIRPhoneAuthCredential*)credentials->iosCredential
							  completion:^(NSError* _Nullable error) {
								  if (error != nil)
								  {
									  FString errorString = FString(error.localizedDescription);
									  AsyncTask(ENamedThreads::GameThread, [=]() {
										  UFGFirebaseUser::OnUserOperationErrorCallback.ExecuteIfBound(GetUid(), errorString);
									  });
									  return;
								  }

								  AsyncTask(ENamedThreads::GameThread, [=]() {
									  UFGFirebaseUser::OnUserVoidTaskCompletedCallback.ExecuteIfBound(GetUid());
								  });
							  }];
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	auto task = desktopUser->UpdatePhoneNumberCredential(credentials->desktopCredential);
	task.AddOnCompletion([=](const ::firebase::Future<firebase::auth::User*>& callback) {
		if (callback.status() == firebase::FutureStatus::kFutureStatusComplete)
		{
			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGFirebaseUser::OnUserVoidTaskCompletedCallback.ExecuteIfBound(GetUid());
			});
		}
		else
		{
			FString ErrorMessage = FString(callback.error_message());
			UE_LOG(LogFirebaseGoodies, Error, TEXT("Failed to Send Email Verification for user: %s"), *ErrorMessage);

			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGFirebaseUser::OnUserOperationErrorCallback.ExecuteIfBound(GetUid(), ErrorMessage);
			});
		}
	});
#endif
}

void UFGFirebaseUser::UpdateProfile(const FString& displayName, const FString& avatarUrl, const FOnUserVoidTaskCompleted& OnSuccess,
	const FOnUserOperationError& OnError)
{
	OnUserVoidTaskCompletedCallback = OnSuccess;
	OnUserOperationErrorCallback = OnError;

	if (!IsUserValid())
	{
		return;
	}

#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(UFGAuthLibrary::FGAuthClassName, "updateProfileForUser",
		"(Lcom/google/firebase/auth/FirebaseUser;Ljava/lang/String;Ljava/lang/String;)V",
		JavaFirebaseUserObject, *FJavaClassObject::GetJString(displayName), *FJavaClassObject::GetJString(avatarUrl));
#elif PLATFORM_IOS
	FIRUserProfileChangeRequest* request = iosUser.profileChangeRequest;
	request.displayName = displayName.GetNSString();
	request.photoURL = [NSURL URLWithString:avatarUrl.GetNSString()];
	[request commitChangesWithCompletion:^(NSError* _Nullable error) {
		if (error != nil)
		{
			FString errorString = FString(error.localizedDescription);
			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGFirebaseUser::OnUserOperationErrorCallback.ExecuteIfBound(GetUid(), errorString);
			});
			return;
		}

		AsyncTask(ENamedThreads::GameThread, [=]() {
			UFGFirebaseUser::OnUserVoidTaskCompletedCallback.ExecuteIfBound(GetUid());
		});
	}];
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	firebase::auth::User::UserProfile profile;
	profile.display_name = TCHAR_TO_ANSI(*displayName);
	profile.photo_url = TCHAR_TO_ANSI(*avatarUrl);

	auto task = desktopUser->UpdateUserProfile(profile);
	task.AddOnCompletion([=](const ::firebase::Future<void>& callback) {
		if (callback.status() == firebase::FutureStatus::kFutureStatusComplete)
		{
			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGFirebaseUser::OnUserVoidTaskCompletedCallback.ExecuteIfBound(GetUid());
			});
		}
		else
		{
			FString ErrorMessage = FString(callback.error_message());
			UE_LOG(LogFirebaseGoodies, Error, TEXT("Failed to Send Email Verification for user: %s"), *ErrorMessage);

			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGFirebaseUser::OnUserOperationErrorCallback.ExecuteIfBound(GetUid(), ErrorMessage);
			});
		}
	});
#endif
}

void UFGFirebaseUser::GetToken(bool ForceRefresh, const FOnUserStringTaskCompleted& OnSuccess, const FOnUserOperationError& OnError)
{
	OnUserStringTaskCompletedCallback = OnSuccess;
	OnUserOperationErrorCallback = OnError;
	
	if (!IsUserValid())
	{
		return;
	}
	
#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(UFGAuthLibrary::FGAuthClassName, "getUserToken",
											"(Lcom/google/firebase/auth/FirebaseUser;Z)V",
											JavaFirebaseUserObject, ForceRefresh);
#elif PLATFORM_IOS
	[iosUser getIDTokenForcingRefresh:ForceRefresh
						   completion:^(NSString* token, NSError* error) {
		if (error != nil)
		{
			FString errorString = FString(error.localizedDescription);
			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGFirebaseUser::OnUserOperationErrorCallback.ExecuteIfBound(GetUid(), errorString);
			});
			return;
		}
		
		FString TokenString = FString(token);
		AsyncTask(ENamedThreads::GameThread, [=]() {
			UFGFirebaseUser::OnUserStringTaskCompletedCallback.ExecuteIfBound(GetUid(), TokenString);
		});
	}];
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	auto task = desktopUser->GetToken(ForceRefresh);
	task.AddOnCompletion([=](const ::firebase::Future<std::string>& callback) {
		if (callback.status() == firebase::FutureStatus::kFutureStatusComplete)
		{
			FString Token = (*callback.result()).c_str();
			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGFirebaseUser::OnUserStringTaskCompletedCallback.ExecuteIfBound(GetUid(), Token);
			});
		}
		else
		{
			FString ErrorMessage = FString(callback.error_message());
			UE_LOG(LogFirebaseGoodies, Error, TEXT("Failed to Send Email Verification for user: %s"), *ErrorMessage);

			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGFirebaseUser::OnUserOperationErrorCallback.ExecuteIfBound(GetUid(), ErrorMessage);
			});
		}
	});
#endif
}

TArray<UFGFirebaseUserInfo*> UFGFirebaseUser::GetProviderData()
{
	TArray<UFGFirebaseUserInfo*> Result;
	
#if PLATFORM_ANDROID
	jobject javaArrayList = FGMethodCallUtils::CallStaticObjectMethod(UFGAuthLibrary::FGAuthClassName, "getUserProviderData",
											"(Lcom/google/firebase/auth/FirebaseUser;)Ljava/util/ArrayList;",
											JavaFirebaseUserObject);
	
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	
	jclass ArrayListClass = FJavaWrapper::FindClass(Env, "java/util/ArrayList", false);
	jmethodID SizeMethod = FJavaWrapper::FindMethod(Env, ArrayListClass, "size", "()I", false);
	jmethodID GetMethod = FJavaWrapper::FindMethod(Env, ArrayListClass, "get", "(I)Ljava/lang/Object;", false);
	
	const int Size = Env->CallIntMethod(javaArrayList, SizeMethod);
	
	for (int i = 0; i < Size; i++)
	{
		jobject Item = Env->CallObjectMethod(javaArrayList, GetMethod, i);
		UFGFirebaseUserInfo* Info = NewObject<UFGFirebaseUserInfo>();
		Info->InitUserInfo(Item);
		Result.Add(Info);
	}
#elif PLATFORM_IOS
	for (id<FIRUserInfo> info in iosUser.providerData) {
		UFGFirebaseUserInfo* Info = NewObject<UFGFirebaseUserInfo>();
		Info->InitUserInfo(info);
		Result.Add(Info);
	}
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	for (firebase::auth::User::UserInfoInterface* provider : desktopUser->provider_data()) {
		UFGFirebaseUserInfo* Info = NewObject<UFGFirebaseUserInfo>();
		Info->InitUserInfo(provider);
		Result.Add(Info);
	}
#endif
	
	return Result;
}

#if PLATFORM_ANDROID
void UFGFirebaseUser::Init(jobject javaUser)
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	JavaFirebaseUserObject = Env->NewGlobalRef(javaUser);

	InitUserInfo(javaUser);
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGAuth_OnUserVoidTaskCompleted(JNIEnv* env, jclass clazz, jstring uid)
{
	FString result = FGJavaConvertor::FromJavaString(uid);

	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGFirebaseUser::OnUserVoidTaskCompletedCallback.ExecuteIfBound(result);
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGAuth_OnUserStringTaskCompleted(JNIEnv* env, jclass clazz, jstring uid, jstring result)
{
	FString UserId = FGJavaConvertor::FromJavaString(uid);
	FString Result = FGJavaConvertor::FromJavaString(result);
	
	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGFirebaseUser::OnUserStringTaskCompletedCallback.ExecuteIfBound(UserId, Result);
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGAuth_OnUserUpdated(JNIEnv* env, jclass clazz, jobject user)
{
	UFGFirebaseUser* userObject = NewObject<UFGFirebaseUser>();
	userObject->Init(user);

	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGFirebaseUser::OnUserUpdatedCallback.ExecuteIfBound(userObject);
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGAuth_OnUserOperationError(JNIEnv* env, jclass clazz, jstring uid, jstring error)
{
	FString uidString = FGJavaConvertor::FromJavaString(uid);
	FString errorString = FGJavaConvertor::FromJavaString(error);

	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGFirebaseUser::OnUserOperationErrorCallback.ExecuteIfBound(uidString, errorString);
	});
}
#endif

#if PLATFORM_IOS
void UFGFirebaseUser::Init(FIRUser* user)
{
	iosUser = (FIRUser*)CFBridgingRetain(user);

	InitUserInfo(user);
}
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
void UFGFirebaseUser::Init(firebase::auth::User* user)
{
	desktopUser = user;

	InitUserInfo(user);
}
#endif
