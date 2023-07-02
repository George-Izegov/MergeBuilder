// Copyright (c) 2022 Nineva Studios

#include "Auth/FGAuthLibrary.h"

#include "Async/Async.h"
#include "FirebaseGoodies.h"
#include "FirebaseGoodiesLog.h"
#include "FirebaseGoodiesSettings.h"

#if PLATFORM_ANDROID
#include "Android/Utils/FGJavaConvertor.h"
#include "Android/Utils/FGMethodCallUtils.h"
#endif

#if PLATFORM_IOS
#import "IOS/IOSAppDelegate.h"

#import <FirebaseAuth/FirebaseAuth.h>
#import <GoogleSignIn/GoogleSignIn.h>
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
#include "firebase/auth.h"
#include "firebase/app.h"

#include "Desktop/Auth/FGAuthStateListener.h"
#include "Desktop/Auth/FGIdTokenListener.h"
#include "Desktop/Auth/FGPhoneListener.h"
#endif

FAuthVoidDelegate UFGAuthLibrary::IdTokenChangedCallback;
FAuthVoidDelegate UFGAuthLibrary::AuthStateChangedCallback;
FAuthVoidDelegate UFGAuthLibrary::PasswordResetSuccessCallback;
FAuthUserDelegate UFGAuthLibrary::UserCreateSuccessCallback;
FAuthUserDelegate UFGAuthLibrary::SignInSuccessCallback;
FAuthStringDelegate UFGAuthLibrary::AuthErrorCallback;
FSignInMethodsFetchedDelegate UFGAuthLibrary::SignInMethodsFetchedCallback;
FAuthCredentialsDelegate UFGAuthLibrary::PhoneVerificationSuccessCallback;
FAuthStringDelegate UFGAuthLibrary::PhoneVerificationTimeoutCallback;
FAuthStringDelegate UFGAuthLibrary::PhoneVerificationSentCallback;
FAuthCredentialsDelegate UFGAuthLibrary::SignInWithGoogleSuccessCallback;

const ANSICHAR* UFGAuthLibrary::FGAuthClassName = "com/ninevastudios/unrealfirebase/FGAuth";

void UFGAuthLibrary::InitListeners(const FAuthVoidDelegate& IdTokenChangedDelegate, const FAuthVoidDelegate& AuthStateChangedDelegate)
{
	IdTokenChangedCallback = IdTokenChangedDelegate;
	AuthStateChangedCallback = AuthStateChangedDelegate;

#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(FGAuthClassName, "initListeners", "()V");
#elif PLATFORM_IOS
	[[FIRAuth auth] addAuthStateDidChangeListener:^(FIRAuth* _Nonnull auth, FIRUser* _Nullable user) {
		AsyncTask(ENamedThreads::GameThread, [=]() {
			UFGAuthLibrary::AuthStateChangedCallback.ExecuteIfBound();
		});
	}];

	[[FIRAuth auth] addIDTokenDidChangeListener:^(FIRAuth* _Nonnull auth, FIRUser* _Nullable user) {
		AsyncTask(ENamedThreads::GameThread, [=]() {
			UFGAuthLibrary::IdTokenChangedCallback.ExecuteIfBound();
		});
	}];

#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	firebase::auth::Auth* auth = firebase::auth::Auth::GetAuth(firebase::App::GetInstance());

	auth->AddAuthStateListener(new FGAuthStateListener());
	auth->AddIdTokenListener(new FGIdTokenListener());
#endif
}

UFGFirebaseUser* UFGAuthLibrary::CurrentUser()
{
	UFGFirebaseUser* user = NewObject<UFGFirebaseUser>();
#if PLATFORM_ANDROID
	jobject javaUser = FGMethodCallUtils::CallStaticObjectMethod(FGAuthClassName, "currentUser", "()Lcom/google/firebase/auth/FirebaseUser;");
	user->Init(javaUser);
#elif PLATFORM_IOS
	user->Init([[FIRAuth auth] currentUser]);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	firebase::auth::Auth* auth = firebase::auth::Auth::GetAuth(firebase::App::GetInstance());

	user->Init(auth->current_user());
#endif
	return user;
}

void UFGAuthLibrary::CreateUser(const FString& Email, const FString& Password, const FAuthUserDelegate& OnSuccess,
                                const FAuthStringDelegate& OnError)
{
	UserCreateSuccessCallback = OnSuccess;
	AuthErrorCallback = OnError;
#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(FGAuthClassName, "createUser", "(Ljava/lang/String;Ljava/lang/String;Landroid/app/Activity;)V",
		*FJavaClassObject::GetJString(Email), *FJavaClassObject::GetJString(Password), FJavaWrapper::GameActivityThis);
