// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MergeBuilder : ModuleRules
{
	public MergeBuilder(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

        PrivateDependencyModuleNames.AddRange(new string[] { "Http", "Json", "JsonUtilities", "Paper2D" });

        PublicIncludePaths.AddRange(new string[]
        {
            "MergeBuilder/Public",
            "MergeBuilder/Public/MergeSystem",
            "MergeBuilder/Public/Utilities",
        });
    }
}
