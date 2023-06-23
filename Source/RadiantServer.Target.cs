using UnrealBuildTool;
using System.Collections.Generic;

public class RadiantServerTarget : TargetRules //Change this line according to the name of your project
{
    public RadiantServerTarget(TargetInfo Target) : base(Target) //Change this line according to the name of your project
    {
        bUseUnityBuild = true;
        bUsePCHFiles = true;
        bUseAdaptiveUnityBuild = true;
        
        Type = TargetType.Server;
        DefaultBuildSettings = BuildSettingsVersion.V2;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_1;
        ExtraModuleNames.Add("Radiant"); //Change this line according to the name of your project
        BuildEnvironment = TargetBuildEnvironment.Unique;
        bUseLoggingInShipping = true;
    }
}