#elif PLATFORM_IOS
	[[FIRAuth auth] createUserWithEmail:Email.GetNSString()
							   password:Password.GetNSString()
							 completion:^(FIRAuthDataResult* _Nullable result, NSError* _Nullable error) {
								 if (error != nil)
								 {
									 FString ErrorMessage = FString(error.localizedDescription);
									 UE_LOG(LogFirebaseGoodies, Error, TEXT("Create user error: %s"), *ErrorMessage);
									 AsyncTask(ENamedThreads::GameThread, [=]() {
										 UFGAuthLibrary::AuthErrorCallback.ExecuteIfBound(ErrorMessage);
									 });
									 return;
								 }

								 UFGFirebaseUser* user = NewObject<UFGFirebaseUser>();
								 user->Init(result.user);

								 AsyncTask(ENamedThreads::GameThread, [=]() {
									 UFGAuthLibrary::UserCreateSuccessCallback.ExecuteIfBound(user);
								 });
							 }];
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	firebase::auth::Auth* auth = firebase::auth::Auth::GetAuth(firebase::App::GetInstance());

	auto task = auth->CreateUserWithEmailAndPassword(TCHAR_TO_ANSI(*Email), TCHAR_TO_ANSI(*Password));
	task.AddOnCompletion([=](const ::firebase::Future<firebase::auth::User*>& callback)
	{
		if (callback.status() == firebase::FutureStatus::kFutureStatusComplete)
		{
			UFGFirebaseUser* user = NewObject<UFGFirebaseUser>();
			user->Init(*callback.result());

			AsyncTask(ENamedThreads::GameThread, [=]()
			{
				UFGAuthLibrary::UserCreateSuccessCallback.ExecuteIfBound(user);
			});
		}
		else
		{
			FString ErrorMessage = FString(callback.error_message());
			UE_LOG(LogFirebaseGoodies, Error, TEXT("Create user error: %s"), *ErrorMessage);

			AsyncTask(ENamedThreads::GameThread, [=]()
			{
				UFGAuthLibrary::AuthErrorCallback.ExecuteIfBound(ErrorMessage);
			});
		}
	});
#endif
}

void UFGAuthLibrary::FetchProvidersForEmail(const FString& Email, const FSignInMethodsFetchedDelegate& OnSuccess,
                                            const FAuthStringDelegate& OnError)
{
	SignInMethodsFetchedCallback = OnSuccess;
	AuthErrorCallback = OnError;
#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(FGAuthClassName, "fetchSignInMethods", "(Ljava/lang/String;)V", *FJavaClassObject::GetJString(Email));
#elif PLATFORM_IOS
	[[FIRAuth auth] fetchSignInMethodsForEmail:Email.GetNSString()
									completion:^(NSArray<NSString*>* _Nullable providers, NSError* _Nullable error) {
										if (error != nil)
										{
											FString ErrorMessage = FString(error.localizedDescription);
											UE_LOG(LogFirebaseGoodies, Error, TEXT("Failed to fetch providers: %s"), *ErrorMessage);
											AsyncTask(ENamedThreads::GameThread, [=]() {
												UFGAuthLibrary::AuthErrorCallback.ExecuteIfBound(ErrorMessage);
											});
											return;
										}

										TArray<FString> fetchedMethods;
										for (NSString* method in providers)
										{
											fetchedMethods.Add(FString(method));
										}

										AsyncTask(ENamedThreads::GameThread, [=]() {
											UFGAuthLibrary::SignInMethodsFetchedCallback.ExecuteIfBound(fetchedMethods);
										});
									}];
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	firebase::auth::Auth* auth = firebase::auth::Auth::GetAuth(firebase::App::GetInstance());

	auto task = auth->FetchProvidersForEmail(TCHAR_TO_ANSI(*Email));
	task.AddOnCompletion([=](const ::firebase::Future<firebase::auth::Auth::FetchProvidersResult>& callback)
	{
		if (callback.status() == firebase::FutureStatus::kFutureStatusComplete)
		{
			TArray<FString> FetchedMethods;

			for (auto provider : (*callback.result()).providers)
			{
				FetchedMethods.Add(provider.c_str());
			}

			AsyncTask(ENamedThreads::GameThread, [=]()
			{
				UFGAuthLibrary::SignInMethodsFetchedCallback.ExecuteIfBound(FetchedMethods);
			});
		}
		else
		{
			FString ErrorMessage = FString(callback.error_message());
			UE_LOG(LogFirebaseGoodies, Error, TEXT("Failed to fetch providers: %s"), *ErrorMessage);

			AsyncTask(ENamedThreads::GameThread, [=]()
			{
				UFGAuthLibrary::AuthErrorCallback.ExecuteIfBound(ErrorMessage);
			});
		}
	});
#endif
}

