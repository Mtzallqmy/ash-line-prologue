using UnrealBuildTool;

public class AshLinePlatform : ModuleRules
{
    public AshLinePlatform(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "AshLineCore", "AshLineContent" });
        PrivateDependencyModuleNames.AddRange(new[] { "Projects" });
        bEnableExceptions = false;
    }
}
