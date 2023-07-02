// Copyright (c) 2022 Nineva Studios

using System.IO;
using UnrealBuildTool;

#if UE_5_0_OR_LATER
using EpicGames.Core;
#else
using Tools.DotNETCommon;
#endif

public class FirebaseGoodies : ModuleRules
{
	public FirebaseGoodies(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(new[] {Path.Combine(ModuleDirectory, "Public")});
		PrivateIncludePaths.AddRange(new[] {Path.Combine(ModuleDirectory, "Private")});

		PublicDependencyModuleNames.AddRange(new[] {"Core"});

		PrivateDependencyModuleNames.AddRange(
			new[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"XmlParser",
				"Json",
				"Projects"
			}
		);

		var EnableEditorSupport = false;

		if (Target.ProjectFile != null)
		{
			var ProjectPath = Path.GetDirectoryName(Target.ProjectFile.ToString());
			var Ini = ConfigCache.ReadHierarchy(ConfigHierarchyType.Engine, new DirectoryReference(ProjectPath), Target.Platform);

			bool TempBool;
			if (Ini.TryGetValue("/Script/FirebaseGoodies.FirebaseGoodiesSettings", "bEnableEditorSupport", out TempBool))
				EnableEditorSupport = TempBool;

			if (Target.Platform == UnrealTargetPlatform.Mac && Target.Version.MajorVersion >= 5)
				EnableEditorSupport = false;

			PublicDefinitions.Add("FG_ENABLE_EDITOR_SUPPORT=" + (EnableEditorSupport ? "1" : "0"));
		}

		if (Target.Platform == UnrealTargetPlatform.Mac && EnableEditorSupport)
		{
			HandleCommonDesktop(Target);
			HandleOsX(Target);
		}

		if (Target.Platform == UnrealTargetPlatform.Win64 && EnableEditorSupport)
		{
			HandleCommonDesktop(Target);
			HandleWindows(Target);
		}

		if (Target.Platform == UnrealTargetPlatform.Android)
		{
			HandleAndroid(Target);
		}

		if (Target.Platform == UnrealTargetPlatform.IOS)
		{
			HandleIos(Target);
		}
	}