void UFGAuthLibrary::SendPasswordReset(const FString& Email, const FAuthVoidDelegate& OnSuccess, const FAuthStringDelegate& OnError)
{
	PasswordResetSuccessCallback = OnSuccess;
	AuthErrorCallback = OnError;
#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(FGAuthClassName, "sendPasswordReset", "(Ljava/lang/String;)V", *FJavaClassObject::GetJString(Email));
#elif PLATFORM_IOS
	[[FIRAuth auth] sendPasswordResetWithEmail:Email.GetNSString()
									completion:^(NSError* _Nullable error) {
										if (error != nil)
										{
											FString ErrorMessage = FString(error.localizedDescription);
											UE_LOG(LogFirebaseGoodies, Error, TEXT("Password reset: %s"), *ErrorMessage);
											AsyncTask(ENamedThreads::GameThread, [=]() {
												UFGAuthLibrary::AuthErrorCallback.ExecuteIfBound(ErrorMessage);
											});
											return;
										}

										AsyncTask(ENamedThreads::GameThread, [=]() {
											UFGAuthLibrary::PasswordResetSuccessCallback.ExecuteIfBound();
										});
									}];
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	firebase::auth::Auth* auth = firebase::auth::Auth::GetAuth(firebase::App::GetInstance());

	auto task = auth->SendPasswordResetEmail(TCHAR_TO_ANSI(*Email));
	task.AddOnCompletion([=](const ::firebase::Future<void>& callback)
	{
		if (callback.status() == firebase::FutureStatus::kFutureStatusComplete)
		{
			AsyncTask(ENamedThreads::GameThread, [=]()
			{
				UFGAuthLibrary::PasswordResetSuccessCallback.ExecuteIfBound();
			});
		}
		else
		{
			FString ErrorMessage = FString(callback.error_message());
			UE_LOG(LogFirebaseGoodies, Error, TEXT("Password reset error: %s"), *ErrorMessage);

			AsyncTask(ENamedThreads::GameThread, [=]()
			{
				UFGAuthLibrary::AuthErrorCallback.ExecuteIfBound(ErrorMessage);
			});
		}
	});
#endif
}

#if PLATFORM_IOS
static void HandleSignInCallback(FIRAuthDataResult* result, NSError* error)
{
	if (error != nil)
	{
		FString ErrorMessage = FString(error.localizedDescription);
		UE_LOG(LogFirebaseGoodies, Error, TEXT("Auth error: %s"), *ErrorMessage);
		AsyncTask(ENamedThreads::GameThread, [=]() {
			UFGAuthLibrary::AuthErrorCallback.ExecuteIfBound(ErrorMessage);
		});
		return;
	}

	UFGFirebaseUser* user = NewObject<UFGFirebaseUser>();
	user->Init(result.user);

	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGAuthLibrary::SignInSuccessCallback.ExecuteIfBound(user);
	});
}
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
static void HandleSignInCallback(const ::firebase::Future<firebase::auth::User*>& callback)
{
	if (callback.status() == firebase::FutureStatus::kFutureStatusComplete && *callback.result() != nullptr)
	{
		UFGFirebaseUser* user = NewObject<UFGFirebaseUser>();
		user->Init(*callback.result());

		AsyncTask(ENamedThreads::GameThread, [=]()
		{
			UFGAuthLibrary::SignInSuccessCallback.ExecuteIfBound(user);
		});
	}
	else
	{
		FString ErrorMessage = FString(callback.error_message());
		UE_LOG(LogFirebaseGoodies, Error, TEXT("Auth error: %s"), *ErrorMessage);

		AsyncTask(ENamedThreads::GameThread, [=]()
		{
			UFGAuthLibrary::AuthErrorCallback.ExecuteIfBound(ErrorMessage);
		});
	}
}
#endif

void UFGAuthLibrary::SignInWithEmailAndPassword(const FString& Email, const FString& Password, const FAuthUserDelegate& OnSuccess,
                                                const FAuthStringDelegate& OnError)
{
	SignInSuccessCallback = OnSuccess;
	AuthErrorCallback = OnError;
#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(FGAuthClassName, "signInWithEmail", "(Ljava/lang/String;Ljava/lang/String;)V",
		*FJavaClassObject::GetJString(Email), *FJavaClassObject::GetJString(Password));
