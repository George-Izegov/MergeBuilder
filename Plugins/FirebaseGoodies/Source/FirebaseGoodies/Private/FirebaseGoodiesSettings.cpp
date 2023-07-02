// Copyright (c) 2022 Nineva Studios

#include "FirebaseGoodiesSettings.h"

#include "FirebaseGoodiesLog.h"
#include "Json/Public/Dom/JsonValue.h"
#include "Json/Public/Dom/JsonObject.h"
#include "Json/Public/Serialization/JsonReader.h"
#include "Json/Public/Serialization/JsonSerializer.h"
#include "XmlParser/Public/XmlFile.h"
#include "XmlParser/Public/XmlNode.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"

#if WITH_EDITOR
void UFirebaseGoodiesSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName Name = (PropertyChangedEvent.MemberProperty != nullptr) ? PropertyChangedEvent.MemberProperty->GetFName() : NAME_None;

	if (Name == FName(TEXT("GoogleServicesJsonPath")) && !GoogleServicesJsonPath.IsEmpty())
	{
		//GoogleServicesJsonPath.FilePath = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*GoogleServicesJsonPath.FilePath);
		ParseJson();
		//SaveConfig(CPF_Config, *GetDefaultConfigFilename());
		SaveConfig();
	}

	if (Name == FName(TEXT("GoogleServicesPlistPath")) && !GoogleServicesPlistPath.FilePath.IsEmpty())
	{
		GoogleServicesPlistPath.FilePath = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*GoogleServicesPlistPath.FilePath);
		ParsePlist();
		//SaveConfig(CPF_Config, *GetDefaultConfigFilename());
		SaveConfig();
	}

	if (Name == FName(TEXT("bEnableAPNForIOS")))
	{
		SaveConfig(CPF_Config, *GetDefaultConfigFilename());
		HandleAPNState();
	}

	if (Name == FName(TEXT("AndroidDynamicLinks")))
	{
		ParseAndSaveXmlAndroid();
	}

	if (Name == FName(TEXT("URLSchemes")) || Name == FName(TEXT("ReversedClientId")))
	{
		ParseAndSaveUrlSchemesIos();
	}

	if (Name == FName(TEXT("FirebaseDynamicLinksCustomDomains")))
	{
		ParseAndSaveFirebaseDynamicLinksCustomDomains();
	}
}

void UFirebaseGoodiesSettings::ParseJson()
{
	FString FileContents;
	if (!FFileHelper::LoadFileToString(FileContents, *GoogleServicesJsonPath))
	{
		UE_LOG(LogFirebaseGoodies, Error, TEXT("Failed to load google-services.json"));
		return;
	}

	TSharedPtr<FJsonObject> GoogleServicesJson;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(FileContents);
	if (!FJsonSerializer::Deserialize(Reader, GoogleServicesJson))
	{
		UE_LOG(LogFirebaseGoodies, Error, TEXT("Failed to parse google-services.json"));
		return;
	}

	TArray<TSharedPtr<FJsonValue>> ClientArray = GoogleServicesJson->GetArrayField(TEXT("client"));
	for (const auto ClientJson : ClientArray)
	{
		const TSharedPtr<FJsonObject>* ClientJsonObject;
		if (ClientJson->TryGetObject(ClientJsonObject))
		{
			TArray<TSharedPtr<FJsonValue>> OauthClientArray = (*ClientJsonObject)->GetArrayField(TEXT("oauth_client"));
			for (const auto OauthClientJson : OauthClientArray)
			{
				const TSharedPtr<FJsonObject>* OauthClientJsonObject;
				if (OauthClientJson->TryGetObject(OauthClientJsonObject) && (*OauthClientJsonObject)->GetNumberField(TEXT("client_type")) == 3)
				{
					AndroidOauthClientID = (*OauthClientJsonObject)->GetStringField(TEXT("client_id"));
					UE_LOG(LogFirebaseGoodies, Display, TEXT("CLIENT_ID : %s"), *AndroidOauthClientID);
					return; // we found what we were looking for
				}
			}
		}
	}
}

