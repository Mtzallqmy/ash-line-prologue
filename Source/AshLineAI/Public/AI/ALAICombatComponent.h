#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ALAICombatComponent.generated.h"

class AALWeaponBase;
class UALWeaponComponent;
class UALAIArchetypeData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FALAICombatEvent);

UCLASS(ClassGroup=(ASHLINE), meta=(BlueprintSpawnableComponent))
class ASHLINEAI_API UALAICombatComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UALAICombatComponent();
    virtual void BeginPlay() override;
    void ConfigureFromArchetype(const UALAIArchetypeData* Archetype);
    void StartCombat(AActor* Target);
    void StopCombat();
    void SetTargetVisible(bool bVisible);
    void EvaluateCombat();

    UFUNCTION(BlueprintPure, Category="ASH LINE|AI") bool IsCombatActive() const { return bCombatActive; }
    UFUNCTION(BlueprintPure, Category="ASH LINE|AI") float GetAccuracy() const { return Accuracy; }
    UPROPERTY(BlueprintAssignable, Category="ASH LINE|AI") FALAICombatEvent OnAIFired;

protected:
    void BeginBurst();
    void EndBurst();
    void FinishReaction();
    FVector BuildImperfectAim(AActor* Target) const;

    UPROPERTY() TObjectPtr<UALWeaponComponent> WeaponComponent;
    TWeakObjectPtr<AActor> CurrentTarget;
    FTimerHandle DecisionTimer;
    FTimerHandle ReactionTimer;
    FTimerHandle BurstTimer;
    float ReactionTime = 0.45f;
    float Accuracy = 0.65f;
    float CombatRange = 1500.0f;
    float MinBurstTime = 0.35f;
    float MaxBurstTime = 0.8f;
    float MinPauseTime = 0.45f;
    float MaxPauseTime = 1.0f;
    bool bCombatActive = false;
    bool bTargetVisible = false;
    bool bReactionReady = false;
    bool bBurstActive = false;
};
