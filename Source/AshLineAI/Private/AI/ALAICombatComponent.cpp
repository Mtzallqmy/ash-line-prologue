#include "AI/ALAICombatComponent.h"
#include "Data/ALAIArchetypeData.h"
#include "Components/ALWeaponComponent.h"
#include "ALWeaponBase.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"
#include "TimerManager.h"

UALAICombatComponent::UALAICombatComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UALAICombatComponent::BeginPlay()
{
    Super::BeginPlay();
    WeaponComponent = GetOwner() ? GetOwner()->FindComponentByClass<UALWeaponComponent>() : nullptr;
}

void UALAICombatComponent::ConfigureFromArchetype(const UALAIArchetypeData* Archetype)
{
    if (!Archetype) return;
    ReactionTime = Archetype->ReactionTime;
    Accuracy = Archetype->Accuracy;
    CombatRange = Archetype->CombatRange;
    MinBurstTime = Archetype->BurstSettings.MinBurstTime;
    MaxBurstTime = Archetype->BurstSettings.MaxBurstTime;
    MinPauseTime = Archetype->BurstSettings.MinPauseTime;
    MaxPauseTime = Archetype->BurstSettings.MaxPauseTime;
}

void UALAICombatComponent::StartCombat(AActor* Target)
{
    if (!IsValid(Target) || !WeaponComponent) return;
    const bool bSameEngagement = bCombatActive && CurrentTarget.Get() == Target;
    CurrentTarget = Target;
    bCombatActive = true;
    if (bSameEngagement) return;
    bReactionReady = false;
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(ReactionTimer);
        GetWorld()->GetTimerManager().SetTimer(ReactionTimer, this, &UALAICombatComponent::FinishReaction, FMath::Max(ReactionTime, 0.05f), false);
        GetWorld()->GetTimerManager().SetTimer(DecisionTimer, this, &UALAICombatComponent::EvaluateCombat, 0.25f, true);
    }
}

void UALAICombatComponent::StopCombat()
{
    bCombatActive = false;
    bReactionReady = false;
    bBurstActive = false;
    CurrentTarget.Reset();
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(DecisionTimer);
        GetWorld()->GetTimerManager().ClearTimer(ReactionTimer);
        GetWorld()->GetTimerManager().ClearTimer(BurstTimer);
    }
    if (WeaponComponent) { WeaponComponent->StopFire(); WeaponComponent->StopAim(); }
}

void UALAICombatComponent::SetTargetVisible(bool bVisible)
{
    bTargetVisible = bVisible;
    if (!bVisible && bBurstActive) EndBurst();
}

void UALAICombatComponent::FinishReaction()
{
    bReactionReady = true;
    EvaluateCombat();
}

void UALAICombatComponent::EvaluateCombat()
{
    if (!bCombatActive || !bReactionReady || !WeaponComponent || !CurrentTarget.IsValid()) return;
    APawn* Pawn = Cast<APawn>(GetOwner());
    AActor* Target = CurrentTarget.Get();
    if (!Pawn || !IsValid(Target)) { StopCombat(); return; }
    const float Distance = FVector::Dist(Pawn->GetActorLocation(), Target->GetActorLocation());
    if (!bTargetVisible || Distance > CombatRange * 1.25f)
    {
        EndBurst();
        return;
    }
    if (!bBurstActive) BeginBurst();
}

void UALAICombatComponent::BeginBurst()
{
    if (!bCombatActive || !WeaponComponent || !CurrentTarget.IsValid()) return;
    AALWeaponBase* Weapon = WeaponComponent->GetCurrentWeapon();
    APawn* Pawn = Cast<APawn>(GetOwner());
    if (!Weapon || !Pawn) return;
    const FVector Origin = Pawn->GetPawnViewLocation();
    const FVector Direction = BuildImperfectAim(CurrentTarget.Get());
    Weapon->SetAimDirection(Origin, Direction);
    Weapon->StartAim();
    Weapon->StartFire();
    bBurstActive = true;
    OnAIFired.Broadcast();
    if (GetWorld()) GetWorld()->GetTimerManager().SetTimer(BurstTimer, this, &UALAICombatComponent::EndBurst, FMath::FRandRange(MinBurstTime, MaxBurstTime), false);
}

void UALAICombatComponent::EndBurst()
{
    if (!bBurstActive) return;
    bBurstActive = false;
    if (WeaponComponent) { WeaponComponent->StopFire(); WeaponComponent->StopAim(); }
    if (bCombatActive && GetWorld()) GetWorld()->GetTimerManager().SetTimer(BurstTimer, this, &UALAICombatComponent::EvaluateCombat, FMath::FRandRange(MinPauseTime, MaxPauseTime), false);
}

FVector UALAICombatComponent::BuildImperfectAim(AActor* Target) const
{
    APawn* Pawn = Cast<APawn>(GetOwner());
    if (!Pawn || !IsValid(Target)) return FVector::ForwardVector;
    const FVector Origin = Pawn->GetPawnViewLocation();
    const FVector ToTarget = (Target->GetActorLocation() - Origin).GetSafeNormal();
    const float ErrorDegrees = FMath::Lerp(9.0f, 1.5f, FMath::Clamp(Accuracy, 0.0f, 1.0f));
    return FMath::VRandCone(ToTarget, FMath::DegreesToRadians(ErrorDegrees));
}
