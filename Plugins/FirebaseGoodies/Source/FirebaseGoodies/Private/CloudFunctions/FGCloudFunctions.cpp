// Copyright (c) 2022 Nineva Studios

#include "CloudFunctions/FGCloudFunctions.h"

#include "Async/Async.h"
#include "FirebaseGoodiesLog.h"
#include "Common/FGValueVariantAccess.h"

#if PLATFORM_ANDROID
#include "Android/Utils/FGJavaConvertor.h"
#include "Android/Utils/FGMethodCallUtils.h"
#include "Android/Utils/FGValueVariantUtils.h"
#endif

#if PLATFORM_IOS
#include "IOS/Utils/FGCloudFunctionsErrorUtils.h"
#include "IOS/Utils/FGValueVariantUtils.h"

#import <FirebaseFunctions/FirebaseFunctions.h>
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
#include "firebase/functions.h"

#include "Desktop/FGUtils.h"
#include "Desktop/FGVariantUtils.h"
#endif

FCloudFunctionsStringDelegate UFGCloudFunctions::OnCallCloudFunctionErrorCallback;
FCloudFunctionsStringDelegate UFGCloudFunctions::OnCallStringCloudFunctionSuccessCallback;
FCloudFunctionsIntDelegate UFGCloudFunctions::OnCallIntCloudFunctionSuccessCallback;
FCloudFunctionsFloatDelegate UFGCloudFunctions::OnCallFloatCloudFunctionSuccessCallback;
FCloudFunctionsBoolDelegate UFGCloudFunctions::OnCallBoolCloudFunctionSuccessCallback;
FCloudFunctionsVoidDelegate UFGCloudFunctions::OnCallVoidCloudFunctionSuccessCallback;
FCloudFunctionsMapDelegate UFGCloudFunctions::OnCallMapCloudFunctionSuccessCallback;
FCloudFunctionsArrayDelegate UFGCloudFunctions::OnCallArrayCloudFunctionSuccessCallback;

const ANSICHAR* UFGCloudFunctions::FGCloudFunctionsClassName = "com/ninevastudios/unrealfirebase/FGCloudFunctions";

const FString DefaultRegion = "us-central1";

void UFGCloudFunctions::CallStringFunction(const FString& MethodName, FString Region,
	const TMap<FString, FFGValueVariant>& Parameters,
	const FCloudFunctionsStringDelegate& OnSuccess,
	const FCloudFunctionsStringDelegate& OnError)
{
	OnCallStringCloudFunctionSuccessCallback = OnSuccess;
	OnCallCloudFunctionErrorCallback = OnError;

	if (Region.IsEmpty())
	{
		Region = DefaultRegion;
	}

#if PLATFORM_ANDROID
	JNIEnv* JEnv = AndroidJavaEnv::GetJavaEnv();

	FGMethodCallUtils::CallStaticVoidMethod(FGCloudFunctionsClassName, "callStringFunction",
		"(Ljava/lang/String;Ljava/lang/String;Ljava/util/Map;)V",
		*FJavaClassObject::GetJString(MethodName), *FJavaClassObject::GetJString(Region),
		ValueVariantUtils::VariantMapToJavaMap(JEnv, Parameters));
#elif PLATFORM_IOS
	FIRFunctions* functions = [FIRFunctions functionsForRegion:Region.GetNSString()];

	id ParametersObject = ValueVariantUtils::VariantMapToNSDictionary(Parameters);

	[[functions HTTPSCallableWithName:MethodName.GetNSString()] callWithObject:ParametersObject
																	completion:^(FIRHTTPSCallableResult* result,
																		NSError* error) {
																		if (error)
																		{
																			CloudFunctionsErrorUtils::HandleError(error);
																		}
																		else
																		{
																			FFGValueVariant Data = ValueVariantUtils::IdToVariant(result.data);
																			AsyncTask(ENamedThreads::GameThread, [=]() {
																				UFGCloudFunctions::OnCallStringCloudFunctionSuccessCallback.ExecuteIfBound(UFGValueVariantAccess::GetString(Data));
																			});
																		}
																	}];
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	firebase::functions::Functions* functions = firebase::functions::Functions::GetInstance(firebase::App::GetInstance(), TCHAR_TO_ANSI(*Region));

	auto task = functions->GetHttpsCallable(TCHAR_TO_ANSI(*MethodName)).Call(FGVariantUtils::ValueVariantMapToFirebaseVariant(Parameters));
	task.AddOnCompletion([=](const ::firebase::Future<firebase::functions::HttpsCallableResult>& callback) {
		if (callback.status() == firebase::FutureStatus::kFutureStatusComplete)
		{
			FString Result = FString(callback.result()->data().string_value());

			AsyncTask(ENamedThreads::GameThread, [=]() {
				OnCallStringCloudFunctionSuccessCallback.ExecuteIfBound(Result);
			});
		}
		else
		{
			FString ErrorMessage = FString(callback.error_message());

			UE_LOG(LogFirebaseGoodies, Error, TEXT("Call String Function error: %s"), *ErrorMessage);

			AsyncTask(ENamedThreads::GameThread, [=]() {
				OnCallCloudFunctionErrorCallback.ExecuteIfBound(ErrorMessage);
			});
		}

		FGUtils::ClearStringCopies();
	});
#endif
}