#elif PLATFORM_IOS
	[[FIRAuth auth] signInWithEmail:Email.GetNSString()
						   password:Password.GetNSString()
						 completion:^(FIRAuthDataResult* _Nullable result, NSError* _Nullable error) {
							 HandleSignInCallback(result, error);
						 }];
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	firebase::auth::Auth* auth = firebase::auth::Auth::GetAuth(firebase::App::GetInstance());

	auth->SignInWithEmailAndPassword(TCHAR_TO_ANSI(*Email), TCHAR_TO_ANSI(*Password)).AddOnCompletion(HandleSignInCallback);
#endif
}

void UFGAuthLibrary::SignInWithToken(const FString& Token, const FAuthUserDelegate& OnSuccess, const FAuthStringDelegate& OnError)
{
	SignInSuccessCallback = OnSuccess;
	AuthErrorCallback = OnError;
#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(FGAuthClassName, "signInWithToken", "(Ljava/lang/String;)V", *FJavaClassObject::GetJString(Token));
#elif PLATFORM_IOS
	[[FIRAuth auth] signInWithCustomToken:Token.GetNSString()
							   completion:^(FIRAuthDataResult* _Nullable result, NSError* _Nullable error) {
								   HandleSignInCallback(result, error);
							   }];
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	firebase::auth::Auth* auth = firebase::auth::Auth::GetAuth(firebase::App::GetInstance());

	auth->SignInWithCustomToken(TCHAR_TO_ANSI(*Token)).AddOnCompletion(HandleSignInCallback);
#endif
}

void UFGAuthLibrary::SignInAnonymously(const FAuthUserDelegate& OnSuccess, const FAuthStringDelegate& OnError)
{
	SignInSuccessCallback = OnSuccess;
	AuthErrorCallback = OnError;
#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(FGAuthClassName, "signInAnonymously", "()V");
#elif PLATFORM_IOS
	[[FIRAuth auth] signInAnonymouslyWithCompletion:^(FIRAuthDataResult* _Nullable result, NSError* _Nullable error) {
		HandleSignInCallback(result, error);
	}];
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	firebase::auth::Auth* auth = firebase::auth::Auth::GetAuth(firebase::App::GetInstance());

	auth->SignInAnonymously().AddOnCompletion(HandleSignInCallback);
#endif
}

void UFGAuthLibrary::SignInWithCredentials(UFGAuthCredentials* Credentials, const FAuthUserDelegate& OnSuccess,
                                           const FAuthStringDelegate& OnError)
{
	SignInSuccessCallback = OnSuccess;
	AuthErrorCallback = OnError;

	if (!Credentials->AreValid())
	{
		return;
	}

#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(FGAuthClassName, "signInWithCredential", "(Lcom/google/firebase/auth/AuthCredential;)V", Credentials->JavaAuthCredentialObject);
#elif PLATFORM_IOS
	[[FIRAuth auth] signInWithCredential:Credentials->iosCredential
							  completion:^(FIRAuthDataResult* _Nullable result, NSError* _Nullable error) {
								  HandleSignInCallback(result, error);
							  }];
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	firebase::auth::Auth* auth = firebase::auth::Auth::GetAuth(firebase::App::GetInstance());

	auth->SignInWithCredential(Credentials->desktopCredential).AddOnCompletion(HandleSignInCallback);
#endif
}

void UFGAuthLibrary::SignOut()
{
#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(FGAuthClassName, "signOut", "()V");
#elif PLATFORM_IOS
	[[FIRAuth auth] signOut:nil];
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	firebase::auth::Auth* auth = firebase::auth::Auth::GetAuth(firebase::App::GetInstance());

	auth->SignOut();
#endif
}

UFGAuthCredentials* UFGAuthLibrary::GetEmailCredentials(const FString& Email, const FString& Password)
{
	if (Email.IsEmpty() || Password.IsEmpty())
	{
		UE_LOG(LogFirebaseGoodies, Error, TEXT("Empty string! \n Check email & password!"));
		return nullptr;
	}

	UFGAuthCredentials* credentials = NewObject<UFGAuthCredentials>();

#if PLATFORM_ANDROID
	jobject javaCredentials = FGMethodCallUtils::CallStaticObjectMethod(FGAuthClassName, "getEmailCredential",
		"(Ljava/lang/String;Ljava/lang/String;)Lcom/google/firebase/auth/AuthCredential;", *FJavaClassObject::GetJString(Email), *FJavaClassObject::GetJString(Password));
	credentials->Init(javaCredentials);
#elif PLATFORM_IOS
	FIRAuthCredential* iosCredential = [FIREmailAuthProvider credentialWithEmail:Email.GetNSString()
																		password:Password.GetNSString()];
	credentials->Init(iosCredential);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	firebase::auth::Credential credential = firebase::auth::EmailAuthProvider::GetCredential(TCHAR_TO_ANSI(*Email), TCHAR_TO_ANSI(*Password));
	credentials->Init(credential);
#endif

	return credentials;
}

