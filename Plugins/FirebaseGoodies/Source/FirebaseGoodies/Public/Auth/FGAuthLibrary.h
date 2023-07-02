// Copyright (c) 2022 Nineva Studios

#pragma once

#include "FGAuthCredentials.h"
#include "FGFirebaseUser.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "FGAuthLibrary.generated.h"

DECLARE_DYNAMIC_DELEGATE(FAuthVoidDelegate);
DECLARE_DYNAMIC_DELEGATE_OneParam(FAuthUserDelegate, UFGFirebaseUser*, result);
DECLARE_DYNAMIC_DELEGATE_OneParam(FAuthStringDelegate, FString, string);
DECLARE_DYNAMIC_DELEGATE_OneParam(FSignInMethodsFetchedDelegate, const TArray<FString>&, methods);
DECLARE_DYNAMIC_DELEGATE_OneParam(FAuthCredentialsDelegate, UFGAuthCredentials*, result);

UCLASS()
class FIREBASEGOODIES_API UFGAuthLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	* Register listeners to changes in the token and user authentication states.
	*
	* @param IdTokenChangedDelegate - callback to be invoked when token authentication state changes.
	* @param AuthStateChangedDelegate - callback to be invoked when user authentication state changes.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Auth", meta = (AutoCreateRefTerm = "IdTokenChangedDelegate,AuthStateChangedDelegate"))
	static void InitListeners(const FAuthVoidDelegate& IdTokenChangedDelegate, const FAuthVoidDelegate& AuthStateChangedDelegate);

	/**
	* @return the currently signed-in FGFirebaseUser object.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Auth")
	static UFGFirebaseUser* CurrentUser();

	/**
	* Try to create a new user account with the given email address and password.
	*
	* @param Email - email address.
	* @param Password - password.
	* @param OnSuccess - callback to be invoked upon successful user registration.
	* @param OnError - callback to be invoked when user registration failed.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Auth", meta = (AutoCreateRefTerm = "OnSuccess,OnError"))
	static void CreateUser(const FString& Email, const FString& Password, const FAuthUserDelegate& OnSuccess, const FAuthStringDelegate& OnError);

	/**
	* Fetch a list of signin methods that can be used to sign in a given user (identified by its main email address).
	*
	* @param Email - email address.
	* @param OnSuccess - callback to be invoked with the array of fetched providers.
	* @param OnError - callback to be invoked when fetching providers failed.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Auth", meta = (AutoCreateRefTerm = "OnSuccess,OnError"))
	static void FetchProvidersForEmail(const FString& Email, const FSignInMethodsFetchedDelegate& OnSuccess, const FAuthStringDelegate& OnError);

	/**
	* Triggers the Firebase Authentication backend to send a password-reset email to the given email address,
	* which must correspond to an existing user of your app.
	*
	* @param Email - email address.
	* @param OnSuccess - callback to be invoked upon successful sending.
	* @param OnError - callback to be invoked when operation failed.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Auth", meta = (AutoCreateRefTerm = "OnSuccess,OnError"))
	static void SendPasswordReset(const FString& Email, const FAuthVoidDelegate& OnSuccess, const FAuthStringDelegate& OnError);

	/**
	* Try to sign in a user with the given email address and password.
	*
	* @param Email - email address.
	* @param Password - password.
	* @param OnSuccess - callback to be invoked upon successful sign in.
	* @param OnError - callback to be invoked when sign in failed.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Auth", meta = (AutoCreateRefTerm = "OnSuccess,OnError"))
	static void SignInWithEmailAndPassword(const FString& Email, const FString& Password, const FAuthUserDelegate& OnSuccess,
		const FAuthStringDelegate& OnError);

	/**
	* Try to sign in a user with the given token.
	*
	* @param Token - token.
	* @param OnSuccess - callback to be invoked upon successful sign in.
	* @param OnError - callback to be invoked when sign in failed.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Auth", meta = (AutoCreateRefTerm = "OnSuccess,OnError"))
	static void SignInWithToken(const FString& Token, const FAuthUserDelegate& OnSuccess, const FAuthStringDelegate& OnError);

	/**
	* Sign in the user anonymously without requiring any credential.
	*
	* @param OnSuccess - callback to be invoked upon successful sign in.
	* @param OnError - callback to be invoked when sign in failed.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Auth", meta = (AutoCreateRefTerm = "OnSuccess,OnError"))
	static void SignInAnonymously(const FAuthUserDelegate& OnSuccess, const FAuthStringDelegate& OnError);

	/**
	* Try to sign in a user with the given UFGAuthCredentials.
	*
	* @param Credentials - credentials.
	* @param OnSuccess - callback to be invoked upon successful sign in.
	* @param OnError - callback to be invoked when sign in failed.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Auth", meta = (AutoCreateRefTerm = "OnSuccess,OnError"))
	static void SignInWithCredentials(UFGAuthCredentials* Credentials, const FAuthUserDelegate& OnSuccess, const FAuthStringDelegate& OnError);

	/**
	* Sign out the current user.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Auth")
	static void SignOut();

	/**
	* Get the UFGAuthCredentials object for the given email and password.
	*
	* @param Email - credentials.
	* @param Password - callback to be invoked upon successful sign in.
	* @return UFGAuthCredentials object.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Auth")
	static UFGAuthCredentials* GetEmailCredentials(const FString& Email, const FString& Password);

	/**
	* Get the UFGAuthCredentials object for the given facebook token.
	*
	* @param Token - token obtained after successful Facebook authorization.
	* @return UFGAuthCredentials object.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Auth")
	static UFGAuthCredentials* GetFacebookCredentials(const FString& Token);

	/**
	* Get the UFGAuthCredentials object for the given github token.
	*
	* @param Token - token obtained after successful GitHub authorization.
	* @return UFGAuthCredentials object.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Auth")
	static UFGAuthCredentials* GetGithubCredentials(const FString& Token);

	/**
	* Get the UFGAuthCredentials object for the given Google sign in tokens.
	*
	* @param IdToken - ID token obtained after successful Google authorization.
	* @param AccessToken - access token obtained after successful Google authorization.
	* @return UFGAuthCredentials object.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Auth")
	static UFGAuthCredentials* GetGoogleCredentials(const FString& IdToken, const FString& AccessToken);

	/**
	* Get the UFGAuthCredentials object for the given Apple sign in tokens.
	*
	* @param IdToken - ID token obtained after successful Apple authorization.
	* @param RawNonce - The raw nonce associated with the Auth credential being created.
	* @return UFGAuthCredentials object.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Auth")
	static UFGAuthCredentials* GetAppleCredentials(const FString& IdToken, const FString& RawNonce);

	/**
	* Get the UFGAuthCredentials object for generic OAuth provider.
	*
	* @param Provider - provider name.
	* @param IdToken - ID token obtained after successful authorization.
	* @param AccessToken - access token obtained after successful authorization.
	* @return UFGAuthCredentials object.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Auth")
	static UFGAuthCredentials* GetGenericOAuthCredentials(const FString& Provider, const FString& IdToken, const FString& AccessToken);

	/**
	* Get the UFGAuthCredentials object for PlayGames account.
	*
	* @param ServerAuthCode - server authentication code obtained after successful authentication.
	* @return UFGAuthCredentials object.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Auth")
	static UFGAuthCredentials* GetPlayGamesCredentials(const FString& ServerAuthCode);

	/**
	* Get the UFGAuthCredentials object for the given Twitter sign in token and secret.
	*
	* @param Token - token obtained after successful Twitter authorization.
	* @param Secret - secret obtained after successful Twitter authorization.
	* @return UFGAuthCredentials object.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Auth")
	static UFGAuthCredentials* GetTwitterCredentials(const FString& Token, const FString& Secret);

	/**
	* Get the UFGAuthCredentials object for the given phone verification ID and code.
	*
	* @param VerificationId - verificarion ID, from callback OnSmsSent of the VerifyPhoneNumber call.
	* @param VerificationCode - verification code, sent to user's phone number via SMS.
	* @return UFGAuthCredentials object.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Auth")
	static UFGAuthCredentials* GetPhoneCredentials(const FString& VerificationId, const FString& VerificationCode);

	/**
	* Start the phone number verification process for the given phone number.
	* On most devices the user experience is the following: when you call this method after obtaining the user's phone number,
	* they will either receive an SMS or a push notification with the code, while the application will receive OnSmsSent callback
	* with the Verification ID needed for creating the UFGAuthCredentials object along with the verification code, obtained by the user.
	* Sometimes on Android devices the operation will be performed silently by the operating system, allowing you to use
	* the UFGAuthCredentials object from the OnSuccess callback.
	* In any case, your application should always be able to handle both cases.
	*
	* @param Number - phone number to be verified.
	* @param TimeoutMillis - operation timeout, in milliseconds.
	* @param OnSuccess - callback to be invoked with the UFGAuthCredentials object.
	* @param OnError - callback to be invoked when an error ocurred.
	* @param OnTimeout - callback to be invoked upon operation timeout.
	* @param OnSmsSent - callback to be invoked with VerificationId, required for creating Phone Credentials along with verification code.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Auth", meta = (AutoCreateRefTerm = "OnSuccess,OnError,OnTimeout,OnSmsSent"))
	static void VerifyPhoneNumber(const FString& Number, int TimeoutMillis, const FAuthCredentialsDelegate& OnSuccess,
		const FAuthStringDelegate& OnError, const FAuthStringDelegate& OnTimeout,
		const FAuthStringDelegate& OnSmsSent);

	/**
	* Open the native dialog prompting user to sign in with Google. If the operation is successful, OnSuccess callback will
	* invoked with the UFGAuthCredentials object that can be used in further user-related operations.
	*
	* @param OnSuccess - callback to be invoked with the UFGAuthCredentials object.
	* @param OnError - callback to be invoked when an error ocurred.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Auth", meta = (AutoCreateRefTerm = "OnSuccess,OnError"))
	static void PromptGoogleSignIn(const FAuthCredentialsDelegate& OnSuccess, const FAuthStringDelegate& OnError);

	static FAuthVoidDelegate IdTokenChangedCallback;
	static FAuthVoidDelegate AuthStateChangedCallback;
	static FAuthVoidDelegate PasswordResetSuccessCallback;
	static FAuthUserDelegate UserCreateSuccessCallback;
	static FAuthUserDelegate SignInSuccessCallback;
	static FAuthStringDelegate AuthErrorCallback;
	static FSignInMethodsFetchedDelegate SignInMethodsFetchedCallback;
	static FAuthCredentialsDelegate PhoneVerificationSuccessCallback;
	static FAuthStringDelegate PhoneVerificationTimeoutCallback;
	static FAuthStringDelegate PhoneVerificationSentCallback;
	static FAuthCredentialsDelegate SignInWithGoogleSuccessCallback;

	static const ANSICHAR* FGAuthClassName;
};
