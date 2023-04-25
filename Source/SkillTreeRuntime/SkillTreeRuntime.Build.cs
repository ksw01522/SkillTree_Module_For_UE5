// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SkillTreeRuntime : ModuleRules
{
	public SkillTreeRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        bLegacyPublicIncludePaths = false;
		ShadowVariableWarningLevel = WarningLevel.Error;

        PublicIncludePaths.AddRange(new string[]
        {
            "SkillTreeRuntime/public",
        });

        PrivateIncludePaths.AddRange(new string[]
		{
			"SkillTreeRuntime/public",
			"SkillTreeRuntime/private",
        });

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "GameplayAbilities", "GameplayTasks" });

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"GameplayTags"
            }
			);

	}
}