UFGAuthCredentials* UFGAuthLibrary::GetFacebookCredentials(const FString& Token)
{
	if (Token.IsEmpty())
	{
		UE_LOG(LogFirebaseGoodies, Error, TEXT("Empty string! \n Check token!"));
		return nullptr;
	}

	UFGAuthCredentials* credentials = NewObject<UFGAuthCredentials>();

#if PLATFORM_ANDROID
	jobject javaCredentials = FGMethodCallUtils::CallStaticObjectMethod(FGAuthClassName, "getFacebookCredential",
		"(Ljava/lang/String;)Lcom/google/firebase/auth/AuthCredential;", *FJavaClassObject::GetJString(Token));
	credentials->Init(javaCredentials);
#elif PLATFORM_IOS
	FIRAuthCredential* iosCredential = [FIRFacebookAuthProvider credentialWithAccessToken:Token.GetNSString()];
	credentials->Init(iosCredential);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	firebase::auth::Credential credential = firebase::auth::FacebookAuthProvider::GetCredential(TCHAR_TO_ANSI(*Token));
	credentials->Init(credential);
#endif

	return credentials;
}

UFGAuthCredentials* UFGAuthLibrary::GetGithubCredentials(const FString& Token)
{
	if (Token.IsEmpty())
	{
		UE_LOG(LogFirebaseGoodies, Error, TEXT("Empty string! \n Check token!"));
		return nullptr;
	}

	UFGAuthCredentials* credentials = NewObject<UFGAuthCredentials>();

#if PLATFORM_ANDROID
	jobject javaCredentials = FGMethodCallUtils::CallStaticObjectMethod(FGAuthClassName, "getGithubCredential",
		"(Ljava/lang/String;)Lcom/google/firebase/auth/AuthCredential;", *FJavaClassObject::GetJString(Token));
	credentials->Init(javaCredentials);
#elif PLATFORM_IOS
	FIRAuthCredential* iosCredential = [FIRGitHubAuthProvider credentialWithToken:Token.GetNSString()];
	credentials->Init(iosCredential);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	firebase::auth::Credential credential = firebase::auth::GitHubAuthProvider::GetCredential(TCHAR_TO_ANSI(*Token));
	credentials->Init(credential);
#endif

	return credentials;
}

UFGAuthCredentials* UFGAuthLibrary::GetGoogleCredentials(const FString& IdToken, const FString& AccessToken)
{
	if (IdToken.IsEmpty() || AccessToken.IsEmpty())
	{
		UE_LOG(LogFirebaseGoodies, Error, TEXT("Empty string! \n Check tokens!"));
		return nullptr;
	}

	UFGAuthCredentials* credentials = NewObject<UFGAuthCredentials>();

#if PLATFORM_ANDROID
	jobject javaCredentials = FGMethodCallUtils::CallStaticObjectMethod(FGAuthClassName, "getGoogleCredential",
		"(Ljava/lang/String;Ljava/lang/String;)Lcom/google/firebase/auth/AuthCredential;",
		*FJavaClassObject::GetJString(IdToken), *FJavaClassObject::GetJString(AccessToken));
	credentials->Init(javaCredentials);
#elif PLATFORM_IOS
	FIRAuthCredential* iosCredential = [FIRGoogleAuthProvider credentialWithIDToken:IdToken.GetNSString()
																		accessToken:AccessToken.GetNSString()];
	credentials->Init(iosCredential);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	firebase::auth::Credential credential = firebase::auth::GoogleAuthProvider::GetCredential(TCHAR_TO_ANSI(*IdToken), TCHAR_TO_ANSI(*AccessToken));
	credentials->Init(credential);
#endif

	return credentials;
}

