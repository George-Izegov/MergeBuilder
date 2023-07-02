// Copyright (c) 2022 Nineva Studios

#include "Demo/FGDemoFunctions.h"

#include "FirebaseGoodiesLog.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonReader.h"

FString UFGDemoFunctions::SaveTestFile()
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("FirebaseGoodies => UFGDemoFunctions::SaveTestFile"));

	FString path;
	FString text = FGuid::NewGuid().ToString();
#if PLATFORM_IOS
	NSArray* documentArr = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString* documentPath = [documentArr firstObject];

	path = FString(documentPath) + "/" + text + ".txt";
#elif PLATFORM_ANDROID
	path = FPaths::ProjectPersistentDownloadDir() + "/DemoFiles/";
	path = path + text + ".txt";
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	FString FilePath = FPaths::Combine(FPaths::ProjectDir(), text);
	FilePath += ".txt";
	path = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*FilePath);
#endif
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("Test file path: %s"), *path);

	FFileHelper::SaveStringToFile(text, *path);
	return path;
}

void UFGDemoFunctions::CrashApplication()
{
	int *volatile nptr = 0;
	int c = *nptr;
	printf("%d", c);
}

void UFGDemoFunctions::NotifyIos(const TMap<FString, FFGValueVariant>& NotificationPayload)
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGDemoFunctions::NotifyIos"));
	
	FString Title, Body;
	
	auto Aps = *NotificationPayload.Find(TEXT("aps"));
	if (Aps.Type != EValueType::Map)
	{
		UE_LOG(LogFirebaseGoodies, Error, TEXT("UFGDemoFunctions::NotifyIos - error parsing JSON"));
		return;
	}
	
	auto Alert = *Aps.GetValue<TMap<FString, FFGValueVariant>>().Find(TEXT("alert"));
	if (Alert.Type != EValueType::Map)
	{
		UE_LOG(LogFirebaseGoodies, Error, TEXT("UFGDemoFunctions::NotifyIos - error parsing JSON"));
		return;
	}
	
	auto AlertMap = Alert.GetValue<TMap<FString, FFGValueVariant>>();
	Title = AlertMap.Find(TEXT("title"))->GetValue<FString>();
	Body = AlertMap.Find(TEXT("body"))->GetValue<FString>();
	
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGDemoFunctions::NotifyIos - notification title: %s, body: %s"), *Title, *Body);
	
	if (Title.Equals("") || Body.Equals("")) {
		return;
	}
#if PLATFORM_IOS
	/*dispatch_async(dispatch_get_main_queue(), ^{
		if ([UIApplication sharedApplication].applicationState == UIApplicationStateActive) {
			UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:Title.GetNSString() message:Body.GetNSString() delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
			[alertView show];
		}
	});*/
#endif
}

TMap<FString, FFGValueVariant> DeserializeJsonObject(TSharedPtr<FJsonObject> Payload)
{
	TMap<FString, FFGValueVariant> Result;
    
    for (auto KeyValuePair : Payload->Values) {
        FFGValueVariant Parameter = UFGDemoFunctions::DeserealizeJsonValue(KeyValuePair.Value);
        
        if (Parameter.Type != EValueType::Null) {
            Result.Add(KeyValuePair.Key, Parameter);
        }
    }
    
    return Result;
}

TMap<FString, FFGValueVariant> UFGDemoFunctions::DeserializeJson(const FString& PayloadString)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	TMap<FString, FFGValueVariant> Result;
	
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(PayloadString);
	if (!FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		return Result;
	}
    
	Result = DeserializeJsonObject(JsonObject);
    
    return Result;
}

FFGValueVariant UFGDemoFunctions::DeserealizeJsonValue(TSharedPtr<FJsonValue> Value)
{
	FFGValueVariant Parameter;
    
    switch (Value->Type) {
        case EJson::String: {
            Parameter.Value = Value->AsString();
            Parameter.Type = EValueType::String;
            break;
        }
        case EJson::Number: {
            Parameter.Value = (float) Value->AsNumber();
            Parameter.Type = EValueType::Float;
            break;
        }
        case EJson::Boolean: {
            Parameter.Value = Value->AsBool();
            Parameter.Type = EValueType::Bool;
            break;
        }
        case EJson::Array: {
            TArray<FFGValueVariant> Parameters;
            for (auto Element : Value->AsArray()) {
                FFGValueVariant ArrayElement = DeserealizeJsonValue(Element);
                Parameters.Add(ArrayElement);
            }
            
            Parameter.Value = Parameters;
            Parameter.Type = EValueType::Array;
            break;
        }
        case EJson::Object: {
            TMap<FString, FFGValueVariant> Map = DeserializeJsonObject(Value->AsObject());
            
            Parameter.Value = Map;
            Parameter.Type = EValueType::Map;
            break;
        }
    }
    
    return Parameter;
}
