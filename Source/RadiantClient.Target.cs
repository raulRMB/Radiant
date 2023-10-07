using UnrealBuildTool;
using System.Collections.Generic;

public class RadiantClientTarget : TargetRules //Change this line according to the name of your project
{
    public RadiantClientTarget(TargetInfo Target) : base(Target) //Change this line according to the name of your project
    {
        Type = TargetType.Client;
        DefaultBuildSettings = BuildSettingsVersion.V2;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_1;
        bUseUnityBuild = false;
        bUsePCHFiles = false;
        bUseAdaptiveUnityBuild = false;
        bUseLoggingInShipping = true;
        ExtraModuleNames.Add("Radiant"); //Change this line according to the name of your project
        
    }
}