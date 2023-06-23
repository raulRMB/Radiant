// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class RadiantTarget : TargetRules
{
	public RadiantTarget( TargetInfo Target) : base(Target)
	{
		bUseUnityBuild = true;
		bUsePCHFiles = true;
		bUseAdaptiveUnityBuild = true;
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_1;
		ExtraModuleNames.Add("Radiant");
	}
}
