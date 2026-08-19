using UnrealBuildTool;

public class AshLineCharacters : ModuleRules
{
    public AshLineCharacters(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "AshLineCore", "AshLineCombat", "AshLineWeapons" });
        PrivateDependencyModuleNames.AddRange(new[] { "Projects" });
        bEnableExceptions = false;
    }
}
