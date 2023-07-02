// Copyright (c) 2022 Nineva Studios

#include "Analytics/FGAnalytics.h"

#include "Analytics/FGAnalyticsParameter.h"
#include "FirebaseGoodiesLog.h"

#if PLATFORM_ANDROID
#include "Android/AndroidApplication.h"
#include "Android/AndroidJNI.h"
#include "Android/AndroidJava.h"
#include "Android/Utils/FGJavaConvertor.h"
#include "Android/Utils/FGMethodCallUtils.h"
#endif

#if PLATFORM_IOS
#import <FirebaseAnalytics/FirebaseAnalytics.h>
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
#include "firebase/analytics.h"

#include "Desktop/FGUtils.h"
#endif

static const ANSICHAR* FGAnalyticsClassName = "com/ninevastudios/unrealfirebase/FGAnalytics";

void UFGAnalytics::SetAnalyticsCollectionEnabled(bool bEnabled)
{
#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(FGAnalyticsClassName, "SetAnalyticsCollectionEnabled", "(Landroid/app/Activity;Z)V",
		FJavaWrapper::GameActivityThis, bEnabled);
#endif

#if PLATFORM_IOS
	[FIRAnalytics setAnalyticsCollectionEnabled:bEnabled];
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	firebase::analytics::SetAnalyticsCollectionEnabled(bEnabled);
#endif
}

void UFGAnalytics::ResetAnalyticsData()
{
#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(FGAnalyticsClassName, "ResetAnalyticsData", "(Landroid/app/Activity;)V",
		FJavaWrapper::GameActivityThis);
#endif

#if PLATFORM_IOS
	[FIRAnalytics resetAnalyticsData];
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	firebase::analytics::ResetAnalyticsData();
#endif
}

void UFGAnalytics::SetSessionTimeoutDuration(int64 Milliseconds)
{
#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(FGAnalyticsClassName, "SetSessionTimeoutDuration", "(Landroid/app/Activity;J)V",
		FJavaWrapper::GameActivityThis, Milliseconds);
#endif

#if PLATFORM_IOS
	[FIRAnalytics setSessionTimeoutInterval:Milliseconds];
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	firebase::analytics::SetSessionTimeoutDuration(Milliseconds);
#endif
}

void UFGAnalytics::SetUserId(const FString& Id)
{
#if PLATFORM_ANDROID
	auto JId = FJavaClassObject::GetJString(Id);
	FGMethodCallUtils::CallStaticVoidMethod(FGAnalyticsClassName, "SetUserId", "(Landroid/app/Activity;Ljava/lang/String;)V",
		FJavaWrapper::GameActivityThis, *JId);
#endif

#if PLATFORM_IOS
	NSString* userId = !Id.IsEmpty() ? Id.GetNSString() : nil;
	[FIRAnalytics setUserID:userId];
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	firebase::analytics::SetUserId(TCHAR_TO_ANSI(*Id));
#endif
}

void UFGAnalytics::SetUserProperty(const FString& Name, const FString& Value)
{
#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(FGAnalyticsClassName, "SetUserProperty", "(Landroid/app/Activity;Ljava/lang/String;Ljava/lang/String;)V",
		FJavaWrapper::GameActivityThis, *FJavaClassObject::GetJString(Name), *FJavaClassObject::GetJString(Value));
#endif

#if PLATFORM_IOS
	NSString* value = !Value.IsEmpty() ? Value.GetNSString() : nil;
	[FIRAnalytics setUserPropertyString:value forName:Name.GetNSString()];
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	firebase::analytics::SetUserProperty(TCHAR_TO_ANSI(*Name), TCHAR_TO_ANSI(*Value));
#endif
}

void UFGAnalytics::SetCurrentScreen(const FString& ScreenName, const FString& ScreenClassName)
{
#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(FGAnalyticsClassName, "SetCurrentScreen", "(Landroid/app/Activity;Ljava/lang/String;Ljava/lang/String;)V",
		FJavaWrapper::GameActivityThis, *FJavaClassObject::GetJString(ScreenName), *FJavaClassObject::GetJString(ScreenClassName));
#endif

#if PLATFORM_IOS
	NSString* screenName = !ScreenName.IsEmpty() ? ScreenName.GetNSString() : nil;
	NSString* screenClassName = !ScreenClassName.IsEmpty() ? ScreenClassName.GetNSString() : nil;
	dispatch_async(dispatch_get_main_queue(), ^{
		[FIRAnalytics logEventWithName:kFIREventScreenView parameters: @{kFIRParameterScreenName: screenName, kFIRParameterScreenClass: screenClassName}];
	});
#endif
}

UFGAnalyticsParameter* UFGAnalytics::CreateIntegerParameter(const FString& Name, int64 Value)
{
	UFGAnalyticsParameter* parameter = NewObject<UFGAnalyticsParameter>();
	parameter->SetName(Name);
	parameter->SetInt(Value);

	return parameter;
}

UFGAnalyticsParameter* UFGAnalytics::CreateFloatParameter(const FString& Name, float Value)
{
	UFGAnalyticsParameter* parameter = NewObject<UFGAnalyticsParameter>();
	parameter->SetName(Name);
	parameter->SetFloat(Value);

	return parameter;
}

UFGAnalyticsParameter* UFGAnalytics::CreateStringParameter(const FString& Name, const FString& Value)
{
	UFGAnalyticsParameter* parameter = NewObject<UFGAnalyticsParameter>();
	parameter->SetName(Name);
	parameter->SetString(Value);

	return parameter;
}

