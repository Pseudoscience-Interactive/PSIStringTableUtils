// Copyright 2025-2026 Pseudoscience Interactive. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class PSIStringTableUtils : ModuleRules
{
    public PSIStringTableUtils(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        
        PublicIncludePaths.AddRange(
            new string []
            {
                Path.Combine(PluginDirectory, "Source/PSISharedHeaders/Macros"),
                Path.Combine(PluginDirectory, "Source/PSISharedHeaders/TypeTraits")
            }
        );
        
        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "UnrealEd",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {

            }
        );        
    }
}