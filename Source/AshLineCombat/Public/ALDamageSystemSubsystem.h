#pragma once
#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Damage/ALDamageData.h"
#include "ALDamageSystemSubsystem.generated.h"

UCLASS()
class ASHLINECOMBAT_API UALDamageSystemSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Damage") bool CanDamageTarget(AActor* Target, const FALDamageData& DamageData) const;
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Damage") float ApplyDamage(AActor* Target, const FALDamageData& DamageData);
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Damage") float ApplyPointDamage(AActor* Target, float BaseDamage, EALDamageType DamageType, const FHitResult& HitResult, AController* Instigator, AActor* DamageCauser);
};