void UFGCloudFunctions::CallIntFunction(const FString& MethodName, FString Region,
	const TMap<FString, FFGValueVariant>& Parameters,
	const FCloudFunctionsIntDelegate& OnSuccess,
	const FCloudFunctionsStringDelegate& OnError)
{
	OnCallIntCloudFunctionSuccessCallback = OnSuccess;
	OnCallCloudFunctionErrorCallback = OnError;

	if (Region.IsEmpty())
	{
		Region = DefaultRegion;
	}

#if PLATFORM_ANDROID
	JNIEnv* JEnv = AndroidJavaEnv::GetJavaEnv();

	FGMethodCallUtils::CallStaticVoidMethod(FGCloudFunctionsClassName, "callIntFunction",
		"(Ljava/lang/String;Ljava/lang/String;Ljava/util/Map;)V",
		*FJavaClassObject::GetJString(MethodName), *FJavaClassObject::GetJString(Region),
		ValueVariantUtils::VariantMapToJavaMap(JEnv, Parameters));
#elif PLATFORM_IOS
	FIRFunctions* functions = [FIRFunctions functionsForRegion:Region.GetNSString()];

	id ParametersObject = ValueVariantUtils::VariantMapToNSDictionary(Parameters);

	[[functions HTTPSCallableWithName:MethodName.GetNSString()] callWithObject:ParametersObject
																	completion:^(FIRHTTPSCallableResult* result,
																		NSError* error) {
																		if (error)
																		{
																			CloudFunctionsErrorUtils::HandleError(error);
																		}
																		else
																		{
																			FFGValueVariant Data = ValueVariantUtils::IdToVariant(result.data);
																			AsyncTask(ENamedThreads::GameThread, [=]() {
																				UFGCloudFunctions::OnCallIntCloudFunctionSuccessCallback.ExecuteIfBound(UFGValueVariantAccess::GetInteger(Data));
																			});
																		}
																	}];
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	firebase::functions::Functions* functions = firebase::functions::Functions::GetInstance(firebase::App::GetInstance(), TCHAR_TO_ANSI(*Region));

	auto task = functions->GetHttpsCallable(TCHAR_TO_ANSI(*MethodName)).Call(FGVariantUtils::ValueVariantMapToFirebaseVariant(Parameters));
	task.AddOnCompletion([=](const ::firebase::Future<firebase::functions::HttpsCallableResult>& callback) {
		if (callback.status() == firebase::FutureStatus::kFutureStatusComplete)
		{
			int Result = callback.result()->data().int64_value();

			AsyncTask(ENamedThreads::GameThread, [=]() {
				OnCallIntCloudFunctionSuccessCallback.ExecuteIfBound(Result);
			});
		}
		else
		{
			FString ErrorMessage = FString(callback.error_message());

			UE_LOG(LogFirebaseGoodies, Error, TEXT("Call Int Function error: %s"), *ErrorMessage);

			AsyncTask(ENamedThreads::GameThread, [=]() {
				OnCallCloudFunctionErrorCallback.ExecuteIfBound(ErrorMessage);
			});
		}

		FGUtils::ClearStringCopies();
	});
#endif
}

