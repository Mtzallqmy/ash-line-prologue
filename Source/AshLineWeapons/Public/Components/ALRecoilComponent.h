#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/ALWeaponTypes.h"
#include "ALRecoilComponent.generated.h"

UCLASS(ClassGroup=(ASHLINE), meta=(BlueprintSpawnableComponent))
class ASHLINEWEAPONS_API UALRecoilComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UALRecoilComponent();
    void ApplyRecoil(const FALRecoilSettings& Settings, bool bMobileProfile);
    void ResetRecoil();
    UFUNCTION(BlueprintPure, Category="ASH LINE|Weapon") float GetCurrentPitch() const { return CurrentPitch; }
    UFUNCTION(BlueprintPure, Category="ASH LINE|Weapon") float GetCurrentYaw() const { return CurrentYaw; }
protected:
    void RecoverRecoil();
    float CurrentPitch = 0.0f;
    float CurrentYaw = 0.0f;
    FTimerHandle RecoveryTimer;
};