void UFirebaseGoodiesSettings::ParsePlist()
{
	FXmlFile GoogleServicesInfoPlist;
	GoogleServicesInfoPlist.LoadFile(GoogleServicesPlistPath.FilePath);

	if (!GoogleServicesInfoPlist.IsValid())
	{
		UE_LOG(LogFirebaseGoodies, Error, TEXT("Failed to load GoogleService-Info.plist"));
		return;
	}

	const FXmlNode* GoogleServicesInfoPlistRoot = GoogleServicesInfoPlist.GetRootNode();
	if (!GoogleServicesInfoPlistRoot)
	{
		UE_LOG(LogFirebaseGoodies, Error, TEXT("Failed to get GoogleService-Info.plist root node"));
		return;
	}

	const FString GoogleServicesInfoPlistRootName = GoogleServicesInfoPlistRoot->GetTag();
	if (GoogleServicesInfoPlistRootName != TEXT("plist"))
	{
		UE_LOG(LogFirebaseGoodies, Error, TEXT("GoogleService-Info.plist has invalid content"));
		return;
	}

	const FXmlNode* FirebaseSettingsDictPlistNode = GoogleServicesInfoPlistRoot->GetFirstChildNode();
	if (!FirebaseSettingsDictPlistNode)
	{
		UE_LOG(LogFirebaseGoodies, Error, TEXT("Dictionary with Firebase settings is missing in GoogleService-Info.plist"));
		return;
	}

	const FXmlNode* KeyItemNode = FirebaseSettingsDictPlistNode->GetFirstChildNode();
	if (!KeyItemNode)
	{
		UE_LOG(LogFirebaseGoodies, Error, TEXT("Dictionary with Firebase settings is empty"));
		return;
	}

	TMap<FString, FString> FirebaseStringSettings;
	TMap<FString, bool> FirebaseBoolSettings;

	while (KeyItemNode)
	{
		if (KeyItemNode->GetTag() != TEXT("key"))
		{
			UE_LOG(LogFirebaseGoodies, Error, TEXT("Dictionary with Firebase settings contains item with invalid tag. Tag \"key\" is expected"));
			return;
		}

		FString KeyItemNodeContent = KeyItemNode->GetContent();

		const FXmlNode* ValueItemNode = KeyItemNode->GetNextNode();
		if (!ValueItemNode)
		{
			UE_LOG(LogFirebaseGoodies, Error, TEXT("Dictionary with Firebase settings doesn't contain value for key %s"), *KeyItemNodeContent);
			return;
		}

		if (ValueItemNode->GetTag() == TEXT("string"))
		{
			FString ValueItemNodeContent = ValueItemNode->GetContent();
			UE_LOG(LogFirebaseGoodies, Display, TEXT("%s : %s"), *KeyItemNodeContent, *ValueItemNodeContent);
			FirebaseStringSettings.Add(KeyItemNodeContent, ValueItemNodeContent);
		}
		else if (ValueItemNode->GetTag() == TEXT("true"))
		{
			UE_LOG(LogFirebaseGoodies, Display, TEXT("%s : TRUE"), *KeyItemNodeContent);
			FirebaseBoolSettings.Add(KeyItemNodeContent, true);
		}
		else if (ValueItemNode->GetTag() == TEXT("false"))
		{
			UE_LOG(LogFirebaseGoodies, Display, TEXT("%s : FALSE"), *KeyItemNodeContent);
			FirebaseBoolSettings.Add(KeyItemNodeContent, false);
		}

		KeyItemNode = ValueItemNode->GetNextNode();
	}

	ApiKey = FirebaseStringSettings.FindRef(TEXT("API_KEY"));
	BundleId = FirebaseStringSettings.FindRef(TEXT("BUNDLE_ID"));
	ClientId = FirebaseStringSettings.FindRef(TEXT("CLIENT_ID"));
	ReversedClientId = FirebaseStringSettings.FindRef(TEXT("REVERSED_CLIENT_ID"));
	GcmSenderId = FirebaseStringSettings.FindRef(TEXT("GCM_SENDER_ID"));
	ProjectId = FirebaseStringSettings.FindRef(TEXT("PROJECT_ID"));
	AndroidClientId = FirebaseStringSettings.FindRef(TEXT("ANDROID_CLIENT_ID"));
	GoogleAppId = FirebaseStringSettings.FindRef(TEXT("GOOGLE_APP_ID"));
	DatabaseUrl = FirebaseStringSettings.FindRef(TEXT("DATABASE_URL"));
	StorageBucket = FirebaseStringSettings.FindRef(TEXT("STORAGE_BUCKET"));
}

void UFirebaseGoodiesSettings::HandleAPNState()
{
	if (GConfig)
	{
		const FString Section = "/Script/IOSRuntimeSettings.IOSRuntimeSettings";
		const FString ConfigFileName = IFileManager::Get().ConvertToAbsolutePathForExternalAppForWrite(*(FPaths::ProjectConfigDir() + "DefaultEngine.ini"));
		const FString Value = bEnableAPNForIOS ? "True" : "False";
		UE_LOG(LogFirebaseGoodies, Verbose, TEXT("Writing to config: %s"), *ConfigFileName);
		GConfig->SetString(*Section,
		                   TEXT("bEnableRemoteNotificationsSupport"),
		                   *Value,
		                   ConfigFileName);
		GConfig->Flush(false, ConfigFileName);
	}
}

