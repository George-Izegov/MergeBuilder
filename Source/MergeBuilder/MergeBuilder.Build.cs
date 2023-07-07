// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MergeBuilder : ModuleRules
{
	public MergeBuilder(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "OnlineSubsystemUtils", "PsIronSource", "AndroidPlayBilling"});

        PrivateDependencyModuleNames.AddRange(new string[] { "Http", "Json", "JsonUtilities", "Paper2D", "FirebaseGoodies" });

        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore", "UMG" });
        

        PublicIncludePaths.AddRange(new string[]
        {
            "MergeBuilder/Public",
            "MergeBuilder/Public/MergeSystem",
            "MergeBuilder/Public/Utilities",
        });
    }
}
