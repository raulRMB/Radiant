// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Radiant : ModuleRules
{
	public Radiant(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"UMG",
			"Json",
			"JsonUtilities",
			"NiagaraCore",
			"Niagara",
			"Slate",
			"SlateCore",
			"AIModule",
			"NavigationSystem",
			"CableComponent"
		});
		bEnableUndefinedIdentifierWarnings = false;

		PrivateDependencyModuleNames.AddRange(new[] { "GameplayAbilities", "GameplayTags", "GameplayTasks" });

		PublicIncludePaths.Add("Radiant/");

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}