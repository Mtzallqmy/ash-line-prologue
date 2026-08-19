#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Types/ALWeaponTypes.h"
#include "ALWeaponData.generated.h"

class UNiagaraSystem;

UCLASS(BlueprintType)
class ASHLINEWEAPONS_API UALWeaponDataAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Identity") FName WeaponId = NAME_None;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Identity") FText DisplayName;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Identity") EALWeaponType WeaponType = EALWeaponType::Pistol;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Identity") EALFireMode FireMode = EALFireMode::SemiAuto;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ballistics") float Damage = 20.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ballistics") float RoundsPerMinute = 350.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ballistics") int32 MagazineSize = 15;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ballistics") int32 ReserveAmmo = 45;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ballistics") float ReloadTime = 1.2f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ballistics") float Range = 8000.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ballistics") float HipFireSpread = 1.8f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ballistics") float ADSSpread = 0.6f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ballistics") float MovementSpreadMultiplier = 1.35f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ballistics") float CrouchSpreadMultiplier = 0.85f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ballistics") float ADSModifier = 0.65f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Aim") float DefaultFOV = 90.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Aim") float ADSFOV = 70.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Aim") float AimTransitionTime = 0.15f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Aim") float HipSensitivity = 1.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Aim") float ADSSensitivity = 0.65f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Recoil") FALRecoilSettings RecoilSettings;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hit Zones") FALHitZoneProfile HitZones;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Presentation") TSoftObjectPtr<USkeletalMesh> WeaponMesh;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Presentation") FName MuzzleSocketName = TEXT("Muzzle");
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Presentation") TSoftObjectPtr<USoundBase> FireSound;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Presentation") TSoftObjectPtr<USoundBase> ReloadSound;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Presentation") TSoftObjectPtr<UAnimMontage> FireAnimation;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Presentation") TSoftObjectPtr<UAnimMontage> ReloadAnimation;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Presentation") TSoftObjectPtr<UNiagaraSystem> MuzzleFX;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Presentation") FName ImpactProfileId = TEXT("Default");

    virtual FPrimaryAssetId GetPrimaryAssetId() const override { return FPrimaryAssetId(TEXT("Weapon"), WeaponId); }
    float GetSecondsPerShot() const { return 60.0f / FMath::Max(RoundsPerMinute, 1.0f); }
};
