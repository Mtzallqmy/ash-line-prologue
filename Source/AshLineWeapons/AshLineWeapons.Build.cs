using UnrealBuildTool;

public class AshLineWeapons : ModuleRules
{
    public AshLineWeapons(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "PhysicsCore", "AshLineCore", "AshLineCombat" });
        PrivateDependencyModuleNames.AddRange(new[] { "Projects", "Niagara" });
        bEnableExceptions = false;
    }
}
