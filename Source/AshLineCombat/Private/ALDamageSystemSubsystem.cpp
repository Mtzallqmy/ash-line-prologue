#include "ALDamageSystemSubsystem.h"
#include "Components/ALHealthComponent.h"
#include "Interfaces/ALDamageReceiver.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Controller.h"

bool UALDamageSystemSubsystem::CanDamageTarget(AActor* Target, const FALDamageData& DamageData) const
{
    if (!IsValid(Target) || DamageData.BaseDamage <= 0.0f) return false;
    if (Target->GetClass()->ImplementsInterface(UALDamageReceiver::StaticClass()))
    {
        return IALDamageReceiver::Execute_CanReceiveALDamage(Target, DamageData);
    }
    const UALHealthComponent* Health = Target->FindComponentByClass<UALHealthComponent>();
    return Health && Health->IsAlive() && !Health->IsInvulnerable();
}

float UALDamageSystemSubsystem::ApplyDamage(AActor* Target, const FALDamageData& DamageData)
{
    if (!CanDamageTarget(Target, DamageData)) return 0.0f;
    if (Target->GetClass()->ImplementsInterface(UALDamageReceiver::StaticClass()))
    {
        return IALDamageReceiver::Execute_ReceiveALDamage(Target, DamageData) ? DamageData.BaseDamage : 0.0f;
    }
    if (UALHealthComponent* Health = Target->FindComponentByClass<UALHealthComponent>())
    {
        return Health->ApplyDamage(DamageData);
    }
    return 0.0f;
}

float UALDamageSystemSubsystem::ApplyPointDamage(AActor* Target, float BaseDamage, EALDamageType DamageType, const FHitResult& HitResult, AController* Instigator, AActor* DamageCauser)
{
    FALDamageData DamageData;
    DamageData.BaseDamage = BaseDamage;
    DamageData.DamageType = DamageType;
    DamageData.Instigator = Instigator;
    DamageData.DamageCauser = DamageCauser;
    DamageData.HitLocation = HitResult.ImpactPoint;
    DamageData.HitNormal = HitResult.ImpactNormal;
    DamageData.BoneName = HitResult.BoneName;
    DamageData.bHasHitResult = HitResult.bBlockingHit;
    return ApplyDamage(Target, DamageData);
}