UFGAuthCredentials* UFGAuthLibrary::GetAppleCredentials(const FString& IdToken, const FString& RawNonce)
{
	if (IdToken.IsEmpty() || RawNonce.IsEmpty())
	{
		UE_LOG(LogFirebaseGoodies, Error, TEXT("Empty string! \n Check token or nonce!"));
		return nullptr;
	}

	UFGAuthCredentials* credentials = NewObject<UFGAuthCredentials>();

#if PLATFORM_ANDROID
	// TODO is it available on Android?
#elif PLATFORM_IOS
	FIRAuthCredential *iosCredential  = [FIROAuthProvider credentialWithProviderID:@"apple.com"
                                                                           IDToken:IdToken.GetNSString()
                                                                          rawNonce:RawNonce.GetNSString()];
	credentials->Init(iosCredential);
#endif

	return credentials;
}

UFGAuthCredentials* UFGAuthLibrary::GetGenericOAuthCredentials(const FString& Provider, const FString& IdToken, const FString& AccessToken)
{
	if (Provider.IsEmpty() || IdToken.IsEmpty() || AccessToken.IsEmpty())
	{
		UE_LOG(LogFirebaseGoodies, Error, TEXT("Empty string! \n Check provider and tokens!"));
		return nullptr;
	}

	UFGAuthCredentials* credentials = NewObject<UFGAuthCredentials>();

#if PLATFORM_ANDROID
	jobject javaCredentials = FGMethodCallUtils::CallStaticObjectMethod(FGAuthClassName, "getGenericOAuthCredential",
		"(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Lcom/google/firebase/auth/AuthCredential;",
		*FJavaClassObject::GetJString(Provider), *FJavaClassObject::GetJString(IdToken), *FJavaClassObject::GetJString(AccessToken));
	credentials->Init(javaCredentials);
#elif PLATFORM_IOS
	FIRAuthCredential* iosCredential = [FIROAuthProvider credentialWithProviderID:Provider.GetNSString()
																		  IDToken:IdToken.GetNSString()
																	  accessToken:AccessToken.GetNSString()];
	credentials->Init(iosCredential);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	firebase::auth::Credential credential = firebase::auth::OAuthProvider::GetCredential(TCHAR_TO_ANSI(*Provider), TCHAR_TO_ANSI(*IdToken), TCHAR_TO_ANSI(*AccessToken));
	credentials->Init(credential);
#endif

	return credentials;
}

UFGAuthCredentials* UFGAuthLibrary::GetPlayGamesCredentials(const FString& ServerAuthCode)
{
	if (ServerAuthCode.IsEmpty())
	{
		UE_LOG(LogFirebaseGoodies, Error, TEXT("Empty string! \n Check server authorization code!"));
		return nullptr;
	}

	UFGAuthCredentials* credentials = NewObject<UFGAuthCredentials>();

#if PLATFORM_ANDROID
	jobject javaCredentials = FGMethodCallUtils::CallStaticObjectMethod(FGAuthClassName, "getPlayGamesCredential",
		"(Ljava/lang/String;)Lcom/google/firebase/auth/AuthCredential;", *FJavaClassObject::GetJString(ServerAuthCode));
	credentials->Init(javaCredentials);
#elif PLATFORM_IOS
	//No implementation for Google Play on iOS
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	firebase::auth::Credential credential = firebase::auth::PlayGamesAuthProvider::GetCredential(TCHAR_TO_ANSI(*ServerAuthCode));
	credentials->Init(credential);
#endif

	return credentials;
}

UFGAuthCredentials* UFGAuthLibrary::GetTwitterCredentials(const FString& Token, const FString& Secret)
{
	if (Token.IsEmpty() || Secret.IsEmpty())
	{
		UE_LOG(LogFirebaseGoodies, Error, TEXT("Empty string! \n Check token and secret!"));
		return nullptr;
	}

	UFGAuthCredentials* credentials = NewObject<UFGAuthCredentials>();

#if PLATFORM_ANDROID
	jobject javaCredentials = FGMethodCallUtils::CallStaticObjectMethod(FGAuthClassName, "getTwitterCredential",
		"(Ljava/lang/String;Ljava/lang/String;)Lcom/google/firebase/auth/AuthCredential;",
		*FJavaClassObject::GetJString(Token), *FJavaClassObject::GetJString(Secret));
	credentials->Init(javaCredentials);
#elif PLATFORM_IOS
	FIRAuthCredential* iosCredential = [FIRTwitterAuthProvider credentialWithToken:Token.GetNSString()
																			secret:Secret.GetNSString()];
	credentials->Init(iosCredential);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	firebase::auth::Credential credential = firebase::auth::TwitterAuthProvider::GetCredential(TCHAR_TO_ANSI(*Token), TCHAR_TO_ANSI(*Secret));
	credentials->Init(credential);
#endif

	return credentials;
}

