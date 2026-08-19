#include "World/ALEnemySpawner.h"
#include "ALInfantryCharacter.h"
#include "Data/ALAIArchetypeData.h"
#include "Engine/World.h"

AALEnemySpawner::AALEnemySpawner()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AALEnemySpawner::BeginPlay()
{
    Super::BeginPlay();
    if (bSpawnOnBeginPlay) ActivateSpawner();
}

void AALEnemySpawner::ActivateSpawner()
{
    if (bActivated && bOneShot) return;
    if (!GetWorld() || !EnemyClass) return;
    bActivated = true;
    const int32 SpawnCount = FMath::Min(FMath::Max(Count, 0), SpawnPoints.Num());
    for (int32 Index = 0; Index < SpawnCount; ++Index)
    {
        const FTransform SpawnTransform = SpawnPoints.IsValidIndex(Index) ? SpawnPoints[Index] * GetActorTransform() : GetActorTransform();
        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
        if (AALInfantryCharacter* Enemy = GetWorld()->SpawnActor<AALInfantryCharacter>(EnemyClass, SpawnTransform, Params))
        {
            Enemy->Archetype = Archetype;
            SpawnedEnemies.Add(Enemy);
        }
    }
}

void AALEnemySpawner::DeactivateSpawner()
{
    bActivated = false;
    for (TObjectPtr<AALInfantryCharacter>& Enemy : SpawnedEnemies) if (IsValid(Enemy)) Enemy->Destroy();
    SpawnedEnemies.Reset();
}
