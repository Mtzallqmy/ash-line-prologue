using UnrealBuildTool;
using System.Collections.Generic;

public class ASH_LINETarget : TargetRules
{
    public ASH_LINETarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;
        ExtraModuleNames.AddRange(new[] {
            "AshLineCore", "AshLineCharacters", "AshLineCombat", "AshLineWeapons",
            "AshLineAI", "AshLineDrones", "AshLineMissions", "AshLineContent",
            "AshLineUI", "AshLinePlatform"
        });
    }
}