void UFGAnalytics::LogEvent(const FString& EventName)
{
#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(FGAnalyticsClassName, "LogEvent", "(Landroid/app/Activity;Ljava/lang/String;)V",
		FJavaWrapper::GameActivityThis, *FJavaClassObject::GetJString(EventName));
#endif

#if PLATFORM_IOS
	[FIRAnalytics logEventWithName:EventName.GetNSString()
						parameters:nil];
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	firebase::analytics::LogEvent(TCHAR_TO_ANSI(*EventName));
#endif
}

void UFGAnalytics::LogEventWithParameter(const FString& EventName, UFGAnalyticsParameter* Parameter)
{
	TArray<UFGAnalyticsParameter*> parameters;
	parameters.Add(Parameter);
	LogEventWithParameters(EventName, parameters);
}

void UFGAnalytics::LogEventWithParameters(const FString& EventName, const TArray<UFGAnalyticsParameter*>& Parameters)
{
#if PLATFORM_ANDROID
	// TODO wrap this in a separate class
	JNIEnv* env = FAndroidApplication::GetJavaEnv();
	jclass bundleClass = FJavaWrapper::FindClass(env, "android/os/Bundle", false);
	jmethodID bundleCtor = FJavaWrapper::FindMethod(env, bundleClass, "<init>", "()V", false);
	jmethodID putLongMethod = FJavaWrapper::FindMethod(env, bundleClass, "putLong", "(Ljava/lang/String;J)V", false);
	jmethodID putDoubleMethod = FJavaWrapper::FindMethod(env, bundleClass, "putDouble", "(Ljava/lang/String;D)V", false);
	jmethodID putStringMethod = FJavaWrapper::FindMethod(env, bundleClass, "putString", "(Ljava/lang/String;Ljava/lang/String;)V", false);
	jobject bundle = env->NewObject(bundleClass, bundleCtor);

	for (const auto parameter : Parameters)
	{
		switch (parameter->GetType())
		{
			case EParameterType::Integer:
			{
				env->CallVoidMethod(bundle, putLongMethod,
					*FJavaClassObject::GetJString(parameter->GetName()), parameter->GetInt());
				break;
			}
			case EParameterType::Float:
			{
				env->CallVoidMethod(bundle, putDoubleMethod,
					*FJavaClassObject::GetJString(parameter->GetName()), parameter->GetFloat());
				break;
			}
			case EParameterType::String:
			{
				env->CallVoidMethod(bundle, putStringMethod,
					*FJavaClassObject::GetJString(parameter->GetName()),
					*FJavaClassObject::GetJString(parameter->GetString()));
				break;
			}
			default:
			{
				UE_LOG(LogFirebaseGoodies, Error, TEXT("Trying to log an event with an undefined parameter"));
				break;
			}
		}
	}

	FGMethodCallUtils::CallStaticVoidMethod(FGAnalyticsClassName, "LogEvent",
		"(Landroid/app/Activity;Ljava/lang/String;Landroid/os/Bundle;)V",
		FJavaWrapper::GameActivityThis, *FJavaClassObject::GetJString(EventName), bundle);
#endif

#if PLATFORM_IOS
	NSMutableDictionary* parameters = [NSMutableDictionary dictionary];

	for (const auto parameter : Parameters)
	{
		switch (parameter->GetType())
		{
			case EParameterType::Integer:
			{
				NSNumber* paramValue = [NSNumber numberWithInt:parameter->GetInt()];
				parameters[parameter->GetName().GetNSString()] = paramValue;
				break;
			}
			case EParameterType::Float:
			{
				NSNumber* paramValue = [NSNumber numberWithFloat:parameter->GetFloat()];
				parameters[parameter->GetName().GetNSString()] = paramValue;
				break;
			}
			case EParameterType::String:
			{
				parameters[parameter->GetName().GetNSString()] = parameter->GetString().GetNSString();
				break;
			}
			default:
			{
				UE_LOG(LogFirebaseGoodies, Error, TEXT("Trying to log an event with an undefined parameter"));
				break;
			}
		}
	}

	[FIRAnalytics logEventWithName:EventName.GetNSString() parameters:parameters];
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	const int size = Parameters.Num();

	TArray<firebase::analytics::Parameter> parameters;

	for (int i = 0; i < Parameters.Num(); i++)
	{
		firebase::analytics::Parameter parameter;

		char* parameterName = FGUtils::GetStringCopy(Parameters[i]->GetName());
		switch (Parameters[i]->GetType())
		{
			case EParameterType::Integer:
			{
				parameters.Add({ parameterName, Parameters[i]->GetInt() });
				break;
			}
			case EParameterType::Float:
			{
				parameters.Add({ parameterName, Parameters[i]->GetFloat() });
				break;
			}
			case EParameterType::String:
			{
				parameters.Add({ parameterName, FGUtils::GetStringCopy(Parameters[i]->GetString()) });
				break;
			}
			default:
			{
				UE_LOG(LogFirebaseGoodies, Error, TEXT("Trying to log an event with an undefined parameter"));
				break;
			}
		}
	}

	firebase::analytics::LogEvent(TCHAR_TO_ANSI(*EventName), parameters.GetData(), parameters.Num());

	FGUtils::ClearStringCopies();
#endif
}
