// Copyright 2025-2026 Pseudoscience Interactive. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class PSIStringTableUtilsEditor : ModuleRules
{
    public PSIStringTableUtilsEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
            new string []
            {
                Path.Combine(PluginDirectory, "Source/PSISharedHeaders/Macros")
            }
        );
        
        PrivateIncludePaths.AddRange(
            new string []
            {
                Path.Combine(PluginDirectory, "Source/PSISharedHeaders/StringTables"),
                Path.Combine(PluginDirectory, "Source/PSISharedHeaders/StringTablesEditor")
            }
        );
        
        PublicDependencyModuleNames.AddRange(
            new string[]
            {
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Engine",
                "Core",
                "CoreUObject",
                "Slate",
                "SlateCore",
                "PropertyEditor",
                "UnrealEd",
                "GraphEditor",
                "BlueprintGraph",
                
                // PSI
                "PSIStringTableUtils"
            }
        );
    }
}