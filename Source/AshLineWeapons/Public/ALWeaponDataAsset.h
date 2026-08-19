#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ALWeaponDataAsset.generated.h"

UENUM(BlueprintType)
enum class EALWeaponKind : uint8 { AssaultRifle, SMG, Pistol };

UCLASS(BlueprintType)
class ASHLINEWEAPONS_API UALWeaponDataAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Identity") FName WeaponId;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Identity") EALWeaponKind Kind = EALWeaponKind::Pistol;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ballistics") float Damage = 20.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ballistics") float FireRate = 8.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ballistics") int32 MagazineSize = 15;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ballistics") float ReloadTime = 1.2f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ballistics") float Recoil = 0.4f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ballistics") float Range = 12000.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Presentation") TSoftObjectPtr<USkeletalMesh> WeaponMesh;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Presentation") TSoftObjectPtr<USoundBase> FireSound;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Presentation") TSoftObjectPtr<USoundBase> ReloadSound;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Presentation") TSoftObjectPtr<UAnimMontage> FireAnimation;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Presentation") TSoftObjectPtr<UAnimMontage> ReloadAnimation;
    virtual FPrimaryAssetId GetPrimaryAssetId() const override { return FPrimaryAssetId(TEXT("Weapon"), WeaponId); }
};
