#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AI/ALAIStateComponent.h"
#include "ALInfantryCharacter.generated.h"

class UALAIArchetypeData;
class AALPatrolRoute;
class AALWeaponBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FALEnemyKilledEvent, AActor*, Enemy);

UCLASS(Blueprintable)
class ASHLINEAI_API AALInfantryCharacter : public ACharacter
{
    GENERATED_BODY()
public:
    AALInfantryCharacter();
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Health") TObjectPtr<class UALHealthComponent> HealthComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon") TObjectPtr<class UALWeaponComponent> WeaponComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI") TObjectPtr<class UALAIStateComponent> AIStateComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI") TObjectPtr<class UALAISenseComponent> SenseComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI") TObjectPtr<class UALAICombatComponent> CombatComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI") TObjectPtr<class UALAICoverComponent> CoverComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI") TObjectPtr<class UALAISignificanceComponent> SignificanceComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AI") TObjectPtr<UALAIArchetypeData> Archetype;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon") TSubclassOf<AALWeaponBase> WeaponClass;
    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="AI|Patrol") TObjectPtr<AALPatrolRoute> PatrolRoute;
    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="AI") FName SquadId = NAME_None;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Death") float CorpseLifetime = 0.0f;

    UPROPERTY(BlueprintAssignable, Category="ASH LINE|AI") FALEnemyKilledEvent OnEnemyKilled;
    UFUNCTION(BlueprintPure, Category="ASH LINE|AI") EALAIState GetAIState() const { return AIStateComponent ? AIStateComponent->GetState() : EALAIState::Idle; }

protected:
    UFUNCTION() void HandleHealthDamage(FALDamageData DamageData);
    UFUNCTION() void HandleHealthDeath();
    UPROPERTY() TObjectPtr<AALWeaponBase> SpawnedWeapon;
    bool bDeathHandled = false;
};
