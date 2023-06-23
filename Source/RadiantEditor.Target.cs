// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class RadiantEditorTarget : TargetRules
{
	public RadiantEditorTarget( TargetInfo Target) : base(Target)
	{
		bUseUnityBuild = true;
		bUsePCHFiles = true;
		bUseAdaptiveUnityBuild = true;
		
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_1;
		ExtraModuleNames.Add("Radiant");
	}
}
