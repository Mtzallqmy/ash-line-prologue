#include "ALCharacterComponents.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "ALWeaponBase.h"

AActor* UALInteractionComponent::FindInteractable() const
{
    const APawn* Pawn = Cast<APawn>(GetOwner());
    if (!Pawn || !GetWorld()) return nullptr;
    const FVector Start = Pawn->GetPawnViewLocation();
    const FVector End = Start + Pawn->GetViewRotation().Vector() * InteractionRange;
    FHitResult Hit;
    return GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility) ? Hit.GetActor() : nullptr;
}

bool UALCombatComponent::FireCurrentWeapon()
{
    const APawn* Pawn = Cast<APawn>(GetOwner());
    return CurrentWeapon && Pawn && CurrentWeapon->Fire(Pawn->GetPawnViewLocation(), Pawn->GetViewRotation().Vector(), Pawn);
}
void UALCombatComponent::ReloadCurrentWeapon() { if (CurrentWeapon) CurrentWeapon->Reload(); }
