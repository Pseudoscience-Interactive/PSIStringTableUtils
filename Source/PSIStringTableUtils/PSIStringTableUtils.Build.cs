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
        
        PrivateIncludePaths.AddRange(
            new string []
            {
                Path.Combine(PluginDirectory, "Source/PSISharedHeaders/StringTables")
            }
        );
        
        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "AssetRegistry",
            }
        );
    }
}