UFGAuthCredentials* UFGAuthLibrary::GetPhoneCredentials(const FString& VerificationId, const FString& VerificationCode)
{
	UFGAuthCredentials* credentials = NewObject<UFGAuthCredentials>();

	if (VerificationId.IsEmpty() || VerificationCode.IsEmpty())
	{
		UE_LOG(LogFirebaseGoodies, Error, TEXT("Empty string! \n Check verification id and code!"));
		return credentials;
	}

#if PLATFORM_ANDROID
	jobject javaCredentials = FGMethodCallUtils::CallStaticObjectMethod(FGAuthClassName, "getPhoneCredential",
		"(Ljava/lang/String;Ljava/lang/String;)Lcom/google/firebase/auth/AuthCredential;",
		*FJavaClassObject::GetJString(VerificationId), *FJavaClassObject::GetJString(VerificationCode));
	credentials->Init(javaCredentials);
#elif PLATFORM_IOS
	FIRAuthCredential* iosCredential = [[FIRPhoneAuthProvider provider] credentialWithVerificationID:VerificationId.GetNSString()
																					verificationCode:VerificationCode.GetNSString()];
	credentials->Init(iosCredential);
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	firebase::auth::Auth* auth = firebase::auth::Auth::GetAuth(firebase::App::GetInstance());

	firebase::auth::Credential credential = firebase::auth::PhoneAuthProvider::GetInstance(auth).GetCredential(TCHAR_TO_ANSI(*VerificationId), TCHAR_TO_ANSI(*VerificationCode));
	credentials->Init(credential);
#endif

	return credentials;
}

void UFGAuthLibrary::VerifyPhoneNumber(const FString& Number, int TimeoutMillis, const FAuthCredentialsDelegate& OnSuccess,
                                       const FAuthStringDelegate& OnError, const FAuthStringDelegate& OnTimeout,
                                       const FAuthStringDelegate& OnSmsSent)
{
	if (Number.IsEmpty())
	{
		UE_LOG(LogFirebaseGoodies, Error, TEXT("Empty string! \n Check number!"));
		return;
	}

	PhoneVerificationSuccessCallback = OnSuccess;
	AuthErrorCallback = OnError;
	PhoneVerificationTimeoutCallback = OnTimeout;
	PhoneVerificationSentCallback = OnSmsSent;

#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(FGAuthClassName, "verifyPhoneNumber", "(Ljava/lang/String;ILandroid/app/Activity;)V",
		*FJavaClassObject::GetJString(Number), TimeoutMillis, FJavaWrapper::GameActivityThis);
#elif PLATFORM_IOS
	[[FIRPhoneAuthProvider provider] verifyPhoneNumber:Number.GetNSString()
											UIDelegate:nil
											completion:^(NSString* _Nullable verificationID, NSError* _Nullable error) {
												if (error != nil)
												{
													FString ErrorMessage = FString(error.localizedDescription);
													UE_LOG(LogFirebaseGoodies, Error, TEXT("Verify phone number error: %s"), *ErrorMessage);
													AsyncTask(ENamedThreads::GameThread, [=]() {
														UFGAuthLibrary::AuthErrorCallback.ExecuteIfBound(ErrorMessage);
													});
													return;
												}

												FString verificationIDString = FString(verificationID);
												AsyncTask(ENamedThreads::GameThread, [=]() {
													UFGAuthLibrary::PhoneVerificationSentCallback.ExecuteIfBound(verificationIDString);
												});
											}];
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	firebase::auth::Auth* auth = firebase::auth::Auth::GetAuth(firebase::App::GetInstance());

	firebase::auth::PhoneAuthProvider::GetInstance(auth).VerifyPhoneNumber(TCHAR_TO_ANSI(*Number), TimeoutMillis, new firebase::auth::PhoneAuthProvider::ForceResendingToken(), new FGPhoneListener());
#endif
}

void UFGAuthLibrary::PromptGoogleSignIn(const FAuthCredentialsDelegate& OnSuccess,
                                        const FAuthStringDelegate& OnError)
{
	SignInWithGoogleSuccessCallback = OnSuccess;
	AuthErrorCallback = OnError;

	UFirebaseGoodiesSettings* Settings = FFirebaseGoodiesModule::Get().GetSettings();
#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(FGAuthClassName, "signInWithGoogle", "(Landroid/app/Activity;Ljava/lang/String;)V",
		FJavaWrapper::GameActivityThis, *FJavaClassObject::GetJString(Settings->AndroidOauthClientID));
