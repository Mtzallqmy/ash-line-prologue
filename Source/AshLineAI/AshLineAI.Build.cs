using UnrealBuildTool;

public class AshLineAI : ModuleRules
{
    public AshLineAI(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "AIModule", "GameplayTasks", "NavigationSystem", "AshLineCore", "AshLineCombat", "AshLineCharacters" });
        PrivateDependencyModuleNames.AddRange(new[] { "Projects" });
        bEnableExceptions = false;
    }
}