void UFGCloudFunctions::CallFloatFunction(const FString& MethodName, FString Region,
	const TMap<FString, FFGValueVariant>& Parameters,
	const FCloudFunctionsFloatDelegate& OnSuccess,
	const FCloudFunctionsStringDelegate& OnError)
{
	OnCallFloatCloudFunctionSuccessCallback = OnSuccess;
	OnCallCloudFunctionErrorCallback = OnError;

	if (Region.IsEmpty())
	{
		Region = DefaultRegion;
	}

#if PLATFORM_ANDROID
	JNIEnv* JEnv = AndroidJavaEnv::GetJavaEnv();

	FGMethodCallUtils::CallStaticVoidMethod(FGCloudFunctionsClassName, "callFloatFunction",
		"(Ljava/lang/String;Ljava/lang/String;Ljava/util/Map;)V",
		*FJavaClassObject::GetJString(MethodName), *FJavaClassObject::GetJString(Region),
		ValueVariantUtils::VariantMapToJavaMap(JEnv, Parameters));
#elif PLATFORM_IOS
	FIRFunctions* functions = [FIRFunctions functionsForRegion:Region.GetNSString()];

	id ParametersObject = ValueVariantUtils::VariantMapToNSDictionary(Parameters);

	[[functions HTTPSCallableWithName:MethodName.GetNSString()] callWithObject:ParametersObject
																	completion:^(FIRHTTPSCallableResult* result,
																		NSError* error) {
																		if (error)
																		{
																			CloudFunctionsErrorUtils::HandleError(error);
																		}
																		else
																		{
																			FFGValueVariant Data = ValueVariantUtils::IdToVariant(result.data);
																			AsyncTask(ENamedThreads::GameThread, [=]() {
																				UFGCloudFunctions::OnCallFloatCloudFunctionSuccessCallback.ExecuteIfBound(UFGValueVariantAccess::GetFloat(Data));
																			});
																		}
																	}];
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	firebase::functions::Functions* functions = firebase::functions::Functions::GetInstance(firebase::App::GetInstance(), TCHAR_TO_ANSI(*Region));

	auto task = functions->GetHttpsCallable(TCHAR_TO_ANSI(*MethodName)).Call(FGVariantUtils::ValueVariantMapToFirebaseVariant(Parameters));
	task.AddOnCompletion([=](const ::firebase::Future<firebase::functions::HttpsCallableResult>& callback) {
		if (callback.status() == firebase::FutureStatus::kFutureStatusComplete)
		{
			float Result = callback.result()->data().double_value();

			AsyncTask(ENamedThreads::GameThread, [=]() {
				OnCallFloatCloudFunctionSuccessCallback.ExecuteIfBound(Result);
			});
		}
		else
		{
			FString ErrorMessage = FString(callback.error_message());

			UE_LOG(LogFirebaseGoodies, Error, TEXT("Call Float Function error: %s"), *ErrorMessage);

			AsyncTask(ENamedThreads::GameThread, [=]() {
				OnCallCloudFunctionErrorCallback.ExecuteIfBound(ErrorMessage);
			});
		}

		FGUtils::ClearStringCopies();
	});
#endif
}

void UFGCloudFunctions::CallBoolFunction(const FString& MethodName, FString Region,
	const TMap<FString, FFGValueVariant>& Parameters,
	const FCloudFunctionsBoolDelegate& OnSuccess,
	const FCloudFunctionsStringDelegate& OnError)
{
	OnCallBoolCloudFunctionSuccessCallback = OnSuccess;
	OnCallCloudFunctionErrorCallback = OnError;

	if (Region.IsEmpty())
	{
		Region = DefaultRegion;
	}

#if PLATFORM_ANDROID
	JNIEnv* JEnv = AndroidJavaEnv::GetJavaEnv();

	FGMethodCallUtils::CallStaticVoidMethod(FGCloudFunctionsClassName, "callBoolFunction",
		"(Ljava/lang/String;Ljava/lang/String;Ljava/util/Map;)V",
		*FJavaClassObject::GetJString(MethodName), *FJavaClassObject::GetJString(Region),
		ValueVariantUtils::VariantMapToJavaMap(JEnv, Parameters));
#elif PLATFORM_IOS
	FIRFunctions* functions = [FIRFunctions functionsForRegion:Region.GetNSString()];

	id ParametersObject = ValueVariantUtils::VariantMapToNSDictionary(Parameters);

	[[functions HTTPSCallableWithName:MethodName.GetNSString()] callWithObject:ParametersObject
																	completion:^(FIRHTTPSCallableResult* result,
																		NSError* error) {
																		if (error)
																		{
																			CloudFunctionsErrorUtils::HandleError(error);
																		}
																		else
																		{
																			FFGValueVariant Data = ValueVariantUtils::IdToVariant(result.data);
																			AsyncTask(ENamedThreads::GameThread, [=]() {
																				UFGCloudFunctions::OnCallBoolCloudFunctionSuccessCallback.ExecuteIfBound(UFGValueVariantAccess::GetBool(Data));
																			});
																		}
																	}];
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	firebase::functions::Functions* functions = firebase::functions::Functions::GetInstance(firebase::App::GetInstance(), TCHAR_TO_ANSI(*Region));

	auto task = functions->GetHttpsCallable(TCHAR_TO_ANSI(*MethodName)).Call(FGVariantUtils::ValueVariantMapToFirebaseVariant(Parameters));
	task.AddOnCompletion([=](const ::firebase::Future<firebase::functions::HttpsCallableResult>& callback) {
		if (callback.status() == firebase::FutureStatus::kFutureStatusComplete)
		{
			bool Result = callback.result()->data().bool_value();

			AsyncTask(ENamedThreads::GameThread, [=]() {
				OnCallBoolCloudFunctionSuccessCallback.ExecuteIfBound(Result);
			});
		}
		else
		{
			FString ErrorMessage = FString(callback.error_message());

			UE_LOG(LogFirebaseGoodies, Error, TEXT("Call Bool Function error: %s"), *ErrorMessage);

			AsyncTask(ENamedThreads::GameThread, [=]() {
				OnCallCloudFunctionErrorCallback.ExecuteIfBound(ErrorMessage);
			});
		}

		FGUtils::ClearStringCopies();
	});
#endif
}

