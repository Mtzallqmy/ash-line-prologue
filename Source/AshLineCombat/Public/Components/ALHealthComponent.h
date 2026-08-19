#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Damage/ALDamageData.h"
#include "Damage/ALHealthConfig.h"
#include "ALHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FALHealthChanged, float, OldHealth, float, NewHealth, float, MaxHealth, float, Delta);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FALDamageReceived, FALDamageData, DamageData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FALHealingReceived, float, Amount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FALDeathEvent);

UCLASS(ClassGroup=(ASHLINE), meta=(BlueprintSpawnableComponent))
class ASHLINECOMBAT_API UALHealthComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UALHealthComponent();
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category="ASH LINE|Health") float ApplyDamage(const FALDamageData& DamageData);
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Health") float ApplyHealing(float Amount);
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Health") void ResetHealth();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Health") void RestoreFullHealth();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Health") void SetMaxHealth(float NewMaxHealth);
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Health") void SetInvulnerable(bool bNewInvulnerable);

    UFUNCTION(BlueprintPure, Category="ASH LINE|Health") float GetCurrentHealth() const { return CurrentHealth; }
    UFUNCTION(BlueprintPure, Category="ASH LINE|Health") float GetMaxHealth() const { return MaxHealth; }
    UFUNCTION(BlueprintPure, Category="ASH LINE|Health") float GetHealthPercentage() const;
    UFUNCTION(BlueprintPure, Category="ASH LINE|Health") bool IsAlive() const { return !bIsDead; }
    UFUNCTION(BlueprintPure, Category="ASH LINE|Health") bool IsDead() const { return bIsDead; }
    UFUNCTION(BlueprintPure, Category="ASH LINE|Health") bool IsInvulnerable() const { return bInvulnerable; }
    UFUNCTION(BlueprintPure, Category="ASH LINE|Health") FALDamageData GetLastDamageData() const { return LastDamageData; }

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Health") float MaxHealth = 100.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Health") TObjectPtr<UALHealthConfig> HealthConfig;
    UPROPERTY(BlueprintReadOnly, Category="Health") float CurrentHealth = 100.0f;
    UPROPERTY(BlueprintReadOnly, Category="Health") bool bIsDead = false;
    UPROPERTY(BlueprintReadOnly, Category="Health") bool bInvulnerable = false;
    UPROPERTY(BlueprintReadOnly, Category="Damage") FALDamageData LastDamageData;

    UPROPERTY(BlueprintAssignable, Category="ASH LINE|Health") FALHealthChanged OnHealthChanged;
    UPROPERTY(BlueprintAssignable, Category="ASH LINE|Health") FALDamageReceived OnDamageReceived;
    UPROPERTY(BlueprintAssignable, Category="ASH LINE|Health") FALHealingReceived OnHealingReceived;
    UPROPERTY(BlueprintAssignable, Category="ASH LINE|Health") FALDeathEvent OnDeath;

protected:
    FTimerHandle SpawnProtectionTimer;
    virtual float ModifyIncomingDamage(const FALDamageData& DamageData) const;
    virtual void HandleDeath();
    bool CanReceiveDamage(const FALDamageData& DamageData) const;
    void BroadcastHealthChanged(float OldHealth, float Delta);
};
