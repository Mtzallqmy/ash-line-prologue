using UnrealBuildTool;

public class AshLineUI : ModuleRules
{
    public AshLineUI(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "UMG", "Slate", "SlateCore", "AshLineCore", "AshLineCharacters", "AshLineWeapons", "AshLineDrones", "AshLineMissions" });
        PrivateDependencyModuleNames.AddRange(new[] { "Projects" });
        bEnableExceptions = false;
    }
}
