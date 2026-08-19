using UnrealBuildTool;
using System.Collections.Generic;

public class ASH_LINEEditorTarget : TargetRules
{
    public ASH_LINEEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;
        ExtraModuleNames.AddRange(new[] {
            "AshLineCore", "AshLineCharacters", "AshLineCombat", "AshLineWeapons",
            "AshLineAI", "AshLineDrones", "AshLineMissions", "AshLineContent",
            "AshLineUI", "AshLinePlatform"
        });
    }
}
