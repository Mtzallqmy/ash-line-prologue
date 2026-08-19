#include "Components/ALWeaponComponent.h"
#include "Components/ALHealthComponent.h"
#include "GameFramework/Pawn.h"

UALWeaponComponent::UALWeaponComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UALWeaponComponent::BeginPlay()
{
    Super::BeginPlay();
    if (UALHealthComponent* Health = GetOwner() ? GetOwner()->FindComponentByClass<UALHealthComponent>() : nullptr)
    {
        Health->OnDeath.AddDynamic(this, &UALWeaponComponent::HandleOwnerDeath);
    }
}

void UALWeaponComponent::EquipWeapon(AALWeaponBase* Weapon, bool bPrimarySlot)
{
    if (!IsValid(Weapon)) return;
    if (bPrimarySlot) PrimaryWeapon = Weapon; else SidearmWeapon = Weapon;
    if (CurrentWeapon && CurrentWeapon != Weapon) CurrentWeapon->UnequipWeapon();
    CurrentWeapon = Weapon;
    CurrentWeapon->EquipWeapon(Cast<APawn>(GetOwner()));
    BindWeaponEvents(CurrentWeapon);
    OnWeaponChanged.Broadcast(CurrentWeapon);
}

void UALWeaponComponent::UnequipCurrentWeapon()
{
    if (!CurrentWeapon) return;
    UnbindWeaponEvents(CurrentWeapon);
    CurrentWeapon->UnequipWeapon();
    CurrentWeapon = nullptr;
    OnWeaponChanged.Broadcast(nullptr);
}

void UALWeaponComponent::SwitchToNextWeapon()
{
    if (CurrentWeapon == PrimaryWeapon && SidearmWeapon) EquipWeapon(SidearmWeapon, false);
    else if (PrimaryWeapon) EquipWeapon(PrimaryWeapon, true);
}

void UALWeaponComponent::StartFire()
{
    if (!bWeaponInputEnabled || !CurrentWeapon) return;
    if (APawn* Pawn = Cast<APawn>(GetOwner())) CurrentWeapon->SetAimDirection(Pawn->GetPawnViewLocation(), Pawn->GetViewRotation().Vector());
    CurrentWeapon->StartFire();
}

void UALWeaponComponent::StopFire() { if (CurrentWeapon) CurrentWeapon->StopFire(); }
void UALWeaponComponent::StartReload() { if (bWeaponInputEnabled && CurrentWeapon) CurrentWeapon->StartReload(); }
void UALWeaponComponent::StartAim() { if (bWeaponInputEnabled && CurrentWeapon) CurrentWeapon->StartAim(); }
void UALWeaponComponent::StopAim() { if (CurrentWeapon) CurrentWeapon->StopAim(); }
void UALWeaponComponent::SetWeaponInputEnabled(bool bEnabled) { bWeaponInputEnabled = bEnabled; if (CurrentWeapon) CurrentWeapon->SetWeaponInputEnabled(bEnabled); }
void UALWeaponComponent::AddReserveAmmo(int32 Amount) { if (CurrentWeapon) CurrentWeapon->AddReserveAmmo(Amount); }

void UALWeaponComponent::BindWeaponEvents(AALWeaponBase* Weapon)
{
    if (!Weapon) return;
    Weapon->OnAmmoChanged.AddDynamic(this, &UALWeaponComponent::HandleWeaponAmmoChanged);
    Weapon->OnAimChanged.AddDynamic(this, &UALWeaponComponent::HandleWeaponAimChanged);
}

void UALWeaponComponent::UnbindWeaponEvents(AALWeaponBase* Weapon)
{
    if (!Weapon) return;
    Weapon->OnAmmoChanged.RemoveDynamic(this, &UALWeaponComponent::HandleWeaponAmmoChanged);
    Weapon->OnAimChanged.RemoveDynamic(this, &UALWeaponComponent::HandleWeaponAimChanged);
}

void UALWeaponComponent::HandleWeaponAmmoChanged(int32 MagazineAmmo, int32 ReserveAmmo) { OnAmmoChanged.Broadcast(MagazineAmmo, ReserveAmmo); }
void UALWeaponComponent::HandleWeaponAimChanged(bool bAiming) { OnAimChanged.Broadcast(bAiming); }
void UALWeaponComponent::HandleOwnerDeath() { SetWeaponInputEnabled(false); }
