#include "ALPlayerStateComponent.h"

void UALPlayerStateComponent::SetMovementEnabled(bool bEnabled)
{
    bCanMove = bEnabled;
}

void UALPlayerStateComponent::SetLookEnabled(bool bEnabled)
{
    bCanLook = bEnabled;
}

void UALPlayerStateComponent::SetInteractionEnabled(bool bEnabled)
{
    bCanInteract = bEnabled;
}

void UALPlayerStateComponent::SetInputLocked(bool bLocked)
{
    bInputLocked = bLocked;
}

void UALPlayerStateComponent::SetControlBlock(EALPlayerControlBlock NewBlock)
{
    ControlBlock = NewBlock;
    switch (NewBlock)
    {
        case EALPlayerControlBlock::None:
            bCanMove = true; bCanLook = true; bCanInteract = true; bInputLocked = false; break;
        case EALPlayerControlBlock::Scripted:
            bCanMove = false; bCanLook = false; bCanInteract = false; bInputLocked = true; break;
        case EALPlayerControlBlock::Dialogue:
            bCanMove = false; bCanLook = true; bCanInteract = false; bInputLocked = false; break;
        case EALPlayerControlBlock::Drone:
            bCanMove = false; bCanLook = false; bCanInteract = false; bInputLocked = false; break;
        case EALPlayerControlBlock::Pause:
            bCanMove = false; bCanLook = false; bCanInteract = false; bInputLocked = true; break;
        default: break;
    }
}
