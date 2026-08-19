#pragma once
#include "CoreMinimal.h"
#include "ALWeaponTypes.generated.h"

UENUM(BlueprintType)
enum class EALWeaponType : uint8
{
    AssaultRifle,
    SMG,
    Pistol,
    Shotgun,
    Sniper,
    LMG,
    Heavy
};

UENUM(BlueprintType)
enum class EALFireMode : uint8
{
    SemiAuto,
    FullAuto,
    Burst
};

UENUM(BlueprintType)
enum class EALWeaponState : uint8
{
    Idle,
    Firing,
    Reloading,
    Equipping,
    Unequipping
};

UENUM(BlueprintType)
enum class EALHitZone : uint8
{
    Unknown,
    Head,
    Torso,
    Arms,
    Legs
};

USTRUCT(BlueprintType)
struct ASHLINEWEAPONS_API FALRecoilSettings
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Recoil") float PitchMin = 0.15f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Recoil") float PitchMax = 0.35f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Recoil") float YawMin = -0.12f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Recoil") float YawMax = 0.12f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Recoil") float RecoverySpeed = 4.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Recoil") float MobileRecoilScale = 0.75f;
};

USTRUCT(BlueprintType)
struct ASHLINEWEAPONS_API FALHitZoneProfile
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hit Zones") float HeadMultiplier = 1.8f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hit Zones") float TorsoMultiplier = 1.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hit Zones") float LimbMultiplier = 0.75f;

    EALHitZone ResolveZone(FName BoneName) const
    {
        const FString Bone = BoneName.ToString().ToLower();
        if (Bone.Contains(TEXT("head")) || Bone.Contains(TEXT("neck"))) return EALHitZone::Head;
        if (Bone.Contains(TEXT("spine")) || Bone.Contains(TEXT("chest")) || Bone.Contains(TEXT("pelvis")) || Bone.Contains(TEXT("torso"))) return EALHitZone::Torso;
        if (Bone.Contains(TEXT("arm")) || Bone.Contains(TEXT("hand")) || Bone.Contains(TEXT("forearm")) || Bone.Contains(TEXT("leg")) || Bone.Contains(TEXT("foot")) || Bone.Contains(TEXT("thigh"))) return EALHitZone::Legs;
        return EALHitZone::Unknown;
    }

    float GetMultiplier(FName BoneName) const
    {
        switch (ResolveZone(BoneName))
        {
            case EALHitZone::Head: return HeadMultiplier;
            case EALHitZone::Torso: return TorsoMultiplier;
            case EALHitZone::Arms:
            case EALHitZone::Legs: return LimbMultiplier;
            default: return 1.0f;
        }
    }
};
