#include "ALCharacterComponents.h"
#include "GameFramework/Pawn.h"
#include "ALWeaponBase.h"

bool UALCombatComponent::FireCurrentWeapon()
{
    const APawn* Pawn = Cast<APawn>(GetOwner());
    return CurrentWeapon && Pawn && CurrentWeapon->Fire(Pawn->GetPawnViewLocation(), Pawn->GetViewRotation().Vector(), Pawn);
}

void UALCombatComponent::ReloadCurrentWeapon()
{
    if (CurrentWeapon) CurrentWeapon->Reload();
}
