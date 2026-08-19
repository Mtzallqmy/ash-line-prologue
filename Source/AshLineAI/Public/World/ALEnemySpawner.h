#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ALEnemySpawner.generated.h"

class AALInfantryCharacter;
class UALAIArchetypeData;

UCLASS(Blueprintable)
class ASHLINEAI_API AALEnemySpawner : public AActor
{
    GENERATED_BODY()
public:
    AALEnemySpawner();
    virtual void BeginPlay() override;
    UFUNCTION(BlueprintCallable, Category="ASH LINE|AI") void ActivateSpawner();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|AI") void DeactivateSpawner();
    UFUNCTION(BlueprintPure, Category="ASH LINE|AI") bool IsActivated() const { return bActivated; }

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spawn") TSubclassOf<AALInfantryCharacter> EnemyClass;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spawn") TObjectPtr<UALAIArchetypeData> Archetype;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawn") int32 Count = 3;
    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="Spawn") TArray<FTransform> SpawnPoints;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawn") bool bSpawnOnBeginPlay = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawn") bool bOneShot = true;

protected:
    UPROPERTY(BlueprintReadOnly, Category="Spawn") bool bActivated = false;
    UPROPERTY() TArray<TObjectPtr<AALInfantryCharacter>> SpawnedEnemies;
};