void UFGCloudFunctions::CallVoidFunction(const FString& MethodName, FString Region,
	const TMap<FString, FFGValueVariant>& Parameters,
	const FCloudFunctionsVoidDelegate& OnSuccess,
	const FCloudFunctionsStringDelegate& OnError)
{
	OnCallVoidCloudFunctionSuccessCallback = OnSuccess;
	OnCallCloudFunctionErrorCallback = OnError;

	if (Region.IsEmpty())
	{
		Region = DefaultRegion;
	}

#if PLATFORM_ANDROID
	JNIEnv* JEnv = AndroidJavaEnv::GetJavaEnv();

	FGMethodCallUtils::CallStaticVoidMethod(FGCloudFunctionsClassName, "callVoidFunction",
		"(Ljava/lang/String;Ljava/lang/String;Ljava/util/Map;)V",
		*FJavaClassObject::GetJString(MethodName), *FJavaClassObject::GetJString(Region),
		ValueVariantUtils::VariantMapToJavaMap(JEnv, Parameters));
#elif PLATFORM_IOS
	FIRFunctions* functions = [FIRFunctions functionsForRegion:Region.GetNSString()];

	id ParametersObject = ValueVariantUtils::VariantMapToNSDictionary(Parameters);

	[[functions HTTPSCallableWithName:MethodName.GetNSString()] callWithObject:ParametersObject
																	completion:^(FIRHTTPSCallableResult* result,
																		NSError* error) {
																		if (error)
																		{
																			CloudFunctionsErrorUtils::HandleError(error);
																		}
																		else
																		{
																			AsyncTask(ENamedThreads::GameThread, [=]() {
																				UFGCloudFunctions::OnCallVoidCloudFunctionSuccessCallback.ExecuteIfBound();
																			});
																		}
																	}];
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	firebase::functions::Functions* functions = firebase::functions::Functions::GetInstance(firebase::App::GetInstance(), TCHAR_TO_ANSI(*Region));

	auto task = functions->GetHttpsCallable(TCHAR_TO_ANSI(*MethodName)).Call(FGVariantUtils::ValueVariantMapToFirebaseVariant(Parameters));
	task.AddOnCompletion([=](const ::firebase::Future<firebase::functions::HttpsCallableResult>& callback) {
		if (callback.status() == firebase::FutureStatus::kFutureStatusComplete)
		{
			AsyncTask(ENamedThreads::GameThread, [=]() {
				OnCallVoidCloudFunctionSuccessCallback.ExecuteIfBound();
			});
		}
		else
		{
			FString ErrorMessage = FString(callback.error_message());

			UE_LOG(LogFirebaseGoodies, Error, TEXT("Call Bool Function error: %s"), *ErrorMessage);

			AsyncTask(ENamedThreads::GameThread, [=]() {
				OnCallCloudFunctionErrorCallback.ExecuteIfBound(ErrorMessage);
			});
		}

		FGUtils::ClearStringCopies();
	});
#endif
}

