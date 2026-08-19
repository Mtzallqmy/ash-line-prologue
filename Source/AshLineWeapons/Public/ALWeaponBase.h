#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Types/ALWeaponTypes.h"
#include "ALWeaponBase.generated.h"

class UALWeaponDataAsset;
class UALRecoilComponent;
class UInputAction;
class USoundBase;
class UNiagaraSystem;
class APawn;
class USceneComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FALWeaponAmmoChanged, int32, MagazineAmmo, int32, ReserveAmmo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FALWeaponAimChanged, bool, bAiming);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FALWeaponDryFire, FName, WeaponId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FALWeaponEvent);

UCLASS(Blueprintable)
class ASHLINEWEAPONS_API AALWeaponBase : public AActor
{
    GENERATED_BODY()
public:
    AALWeaponBase();
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category="ASH LINE|Weapon") void InitializeWeapon(APawn* NewOwner);
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Weapon") void SetOwningActor(APawn* NewOwner);
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Weapon") void EquipWeapon(APawn* NewOwner);
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Weapon") void UnequipWeapon();

    UFUNCTION(BlueprintCallable, Category="ASH LINE|Weapon") void StartFire();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Weapon") void StopFire();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Weapon") void StartReload();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Weapon") void CompleteReload();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Weapon") void CancelReload();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Weapon") void StartAim();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Weapon") void StopAim();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Weapon") void SetWeaponInputEnabled(bool bEnabled);
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Weapon") void AddReserveAmmo(int32 Amount);
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Weapon") void SetAimDirection(const FVector& Origin, const FVector& Direction);
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Weapon") bool FireAtTarget(AActor* Target, const FVector& Origin, const FVector& Direction);

    UFUNCTION(BlueprintPure, Category="ASH LINE|Weapon") bool IsAiming() const { return bIsAiming; }
    UFUNCTION(BlueprintPure, Category="ASH LINE|Weapon") bool IsInputEnabled() const { return bWeaponInputEnabled; }
    UFUNCTION(BlueprintPure, Category="ASH LINE|Weapon") int32 GetAmmoInMagazine() const { return AmmoInMagazine; }
    UFUNCTION(BlueprintPure, Category="ASH LINE|Weapon") int32 GetReserveAmmo() const { return ReserveAmmo; }
    UFUNCTION(BlueprintPure, Category="ASH LINE|Weapon") int32 GetMagazineSize() const;
    UFUNCTION(BlueprintPure, Category="ASH LINE|Weapon") EALWeaponState GetWeaponState() const { return WeaponState; }
    UFUNCTION(BlueprintPure, Category="ASH LINE|Weapon") FName GetWeaponId() const;
    UFUNCTION(BlueprintPure, Category="ASH LINE|Weapon") float GetDefaultFOV() const;
    UFUNCTION(BlueprintPure, Category="ASH LINE|Weapon") float GetADSFOV() const;
    UFUNCTION(BlueprintPure, Category="ASH LINE|Weapon") float GetAimTransitionTime() const;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon") TObjectPtr<UALWeaponDataAsset> Data;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon") TObjectPtr<USkeletalMeshComponent> WeaponMesh;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon") TObjectPtr<USceneComponent> MuzzlePoint;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon") TObjectPtr<UALRecoilComponent> RecoilComponent;

    UPROPERTY(BlueprintAssignable, Category="ASH LINE|Weapon") FALWeaponAmmoChanged OnAmmoChanged;
    UPROPERTY(BlueprintAssignable, Category="ASH LINE|Weapon") FALWeaponEvent OnWeaponFired;
    UPROPERTY(BlueprintAssignable, Category="ASH LINE|Weapon") FALWeaponEvent OnReloadStarted;
    UPROPERTY(BlueprintAssignable, Category="ASH LINE|Weapon") FALWeaponEvent OnReloadCompleted;
    UPROPERTY(BlueprintAssignable, Category="ASH LINE|Weapon") FALWeaponEvent OnReloadCancelled;
    UPROPERTY(BlueprintAssignable, Category="ASH LINE|Weapon") FALWeaponAimChanged OnAimChanged;
    UPROPERTY(BlueprintAssignable, Category="ASH LINE|Weapon") FALWeaponDryFire OnDryFire;

protected:
    bool TryFireSingle();
    void HandleAutomaticFire();
    void FireHitscan(const FVector& Origin, const FVector& Direction);
    FVector ApplySpread(const FVector& Direction) const;
    float GetCurrentSpread() const;
    void BroadcastAmmoChanged();
    void ResolveRuntimeAssets();

    UPROPERTY(BlueprintReadOnly, Category="Weapon|Runtime") int32 AmmoInMagazine = 0;
    UPROPERTY(BlueprintReadOnly, Category="Weapon|Runtime") int32 ReserveAmmo = 0;
    UPROPERTY(BlueprintReadOnly, Category="Weapon|Runtime") EALWeaponState WeaponState = EALWeaponState::Idle;
    UPROPERTY(BlueprintReadOnly, Category="Weapon|Runtime") bool bIsAiming = false;
    UPROPERTY(BlueprintReadOnly, Category="Weapon|Runtime") bool bWeaponInputEnabled = true;
    UPROPERTY() TObjectPtr<APawn> OwningPawn;
    UPROPERTY(Transient) TObjectPtr<USoundBase> CachedFireSound;
    UPROPERTY(Transient) TObjectPtr<UNiagaraSystem> CachedMuzzleFX;
    FVector AimOrigin = FVector::ZeroVector;
    FVector AimDirection = FVector::ForwardVector;
    float NextFireTime = 0.0f;
    FTimerHandle FireTimer;
    FTimerHandle ReloadTimer;
};
