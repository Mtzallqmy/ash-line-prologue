#include "Components/ALHealthComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

DEFINE_LOG_CATEGORY_STATIC(LogAshLineCombat, Log, All);

UALHealthComponent::UALHealthComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UALHealthComponent::BeginPlay()
{
    Super::BeginPlay();
    if (HealthConfig)
    {
        MaxHealth = FMath::Max(HealthConfig->MaxHealth, 0.0f);
        if (HealthConfig->InitialInvulnerabilityDuration > 0.0f && GetWorld())
        {
            bInvulnerable = true;
            GetWorld()->GetTimerManager().SetTimer(SpawnProtectionTimer, [this]() { bInvulnerable = false; }, HealthConfig->InitialInvulnerabilityDuration, false);
        }
    }
    CurrentHealth = FMath::Clamp(CurrentHealth, 0.0f, MaxHealth);
    if (CurrentHealth <= 0.0f && MaxHealth > 0.0f) CurrentHealth = MaxHealth;
    bIsDead = false;
}

float UALHealthComponent::GetHealthPercentage() const
{
    return MaxHealth > KINDA_SMALL_NUMBER ? FMath::Clamp(CurrentHealth / MaxHealth, 0.0f, 1.0f) : 0.0f;
}

bool UALHealthComponent::CanReceiveDamage(const FALDamageData& DamageData) const
{
    return !bIsDead && !bInvulnerable && DamageData.BaseDamage > 0.0f && MaxHealth > 0.0f;
}

float UALHealthComponent::ModifyIncomingDamage(const FALDamageData& DamageData) const
{
    return FMath::Max(0.0f, DamageData.BaseDamage * FMath::Max(DamageData.CriticalMultiplier, 0.0f));
}

float UALHealthComponent::ApplyDamage(const FALDamageData& DamageData)
{
    if (!CanReceiveDamage(DamageData))
    {
        UE_LOG(LogAshLineCombat, Verbose, TEXT("Ignored invalid damage on %s: Amount=%f Dead=%d Invulnerable=%d"), *GetNameSafe(GetOwner()), DamageData.BaseDamage, bIsDead, bInvulnerable);
        return 0.0f;
    }

    const float ProcessedDamage = ModifyIncomingDamage(DamageData);
    if (ProcessedDamage <= 0.0f) return 0.0f;

    FALDamageData ProcessedData = DamageData;
    ProcessedData.BaseDamage = ProcessedDamage;
    LastDamageData = ProcessedData;

    const float OldHealth = CurrentHealth;
    CurrentHealth = FMath::Clamp(CurrentHealth - ProcessedDamage, 0.0f, MaxHealth);
    const float AppliedDamage = OldHealth - CurrentHealth;
    if (AppliedDamage <= 0.0f) return 0.0f;

    BroadcastHealthChanged(OldHealth, -AppliedDamage);
    OnDamageReceived.Broadcast(ProcessedData);
    UE_LOG(LogAshLineCombat, Verbose, TEXT("%s received %f damage; health=%f/%f"), *GetNameSafe(GetOwner()), AppliedDamage, CurrentHealth, MaxHealth);

    if (CurrentHealth <= 0.0f && !bIsDead)
    {
        bIsDead = true;
        HandleDeath();
    }
    return AppliedDamage;
}

float UALHealthComponent::ApplyHealing(float Amount)
{
    if (bIsDead || Amount <= 0.0f || MaxHealth <= 0.0f) return 0.0f;
    const float OldHealth = CurrentHealth;
    CurrentHealth = FMath::Clamp(CurrentHealth + Amount, 0.0f, MaxHealth);
    const float AppliedHealing = CurrentHealth - OldHealth;
    if (AppliedHealing <= 0.0f) return 0.0f;
    BroadcastHealthChanged(OldHealth, AppliedHealing);
    OnHealingReceived.Broadcast(AppliedHealing);
    return AppliedHealing;
}

void UALHealthComponent::ResetHealth()
{
    const float OldHealth = CurrentHealth;
    CurrentHealth = MaxHealth;
    bIsDead = false;
    LastDamageData = FALDamageData();
    if (!FMath::IsNearlyEqual(OldHealth, CurrentHealth)) BroadcastHealthChanged(OldHealth, CurrentHealth - OldHealth);
}

void UALHealthComponent::RestoreFullHealth()
{
    ResetHealth();
}

void UALHealthComponent::SetMaxHealth(float NewMaxHealth)
{
    const float OldHealth = CurrentHealth;
    MaxHealth = FMath::Max(NewMaxHealth, 0.0f);
    CurrentHealth = FMath::Clamp(CurrentHealth, 0.0f, MaxHealth);
    if (!FMath::IsNearlyEqual(OldHealth, CurrentHealth)) BroadcastHealthChanged(OldHealth, CurrentHealth - OldHealth);
}

void UALHealthComponent::SetInvulnerable(bool bNewInvulnerable)
{
    bInvulnerable = bNewInvulnerable;
}

void UALHealthComponent::BroadcastHealthChanged(float OldHealth, float Delta)
{
    OnHealthChanged.Broadcast(OldHealth, CurrentHealth, MaxHealth, Delta);
}

void UALHealthComponent::HandleDeath()
{
    UE_LOG(LogAshLineCombat, Log, TEXT("%s died from %s"), *GetNameSafe(GetOwner()), *UEnum::GetValueAsString(LastDamageData.DamageType));
    OnDeath.Broadcast();
}
