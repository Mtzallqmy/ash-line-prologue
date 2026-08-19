#include "Diagnostics/ALDamageDebugLibrary.h"
#include "Components/ALHealthComponent.h"
#include "ALDamageSystemSubsystem.h"
#include "Engine/World.h"

bool UALDamageDebugLibrary::DamageActor(AActor* Target, float Amount)
{
#if !UE_BUILD_SHIPPING
    if (!IsValid(Target) || !Target->GetWorld()) return false;
    FALDamageData DamageData;
    DamageData.BaseDamage = Amount;
    DamageData.DamageType = EALDamageType::Generic;
    DamageData.DamageCauser = Target;
    return Target->GetWorld()->GetSubsystem<UALDamageSystemSubsystem>()->ApplyDamage(Target, DamageData) > 0.0f;
#else
    return false;
#endif
}

bool UALDamageDebugLibrary::HealActor(AActor* Target, float Amount)
{
#if !UE_BUILD_SHIPPING
    if (UALHealthComponent* Health = Target ? Target->FindComponentByClass<UALHealthComponent>() : nullptr) return Health->ApplyHealing(Amount) > 0.0f;
#endif
    return false;
}

bool UALDamageDebugLibrary::KillActor(AActor* Target)
{
#if !UE_BUILD_SHIPPING
    if (!IsValid(Target) || !Target->GetWorld()) return false;
    FALDamageData DamageData;
    DamageData.BaseDamage = TNumericLimits<float>::Max();
    DamageData.DamageType = EALDamageType::Generic;
    DamageData.DamageCauser = Target;
    return Target->GetWorld()->GetSubsystem<UALDamageSystemSubsystem>()->ApplyDamage(Target, DamageData) > 0.0f;
#else
    return false;
#endif
}

bool UALDamageDebugLibrary::SetGodMode(AActor* Target, bool bEnabled)
{
#if !UE_BUILD_SHIPPING
    if (UALHealthComponent* Health = Target ? Target->FindComponentByClass<UALHealthComponent>() : nullptr)
    {
        Health->SetInvulnerable(bEnabled);
        return true;
    }
#endif
    return false;
}