#elif PLATFORM_IOS
	GIDConfiguration *config = [[GIDConfiguration alloc] initWithClientID:Settings->ClientId.GetNSString()];
	[GIDSignIn.sharedInstance signInWithConfiguration:config presentingViewController:[IOSAppDelegate GetDelegate].IOSController callback:^(GIDGoogleUser * _Nullable user, NSError * _Nullable error) {
	  if (error == nil) {
		GIDAuthentication *authentication = user.authentication;
		FIRAuthCredential *credential = [FIRGoogleAuthProvider credentialWithIDToken:authentication.idToken
											accessToken:authentication.accessToken];
		UFGAuthCredentials* credentials = NewObject<UFGAuthCredentials>();
		credentials->Init(credential);

		AsyncTask(ENamedThreads::GameThread, [=]() {
			UFGAuthLibrary::SignInWithGoogleSuccessCallback.ExecuteIfBound(credentials);
		});
	  } else {
		FString ErrorMessage = FString(error.localizedDescription);
		UE_LOG(LogFirebaseGoodies, Error, TEXT("Google sign in error: %s"), *ErrorMessage);
		AsyncTask(ENamedThreads::GameThread, [=]() {
			UFGAuthLibrary::AuthErrorCallback.ExecuteIfBound(ErrorMessage);
		});
	  }
	}];
#endif
}

#if PLATFORM_ANDROID

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGAuth_OnIdTokenChanged(JNIEnv* env, jclass clazz)
{
	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGAuthLibrary::IdTokenChangedCallback.ExecuteIfBound();
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGAuth_OnAuthStateChanged(JNIEnv* env, jclass clazz)
{
	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGAuthLibrary::AuthStateChangedCallback.ExecuteIfBound();
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGAuth_OnUserCreated(JNIEnv* env, jclass clazz, jobject result)
{
	UFGFirebaseUser* user = NewObject<UFGFirebaseUser>();
	user->Init(result);

	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGAuthLibrary::UserCreateSuccessCallback.ExecuteIfBound(user);
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGAuth_OnAuthError(JNIEnv* env, jclass clazz, jstring error)
{
	FString ErrorMessage = FGJavaConvertor::FromJavaString(error);
	UE_LOG(LogFirebaseGoodies, Error, TEXT("Auth error: %s"), *ErrorMessage);

	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGAuthLibrary::AuthErrorCallback.ExecuteIfBound(ErrorMessage);
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGAuth_OnSignInMethodsFetched(JNIEnv* env, jclass clazz, jobjectArray methods)
{
	TArray<FString> providers = FGJavaConvertor::ConvertToStringArray(methods);

	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGAuthLibrary::SignInMethodsFetchedCallback.ExecuteIfBound(providers);
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGAuth_OnPasswordResetSent(JNIEnv* env, jclass clazz)
{
	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGAuthLibrary::PasswordResetSuccessCallback.ExecuteIfBound();
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGAuth_OnSignInSuccess(JNIEnv* env, jclass clazz, jobject result)
{
	UFGFirebaseUser* user = NewObject<UFGFirebaseUser>();
	user->Init(result);

	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGAuthLibrary::SignInSuccessCallback.ExecuteIfBound(user);
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGAuth_OnPhoneVerified(JNIEnv* env, jclass clazz, jobject credential)
{
	UFGAuthCredentials* result = NewObject<UFGAuthCredentials>();
	result->Init(credential);

	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGAuthLibrary::PhoneVerificationSuccessCallback.ExecuteIfBound(result);
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGAuth_OnPhoneVerificationCodeSent(JNIEnv* env, jclass clazz, jstring s)
{
	FString verificationId = FGJavaConvertor::FromJavaString(s);

	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGAuthLibrary::PhoneVerificationSentCallback.ExecuteIfBound(verificationId);
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGAuth_OnPhoneVerificationTimeout(JNIEnv* env, jclass clazz, jstring s)
{
	FString verificationId = FGJavaConvertor::FromJavaString(s);

	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGAuthLibrary::PhoneVerificationTimeoutCallback.ExecuteIfBound(verificationId);
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGAuth_OnGoogleSignInSuccess(JNIEnv* env, jclass clazz, jobject credential)
{
	UFGAuthCredentials* result = NewObject<UFGAuthCredentials>();
	result->Init(credential);

	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGAuthLibrary::SignInWithGoogleSuccessCallback.ExecuteIfBound(result);
	});
}

#endif
