// Copyright 2018 Pavlov Dmitriy

using UnrealBuildTool;

public class LevelGenPluginEditor : ModuleRules
{
	public LevelGenPluginEditor(ReadOnlyTargetRules ROTargetRules) : base(ROTargetRules)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs; // Enabel IWYU
         
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

        PrivateDependencyModuleNames.AddRange(new string[] {
            "Slate",
            "SlateCore",
            "PropertyEditor", // Detail panel cotomization
            "UnrealEd", //Asset factory
            "EditorStyle",
            "GraphEditor",
        });

        PrivateDependencyModuleNames.AddRange(new string[] { "LevelGenPluginRuntime" });

		
	}
}
