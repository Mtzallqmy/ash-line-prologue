#include "AI/ALAIStateComponent.h"
#include "Engine/World.h"

UALAIStateComponent::UALAIStateComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UALAIStateComponent::SetState(EALAIState NewState)
{
    if (State == NewState) return;
    const EALAIState Previous = State;
    State = NewState;
    StateStartedAt = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
    OnStateChanged.Broadcast(Previous, NewState);
}

float UALAIStateComponent::GetStateTime() const
{
    return GetWorld() ? FMath::Max(0.0f, GetWorld()->GetTimeSeconds() - StateStartedAt) : 0.0f;
}

void UALAIStateComponent::MarkStateTime(float InStateTime)
{
    StateStartedAt = GetWorld() ? GetWorld()->GetTimeSeconds() - FMath::Max(0.0f, InStateTime) : 0.0f;
}