void UFGCloudFunctions::CallMapFunction(const FString& MethodName, FString Region,
	const TMap<FString, FFGValueVariant>& Parameters,
	const FCloudFunctionsMapDelegate& OnSuccess,
	const FCloudFunctionsStringDelegate& OnError)
{
	OnCallMapCloudFunctionSuccessCallback = OnSuccess;
	OnCallCloudFunctionErrorCallback = OnError;

	if (Region.IsEmpty())
	{
		Region = DefaultRegion;
	}

#if PLATFORM_ANDROID
	JNIEnv* JEnv = AndroidJavaEnv::GetJavaEnv();

	FGMethodCallUtils::CallStaticVoidMethod(FGCloudFunctionsClassName, "callMapFunction",
		"(Ljava/lang/String;Ljava/lang/String;Ljava/util/Map;)V",
		*FJavaClassObject::GetJString(MethodName), *FJavaClassObject::GetJString(Region),
		ValueVariantUtils::VariantMapToJavaMap(JEnv, Parameters));
#elif PLATFORM_IOS
	FIRFunctions* functions = [FIRFunctions functionsForRegion:Region.GetNSString()];

	id ParametersObject = ValueVariantUtils::VariantMapToNSDictionary(Parameters);

	[[functions HTTPSCallableWithName:MethodName.GetNSString()] callWithObject:ParametersObject
																	completion:^(FIRHTTPSCallableResult* result,
																		NSError* error) {
																		if (error)
																		{
																			CloudFunctionsErrorUtils::HandleError(error);
																		}
																		else
																		{
																			FMapWrapper Result;
																			Result.Map = ValueVariantUtils::NSDictionaryToVariantMap(result.data);
																			AsyncTask(ENamedThreads::GameThread, [=]() {
																				UFGCloudFunctions::OnCallMapCloudFunctionSuccessCallback.ExecuteIfBound(Result);
																			});
																		}
																	}];
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	firebase::functions::Functions* functions = firebase::functions::Functions::GetInstance(firebase::App::GetInstance(), TCHAR_TO_ANSI(*Region));

	auto task = functions->GetHttpsCallable(TCHAR_TO_ANSI(*MethodName)).Call(FGVariantUtils::ValueVariantMapToFirebaseVariant(Parameters));
	task.AddOnCompletion([=](const ::firebase::Future<firebase::functions::HttpsCallableResult>& callback) {
		if (callback.status() == firebase::FutureStatus::kFutureStatusComplete)
		{
			FMapWrapper Wrapper;
			Wrapper.Map = FGVariantUtils::FirebaseVariantMapToValueVariantMap(callback.result()->data().map());

			AsyncTask(ENamedThreads::GameThread, [=]() {
				OnCallMapCloudFunctionSuccessCallback.ExecuteIfBound(Wrapper);
			});
		}
		else
		{
			FString ErrorMessage = FString(callback.error_message());

			UE_LOG(LogFirebaseGoodies, Error, TEXT("Call Array Function error: %s"), *ErrorMessage);

			AsyncTask(ENamedThreads::GameThread, [=]() {
				OnCallCloudFunctionErrorCallback.ExecuteIfBound(ErrorMessage);
			});
		}

		FGUtils::ClearStringCopies();
	});
#endif
}

