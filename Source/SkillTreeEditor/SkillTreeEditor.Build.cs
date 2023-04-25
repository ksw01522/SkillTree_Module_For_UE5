// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SkillTreeEditor : ModuleRules
{
	public SkillTreeEditor(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        bLegacyPublicIncludePaths = false;
		ShadowVariableWarningLevel = WarningLevel.Error;

		PrivateIncludePaths.AddRange(new string[]
		{
			"SkillTreeEditor/public",
		});

		PrivateIncludePaths.AddRange(new string[]
		{
			"SkillTreeEditor/private",
			"SkillTreeRuntime/public"
		});

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "UnrealEd" });

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"SkillTreeRuntime",
				"AssetTools",
				"Slate",
				"InputCore",
				"SlateCore",
				"GraphEditor",
				"PropertyEditor",
				"EditorStyle",
				"Kismet",
				"KismetWidgets",
				"ApplicationCore",
				"ToolMenus",
				// ... add private dependencies that you statically link with here ...
			}
			);

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
