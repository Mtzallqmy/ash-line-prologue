using UnrealBuildTool;

public class AshLineMissions : ModuleRules
{
    public AshLineMissions(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "AshLineCore", "AshLineCharacters", "AshLineAI", "AshLineDrones" });
        PrivateDependencyModuleNames.AddRange(new[] { "Projects" });
        bEnableExceptions = false;
    }
}