void UFirebaseGoodiesSettings::ParseAndSaveXmlAndroid()
{
	FString Output;

	if (AndroidDynamicLinks.Num() < 1)
	{
		Output = "";
	}
	else
	{
		FString Entries;
		for (auto Entry : AndroidDynamicLinks)
		{
			if (Entry.Scheme.IsEmpty() && Entry.Host.IsEmpty() && Entry.PathPrefix.IsEmpty())
			{
				UE_LOG(LogFirebaseGoodies, Warning, TEXT("All of the elements in the deep link entry are empty. Please, fill, at least one."));
				continue;
			}

			FString AutoVerify = Entry.AutoVerifyDomain ? " android:autoVerify=\"true\"" : "";
			Entries += FString::Printf(
				TEXT(
					"<intent-filter%s> <action android:name=\"android.intent.action.VIEW\"/> <category android:name=\"android.intent.category.DEFAULT\"/> <category android:name=\"android.intent.category.BROWSABLE\"/> <data "),
				*AutoVerify);
			if (!Entry.Scheme.IsEmpty())
			{
				Entries += FString::Printf(TEXT("android:scheme=\"%s\" "), *Entry.Scheme);
			}
			if (!Entry.Host.IsEmpty())
			{
				Entries += FString::Printf(TEXT("android:host=\"%s\" "), *Entry.Host);
			}
			if (!Entry.PathPrefix.IsEmpty())
			{
				Entries += FString::Printf(TEXT("android:pathPrefix=\"%s\""), *Entry.PathPrefix);
			}

			Entries += "/> </intent-filter> ";

			Output = FString::Printf(
				TEXT(
					"<activity android:name=\"com.ninevastudios.unrealfirebase.FGDynamicLinksActivity\" android:launchMode=\"singleTask\" android:excludeFromRecents=\"true\" android:theme=\"@android:style/Theme.Translucent.NoTitleBar\" android:exported=\"false\"> %s </activity>"),
				*Entries);
		}
	}

	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("Android Application Tag Additions: %s"), *Output);

	AddApplicationTags(Output);
}

void UFirebaseGoodiesSettings::AddApplicationTags(const FString& TagsString)
{
	if (GConfig)
	{
		const FString PropertyName = "ExtraApplicationSettings";
		const FString Section = "/Script/AndroidRuntimeSettings.AndroidRuntimeSettings";
		const FString ConfigFileName = IFileManager::Get().ConvertToAbsolutePathForExternalAppForWrite(*(FPaths::ProjectConfigDir() + "DefaultEngine.ini"));
		UE_LOG(LogFirebaseGoodies, Verbose, TEXT("Writing to config: %s"), *ConfigFileName);

		FString Tags = GConfig->GetStr(*Section, *PropertyName, ConfigFileName);

		const FString TagBeginning = "<activity android:name=\"com.ninevastudios.unrealfirebase.FGDynamicLinksActivity\"";
		const FString TagEnding = "</activity>";

		if (!Tags.Contains(TagBeginning))
		{
			Tags += TagsString;
		}
		else
		{
			FString BeforeTag, AfterTag, Intermediate1, Intermediate2;

			if (!Tags.Split(TagBeginning, &BeforeTag, &AfterTag))
			{
				BeforeTag = "";
				AfterTag = Tags;
			}

			if (!AfterTag.Split(TagEnding, &Intermediate1, &Intermediate2))
			{
				Intermediate1 = AfterTag;
				Intermediate2 = "";
			}

			AfterTag = Intermediate2;

			Tags = BeforeTag + TagsString + AfterTag;

			BeforeTag = AfterTag = Intermediate1 = Intermediate2 = "";
		}

		GConfig->SetString(*Section, *PropertyName, *Tags, ConfigFileName);
		GConfig->Flush(true, ConfigFileName);
	}
}

void UFirebaseGoodiesSettings::ParseAndSaveUrlSchemesIos()
{
	if (URLSchemes.Num() == 0)
	{
		URLSchemesString = "";
		SaveConfig();
		return;
	}

	FString Output = "<dict><key>CFBundleURLSchemes</key><array>";

	for (auto Entry : URLSchemes)
	{
		Output += "<string>" + Entry + "</string>";
	}

	Output += "<string>" + ReversedClientId + "</string>";

	Output += "</array></dict>";

	URLSchemesString = Output;
	SaveConfig();
}

void UFirebaseGoodiesSettings::ParseAndSaveFirebaseDynamicLinksCustomDomains()
{
	if (FirebaseDynamicLinksCustomDomains.Num() == 0)
	{
		FirebaseDynamicLinksCustomDomainsString = "";
		SaveConfig();
		return;
	}

	FString Output = "";

	for (auto Entry : FirebaseDynamicLinksCustomDomains)
	{
		Output += "<string>" + Entry + "</string>";
	}

	FirebaseDynamicLinksCustomDomainsString = Output;
	SaveConfig();
}
#endif
