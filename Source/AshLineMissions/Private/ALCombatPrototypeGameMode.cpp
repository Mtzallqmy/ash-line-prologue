#include "ALCombatPrototypeGameMode.h"

#include "ALInfantryCharacter.h"
#include "ALMissionSubsystem.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

AALCombatPrototypeGameMode::AALCombatPrototypeGameMode()
{
    RequiredEnemyCount = 6;
}

void AALCombatPrototypeGameMode::BeginPlay()
{
    Super::BeginPlay();

    TArray<AActor*> ExistingEnemies;
    UGameplayStatics::GetAllActorsOfClass(this, AALInfantryCharacter::StaticClass(), ExistingEnemies);
    for (AActor* Actor : ExistingEnemies) RegisterEnemy(Cast<AALInfantryCharacter>(Actor));

    if (RequiredEnemyCount <= 0) RequiredEnemyCount = RegisteredEnemies.Num();
    OnEnemyCountChanged.Broadcast(DefeatedEnemyCount, GetRemainingEnemyCount());
    if (UALMissionSubsystem* MissionSubsystem = GetWorld() ? GetWorld()->GetSubsystem<UALMissionSubsystem>() : nullptr)
    {
        MissionSubsystem->StartMission(PrototypeMissionId);
        MissionSubsystem->ActivateObjective(CombatObjectiveIndex);
        MissionSubsystem->SetActiveObjectiveProgress(RequiredEnemyCount > 0 ? 0.0f : 1.0f);
    }
}

void AALCombatPrototypeGameMode::RegisterEnemy(AALInfantryCharacter* Enemy)
{
    if (!IsValid(Enemy) || RegisteredEnemies.Contains(Enemy)) return;
    RegisteredEnemies.Add(Enemy);
    Enemy->OnEnemyKilled.AddUniqueDynamic(this, &AALCombatPrototypeGameMode::HandleEnemyKilled);
}

int32 AALCombatPrototypeGameMode::GetRemainingEnemyCount() const
{
    return FMath::Max(0, RequiredEnemyCount - DefeatedEnemyCount);
}

void AALCombatPrototypeGameMode::HandleEnemyKilled(AActor* EnemyActor)
{
    AALInfantryCharacter* Enemy = Cast<AALInfantryCharacter>(EnemyActor);
    if (!IsValid(Enemy) || !RegisteredEnemies.Contains(Enemy) || DefeatedEnemies.Contains(Enemy)) return;
    DefeatedEnemies.Add(Enemy);
    ++DefeatedEnemyCount;
    OnEnemyCountChanged.Broadcast(DefeatedEnemyCount, GetRemainingEnemyCount());
    if (UALMissionSubsystem* MissionSubsystem = GetWorld() ? GetWorld()->GetSubsystem<UALMissionSubsystem>() : nullptr)
    {
        const float Progress = RequiredEnemyCount > 0 ? static_cast<float>(DefeatedEnemyCount) / static_cast<float>(RequiredEnemyCount) : 1.0f;
        MissionSubsystem->SetActiveObjectiveProgress(Progress);
    }
    if (DefeatedEnemyCount >= RequiredEnemyCount) CompletePrototype();
}

void AALCombatPrototypeGameMode::CompletePrototype()
{
    if (bPrototypeComplete) return;
    bPrototypeComplete = true;
    if (UALMissionSubsystem* MissionSubsystem = GetWorld() ? GetWorld()->GetSubsystem<UALMissionSubsystem>() : nullptr)
    {
        MissionSubsystem->CompleteActiveObjective();
        MissionSubsystem->CompleteMission(PrototypeMissionId);
    }
    OnPrototypeCompleted.Broadcast();
}
