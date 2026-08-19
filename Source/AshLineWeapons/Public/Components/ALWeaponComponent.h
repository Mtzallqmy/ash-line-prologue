#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ALWeaponBase.h"
#include "ALWeaponComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FALWeaponChanged, AALWeaponBase*, Weapon);

UCLASS(ClassGroup=(ASHLINE), meta=(BlueprintSpawnableComponent))
class ASHLINEWEAPONS_API UALWeaponComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UALWeaponComponent();
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category="ASH LINE|Weapon") void EquipWeapon(AALWeaponBase* Weapon, bool bPrimarySlot = true);
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Weapon") void UnequipCurrentWeapon();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Weapon") void SwitchToNextWeapon();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Weapon") void StartFire();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Weapon") void StopFire();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Weapon") void StartReload();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Weapon") void StartAim();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Weapon") void StopAim();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Weapon") void SetWeaponInputEnabled(bool bEnabled);
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Weapon") void AddReserveAmmo(int32 Amount);

    UFUNCTION(BlueprintPure, Category="ASH LINE|Weapon") AALWeaponBase* GetCurrentWeapon() const { return CurrentWeapon; }
    UFUNCTION(BlueprintPure, Category="ASH LINE|Weapon") bool IsAiming() const { return CurrentWeapon && CurrentWeapon->IsAiming(); }

    UPROPERTY(BlueprintAssignable, Category="ASH LINE|Weapon") FALWeaponChanged OnWeaponChanged;
    UPROPERTY(BlueprintAssignable, Category="ASH LINE|Weapon") FALWeaponAmmoChanged OnAmmoChanged;
    UPROPERTY(BlueprintAssignable, Category="ASH LINE|Weapon") FALWeaponAimChanged OnAimChanged;

protected:
    void BindWeaponEvents(AALWeaponBase* Weapon);
    void UnbindWeaponEvents(AALWeaponBase* Weapon);
    UFUNCTION() void HandleWeaponAmmoChanged(int32 MagazineAmmo, int32 ReserveAmmo);
    UFUNCTION() void HandleWeaponAimChanged(bool bAiming);
    UFUNCTION() void HandleOwnerDeath();

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="ASH LINE|Weapon") TObjectPtr<AALWeaponBase> CurrentWeapon;
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="ASH LINE|Weapon") TObjectPtr<AALWeaponBase> PrimaryWeapon;
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="ASH LINE|Weapon") TObjectPtr<AALWeaponBase> SidearmWeapon;
    bool bWeaponInputEnabled = true;
};
