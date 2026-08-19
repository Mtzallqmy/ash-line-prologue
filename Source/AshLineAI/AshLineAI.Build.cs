using UnrealBuildTool;

public class AshLineAI : ModuleRules
{
    public AshLineAI(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "AIModule", "GameplayTasks", "NavigationSystem", "AshLineCore", "AshLineCombat", "AshLineCharacters", "AshLineWeapons" });
        PrivateDependencyModuleNames.AddRange(new[] { "Projects" });
        bEnableExceptions = false;
    }
}