void UFGCloudFunctions::CallArrayFunction(const FString& MethodName, FString Region,
	const TMap<FString, FFGValueVariant>& Parameters,
	const FCloudFunctionsArrayDelegate& OnSuccess,
	const FCloudFunctionsStringDelegate& OnError)
{
	OnCallArrayCloudFunctionSuccessCallback = OnSuccess;
	OnCallCloudFunctionErrorCallback = OnError;

	if (Region.IsEmpty())
	{
		Region = DefaultRegion;
	}

#if PLATFORM_ANDROID
	JNIEnv* JEnv = AndroidJavaEnv::GetJavaEnv();

	FGMethodCallUtils::CallStaticVoidMethod(FGCloudFunctionsClassName, "callArrayFunction",
		"(Ljava/lang/String;Ljava/lang/String;Ljava/util/Map;)V",
		*FJavaClassObject::GetJString(MethodName), *FJavaClassObject::GetJString(Region),
		ValueVariantUtils::VariantMapToJavaMap(JEnv, Parameters));
#elif PLATFORM_IOS
	FIRFunctions* functions = [FIRFunctions functionsForRegion:Region.GetNSString()];

	id ParametersObject = ValueVariantUtils::VariantMapToNSDictionary(Parameters);

	[[functions HTTPSCallableWithName:MethodName.GetNSString()] callWithObject:ParametersObject
																	completion:^(FIRHTTPSCallableResult* result,
																		NSError* error) {
																		if (error)
																		{
																			CloudFunctionsErrorUtils::HandleError(error);
																		}
																		else
																		{
																			TArray<FFGValueVariant> Result = ValueVariantUtils::NSArrayToVariantArray(result.data);
																			AsyncTask(ENamedThreads::GameThread, [=]() {
																				UFGCloudFunctions::OnCallArrayCloudFunctionSuccessCallback.ExecuteIfBound(Result);
																			});
																		}
																	}];
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	firebase::functions::Functions* functions = firebase::functions::Functions::GetInstance(firebase::App::GetInstance(), TCHAR_TO_ANSI(*Region));

	auto task = functions->GetHttpsCallable(TCHAR_TO_ANSI(*MethodName)).Call(FGVariantUtils::ValueVariantMapToFirebaseVariant(Parameters));
	task.AddOnCompletion([=](const ::firebase::Future<firebase::functions::HttpsCallableResult>& callback) {
		if (callback.status() == firebase::FutureStatus::kFutureStatusComplete)
		{
			TArray<FFGValueVariant> Result = FGVariantUtils::FirebaseVariantVectorToValueVariantArray(callback.result()->data().vector());

			AsyncTask(ENamedThreads::GameThread, [=]() {
				OnCallArrayCloudFunctionSuccessCallback.ExecuteIfBound(Result);
			});
		}
		else
		{
			FString ErrorMessage = FString(callback.error_message());

			UE_LOG(LogFirebaseGoodies, Error, TEXT("Call Array Function error: %s"), *ErrorMessage);

			AsyncTask(ENamedThreads::GameThread, [=]() {
				OnCallCloudFunctionErrorCallback.ExecuteIfBound(ErrorMessage);
			});
		}

		FGUtils::ClearStringCopies();
	});
#endif
}

#if PLATFORM_ANDROID
JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGCloudFunctions_OnFunctionError(JNIEnv* env, jclass clazz, jstring error)
{
	FString ErrorMessage = FGJavaConvertor::FromJavaString(error);
	UE_LOG(LogFirebaseGoodies, Error, TEXT("Calling Cloud Function failed: %s"), *ErrorMessage);

	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGCloudFunctions::OnCallCloudFunctionErrorCallback.ExecuteIfBound(ErrorMessage);
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGCloudFunctions_OnStringFunctionComplete(JNIEnv* env, jclass clazz, jstring string)
{
	FString StringResponse = FGJavaConvertor::FromJavaString(string);

	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGCloudFunctions::OnCallStringCloudFunctionSuccessCallback.ExecuteIfBound(StringResponse);
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGCloudFunctions_OnIntFunctionComplete(JNIEnv* env, jclass clazz, jint intRes)
{
	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGCloudFunctions::OnCallIntCloudFunctionSuccessCallback.ExecuteIfBound(intRes);
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGCloudFunctions_OnFloatFunctionComplete(JNIEnv* env, jclass clazz, jfloat floatRes)
{
	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGCloudFunctions::OnCallFloatCloudFunctionSuccessCallback.ExecuteIfBound(floatRes);
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGCloudFunctions_OnBoolFunctionComplete(JNIEnv* env, jclass clazz, jboolean boolRes)
{
	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGCloudFunctions::OnCallBoolCloudFunctionSuccessCallback.ExecuteIfBound(boolRes);
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGCloudFunctions_OnVoidFunctionComplete(JNIEnv* env, jclass clazz)
{
	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGCloudFunctions::OnCallVoidCloudFunctionSuccessCallback.ExecuteIfBound();
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGCloudFunctions_OnMapFunctionComplete(JNIEnv* env, jclass clazz, jobject mapRes)
{
	FMapWrapper Result;
	Result.Map = ValueVariantUtils::JavaMapToVariantMap(env, mapRes);

	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGCloudFunctions::OnCallMapCloudFunctionSuccessCallback.ExecuteIfBound(Result);
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGCloudFunctions_OnArrayFunctionComplete(JNIEnv* env, jclass clazz, jobject arrayRes)
{
	TArray<FFGValueVariant> Result = ValueVariantUtils::JavaListToVariantArray(env, arrayRes);

	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGCloudFunctions::OnCallArrayCloudFunctionSuccessCallback.ExecuteIfBound(Result);
	});
}
#endif