	void HandleCommonDesktop(ReadOnlyTargetRules Target)
	{
		PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "../ThirdParty", Target.Platform.ToString(), "includes"));

		PublicDefinitions.Add("INTERNAL_EXPERIMENTAL");

		PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private/Desktop"));
		PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private/RemoteConfig/Desktop"));
		PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private/Database/Desktop"));
	}

	void HandleWindows(ReadOnlyTargetRules Target)
	{
		LoadThirdPartyLibrary("firebase_app", Target);
		LoadThirdPartyLibrary("firebase_analytics", Target);
		LoadThirdPartyLibrary("firebase_auth", Target);
		LoadThirdPartyLibrary("firebase_database", Target);
		LoadThirdPartyLibrary("firebase_dynamic_links", Target);
		LoadThirdPartyLibrary("firebase_functions", Target);
		LoadThirdPartyLibrary("firebase_firestore", Target);
		LoadThirdPartyLibrary("firebase_installations", Target);
		LoadThirdPartyLibrary("firebase_messaging", Target);
		LoadThirdPartyLibrary("firebase_remote_config", Target);
		LoadThirdPartyLibrary("firebase_storage", Target);

		PublicSystemLibraries.Add("crypt32.lib");
		PublicSystemLibraries.Add("advapi32.lib");
		PublicSystemLibraries.Add("ws2_32.lib");
		PublicSystemLibraries.Add("rpcrt4.lib");
		PublicSystemLibraries.Add("ole32.lib");
		PublicSystemLibraries.Add("shell32.lib");
		PublicSystemLibraries.Add("iphlpapi.lib");
		PublicSystemLibraries.Add("psapi.lib");
		PublicSystemLibraries.Add("userenv.lib");
	}

	void HandleOsX(ReadOnlyTargetRules Target)
	{
		LoadThirdPartyLibrary("libfirebase_app", Target);
		LoadThirdPartyLibrary("libfirebase_analytics", Target);
		LoadThirdPartyLibrary("libfirebase_auth", Target);
		LoadThirdPartyLibrary("libfirebase_database", Target);
		LoadThirdPartyLibrary("libfirebase_dynamic_links", Target);
		LoadThirdPartyLibrary("libfirebase_functions", Target);
		LoadThirdPartyLibrary("libfirebase_firestore", Target);
		LoadThirdPartyLibrary("libfirebase_installations", Target);
		LoadThirdPartyLibrary("libfirebase_messaging", Target);
		LoadThirdPartyLibrary("libfirebase_remote_config", Target);
		LoadThirdPartyLibrary("libfirebase_storage", Target);

		PublicFrameworks.AddRange(
			new[]
			{
				"CoreFoundation",
				"Foundation",
				"Security",
				"SystemConfiguration",
				"GSS",
				"Kerberos",
			}
		);
	}

	void HandleIos(ReadOnlyTargetRules Target)
	{
		PublicFrameworks.AddRange(
			new[]
			{
				"LocalAuthentication",
				"SafariServices",
				"AuthenticationServices",
				"SystemConfiguration",
				"UserNotifications"
			}
		);

		PublicAdditionalFrameworks.Add(new Framework("FirebaseABTesting", "../ThirdParty/IOS/FirebaseABTesting.embeddedframework.zip"));
		PublicAdditionalFrameworks.Add(new Framework("FirebaseAnalytics", "../ThirdParty/IOS/FirebaseAnalytics.embeddedframework.zip"));
		PublicAdditionalFrameworks.Add(new Framework("FirebaseCore", "../ThirdParty/IOS/FirebaseCore.embeddedframework.zip"));
		PublicAdditionalFrameworks.Add(new Framework("FirebaseCoreDiagnostics", "../ThirdParty/IOS/FirebaseCoreDiagnostics.embeddedframework.zip"));
		PublicAdditionalFrameworks.Add(new Framework("FirebaseDatabase", "../ThirdParty/IOS/FirebaseDatabase.embeddedframework.zip"));
		PublicAdditionalFrameworks.Add(new Framework("FirebaseInstallations", "../ThirdParty/IOS/FirebaseInstallations.embeddedframework.zip"));
		PublicAdditionalFrameworks.Add(new Framework("FirebaseRemoteConfig", "../ThirdParty/IOS/FirebaseRemoteConfig.embeddedframework.zip"));
		PublicAdditionalFrameworks.Add(new Framework("GoogleAppMeasurement", "../ThirdParty/IOS/GoogleAppMeasurement.embeddedframework.zip"));
		PublicAdditionalFrameworks.Add(new Framework("GoogleAppMeasurementIdentitySupport", "../ThirdParty/IOS/GoogleAppMeasurementIdentitySupport.embeddedframework.zip"));
		PublicAdditionalFrameworks.Add(new Framework("GoogleDataTransport", "../ThirdParty/IOS/GoogleDataTransport.embeddedframework.zip"));
		PublicAdditionalFrameworks.Add(new Framework("GoogleUtilities", "../ThirdParty/IOS/GoogleUtilities.embeddedframework.zip"));
		PublicAdditionalFrameworks.Add(new Framework("leveldb-library", "../ThirdParty/IOS/leveldb-library.embeddedframework.zip"));
		PublicAdditionalFrameworks.Add(new Framework("nanopb", "../ThirdParty/IOS/nanopb.embeddedframework.zip"));
		PublicAdditionalFrameworks.Add(new Framework("PromisesObjC", "../ThirdParty/IOS/PromisesObjC.embeddedframework.zip"));
		PublicAdditionalFrameworks.Add(new Framework("FirebaseAuth", "../ThirdParty/IOS/FirebaseAuth.embeddedframework.zip"));
		PublicAdditionalFrameworks.Add(new Framework("GoogleSignIn", "../ThirdParty/IOS/GoogleSignIn.embeddedframework.zip", "Resources/GoogleSignIn.bundle"));
		PublicAdditionalFrameworks.Add(new Framework("AppAuth", "../ThirdParty/IOS/AppAuth.embeddedframework.zip"));
		PublicAdditionalFrameworks.Add(new Framework("GTMAppAuth", "../ThirdParty/IOS/GTMAppAuth.embeddedframework.zip"));
		PublicAdditionalFrameworks.Add(new Framework("FirebaseStorage", "../ThirdParty/IOS/FirebaseStorage.embeddedframework.zip"));
		PublicAdditionalFrameworks.Add(new Framework("FirebaseCrashlytics", "../ThirdParty/IOS/FirebaseCrashlytics.embeddedframework.zip"));
		PublicAdditionalFrameworks.Add(new Framework("FirebaseMessaging", "../ThirdParty/IOS/FirebaseMessaging.embeddedframework.zip"));
		PublicAdditionalFrameworks.Add(new Framework("FirebaseFunctions", "../ThirdParty/IOS/FirebaseFunctions.embeddedframework.zip"));
		PublicAdditionalFrameworks.Add(new Framework("GTMSessionFetcher", "../ThirdParty/IOS/GTMSessionFetcher.embeddedframework.zip"));

		PublicAdditionalFrameworks.Add(new Framework("abseil", "../ThirdParty/IOS/abseil.embeddedframework.zip"));
		PublicAdditionalFrameworks.Add(new Framework("BoringSSL-GRPC", "../ThirdParty/IOS/BoringSSL-GRPC.embeddedframework.zip"));
		PublicAdditionalFrameworks.Add(new Framework("FirebaseFirestore", "../ThirdParty/IOS/FirebaseFirestore.embeddedframework.zip"));
		PublicAdditionalFrameworks.Add(new Framework("gRPC-C++", "../ThirdParty/IOS/gRPC-C++.embeddedframework.zip", "Resources/gRPCCertificates-Cpp.bundle"));
		PublicAdditionalFrameworks.Add(new Framework("gRPC-Core", "../ThirdParty/IOS/gRPC-Core.embeddedframework.zip"));

		PublicAdditionalFrameworks.Add(new Framework("FirebaseDynamicLinks", "../ThirdParty/IOS/FirebaseDynamicLinks.embeddedframework.zip"));

		PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private/IOS"));
		PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private/RemoteConfig/IOS"));
		PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private/Database/IOS"));

		var EnableDynamicLinksForIos = false;

		if (Target.ProjectFile != null)
		{
			var ProjectPath = Path.GetDirectoryName(Target.ProjectFile.ToString());
			var Ini = UnrealBuildTool.ConfigCache.ReadHierarchy(ConfigHierarchyType.Engine, new DirectoryReference(ProjectPath), Target.Platform);

			bool TempBool;
			if (Ini.TryGetValue("/Script/FirebaseGoodies.FirebaseGoodiesSettings", "EnableDynamicLinksForIos", out TempBool))
				EnableDynamicLinksForIos = TempBool;
		}

		PublicDefinitions.Add("DYNAMIC_LINKS_IOS=" + (EnableDynamicLinksForIos ? "1" : "0"));

		PublicDependencyModuleNames.AddRange(new[] {"Launch"});
		PrivateDependencyModuleNames.AddRange(new[] {"Launch"});
		var PluginPath = Utils.MakePathRelativeTo(ModuleDirectory, Target.RelativeEnginePath);

		AdditionalPropertiesForReceipt.Add("IOSPlugin", Path.Combine(PluginPath, "FirebaseGoodies_IOS_UPL.xml"));
	}

	void HandleAndroid(ReadOnlyTargetRules Target)
	{
		PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private/Android"));
		PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private/RemoteConfig/Android"));
		PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private/Database/Android"));

		PublicDependencyModuleNames.AddRange(new[] {"Launch"});
		PrivateDependencyModuleNames.AddRange(new[] {"Launch"});
		var PluginPath = Utils.MakePathRelativeTo(ModuleDirectory, Target.RelativeEnginePath);

		AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(PluginPath, "FirebaseGoodies_Android_UPL.xml"));
	}

	public void LoadThirdPartyLibrary(string libname, ReadOnlyTargetRules Target)
	{
		var StaticLibExtension = string.Empty;

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			StaticLibExtension = ".lib";
		}

		if (Target.Platform == UnrealTargetPlatform.Mac)
		{
			StaticLibExtension = ".a";
		}

		var ThirdPartyPath = Path.Combine(ModuleDirectory, "../ThirdParty", Target.Platform.ToString());
		var LibrariesPath = Path.Combine(ThirdPartyPath, "libraries");

		// Link static library (desktop only)
		if (Target.Platform == UnrealTargetPlatform.Win64 || Target.Platform == UnrealTargetPlatform.Mac)
		{
			PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, libname + StaticLibExtension));
		}
	}
}
