using UnrealBuildTool;

public class AshLineDrones : ModuleRules
{
    public AshLineDrones(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "AshLineCore", "AshLineCharacters" });
        PrivateDependencyModuleNames.AddRange(new[] { "Projects" });
        bEnableExceptions = false;
    }
}
