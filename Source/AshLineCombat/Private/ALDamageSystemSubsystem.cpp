#include "ALDamageSystemSubsystem.h"
#include "GameFramework/Actor.h"

bool UALDamageSystemSubsystem::ApplyDamage(AActor* Target, const FALDamageEvent& Event)
{
    if (!IsValid(Target) || !Target->GetClass()->ImplementsInterface(UALDamageableInterface::StaticClass())) return false;
    return IALDamageableInterface::Execute_ReceiveALDamage(Target, Event);
}
