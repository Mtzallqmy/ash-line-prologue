#include "ALDebugInteractableActor.h"

AALDebugInteractableActor::AALDebugInteractableActor()
{
    PrimaryActorTick.bCanEverTick = false;
}

bool AALDebugInteractableActor::CanInteract_Implementation(AActor* Interactor) const
{
    return IsValid(Interactor);
}

void AALDebugInteractableActor::Interact_Implementation(AActor* Interactor)
{
    if (IsValid(Interactor)) ++InteractionCount;
}

FText AALDebugInteractableActor::GetInteractionText_Implementation() const
{
    return FText::FromString(TEXT("Interact"));
}
