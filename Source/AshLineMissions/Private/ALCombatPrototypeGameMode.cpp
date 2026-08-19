#include "ALCombatPrototypeGameMode.h"

#include "ALInfantryCharacter.h"
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
    if (DefeatedEnemyCount >= RequiredEnemyCount) CompletePrototype();
}

void AALCombatPrototypeGameMode::CompletePrototype()
{
    if (bPrototypeComplete) return;
    bPrototypeComplete = true;
    OnPrototypeCompleted.Broadcast();
}
