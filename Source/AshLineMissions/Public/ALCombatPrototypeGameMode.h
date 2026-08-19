#pragma once

#include "CoreMinimal.h"
#include "ALPrologueGameMode.h"
#include "ALCombatPrototypeGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FALCombatPrototypeEnemyCountChanged, int32, DefeatedCount, int32, RemainingCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FALCombatPrototypeCompleted);

UCLASS(Blueprintable)
class ASHLINEMISSIONS_API AALCombatPrototypeGameMode : public AALPrologueGameMode
{
    GENERATED_BODY()

public:
    AALCombatPrototypeGameMode();
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category="ASH LINE|Prototype") void RegisterEnemy(class AALInfantryCharacter* Enemy);
    UFUNCTION(BlueprintPure, Category="ASH LINE|Prototype") int32 GetDefeatedEnemyCount() const { return DefeatedEnemyCount; }
    UFUNCTION(BlueprintPure, Category="ASH LINE|Prototype") int32 GetRemainingEnemyCount() const;
    UFUNCTION(BlueprintPure, Category="ASH LINE|Prototype") bool IsPrototypeComplete() const { return bPrototypeComplete; }

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ASH LINE|Prototype") int32 RequiredEnemyCount = 6;
    UPROPERTY(BlueprintReadOnly, Category="ASH LINE|Prototype") int32 DefeatedEnemyCount = 0;
    UPROPERTY(BlueprintReadOnly, Category="ASH LINE|Prototype") bool bPrototypeComplete = false;

    UPROPERTY(BlueprintAssignable, Category="ASH LINE|Prototype") FALCombatPrototypeEnemyCountChanged OnEnemyCountChanged;
    UPROPERTY(BlueprintAssignable, Category="ASH LINE|Prototype") FALCombatPrototypeCompleted OnPrototypeCompleted;

protected:
    UFUNCTION() void HandleEnemyKilled(AActor* Enemy);
    void CompletePrototype();

    UPROPERTY() TSet<TObjectPtr<class AALInfantryCharacter>> RegisteredEnemies;
    UPROPERTY() TSet<TObjectPtr<class AALInfantryCharacter>> DefeatedEnemies;
};
