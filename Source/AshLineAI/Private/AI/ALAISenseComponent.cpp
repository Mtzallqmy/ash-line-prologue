#include "AI/ALAISenseComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"

UALAISenseComponent::UALAISenseComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UALAISenseComponent::ProcessSightStimulus(AActor* Actor, bool bSensed, const FVector& StimulusLocation)
{
    if (!IsValid(Actor)) return;
    if (bSensed)
    {
        const bool bWasVisible = bTargetVisible;
        CurrentTarget = Actor;
        bTargetVisible = true;
        LastKnownLocation = StimulusLocation;
        LastSeenTime = GetWorld() ? GetWorld()->GetTimeSeconds() : LastSeenTime;
        SetAwareness(FMath::Clamp(Awareness + 0.35f, 0.0f, 1.0f));
        if (!bWasVisible) OnTargetDetected.Broadcast(Actor);
    }
    else if (CurrentTarget == Actor)
    {
        bTargetVisible = false;
        OnTargetLost.Broadcast(Actor);
    }
}

void UALAISenseComponent::ProcessHearingStimulus(AActor* Actor, const FVector& StimulusLocation)
{
    if (!IsValid(Actor)) return;
    CurrentTarget = Actor;
    LastKnownLocation = StimulusLocation;
    SetAwareness(FMath::Clamp(Awareness + 0.25f, 0.0f, 1.0f));
    OnNoiseHeard.Broadcast(Actor);
}

void UALAISenseComponent::ProcessDamageAwareness(const FALDamageData& DamageData)
{
    AActor* SourceActor = DamageData.DamageCauser;
    if (DamageData.Instigator.IsValid() && DamageData.Instigator->GetPawn()) SourceActor = DamageData.Instigator->GetPawn();
    if (IsValid(SourceActor))
    {
        CurrentTarget = SourceActor;
        LastKnownLocation = DamageData.HitLocation;
    }
    SetAwareness(1.0f);
}

void UALAISenseComponent::DecayAwareness(float DeltaSeconds)
{
    if (bTargetVisible) return;
    SetAwareness(FMath::Clamp(Awareness - FMath::Max(DeltaSeconds, 0.0f) * 0.08f, 0.0f, 1.0f));
    if (Awareness <= KINDA_SMALL_NUMBER && CurrentTarget.IsValid()) ClearTarget();
}

void UALAISenseComponent::ClearTarget()
{
    AActor* PreviousTarget = CurrentTarget.Get();
    CurrentTarget.Reset();
    bTargetVisible = false;
    Awareness = 0.0f;
    if (IsValid(PreviousTarget)) OnTargetLost.Broadcast(PreviousTarget);
}

void UALAISenseComponent::SetAwareness(float NewAwareness)
{
    const float Previous = Awareness;
    Awareness = FMath::Clamp(NewAwareness, 0.0f, 1.0f);
    if (!FMath::IsNearlyEqual(Previous, Awareness)) OnAwarenessChanged.Broadcast(CurrentTarget.Get(), Awareness);
}
