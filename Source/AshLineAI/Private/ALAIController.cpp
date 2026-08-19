#include "ALAIController.h"
#include "ALNoiseSystemSubsystem.h"
#include "ALInfantryCharacter.h"
#include "AI/ALAISenseComponent.h"
#include "AI/ALAICombatComponent.h"
#include "AI/ALAICoverComponent.h"
#include "AI/ALAISignificanceComponent.h"
#include "Data/ALAIArchetypeData.h"
#include "World/ALPatrolRoute.h"
#include "World/ALCoverPoint.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

AALAIController::AALAIController()
{
    PrimaryActorTick.bCanEverTick = false;
    PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
    SetPerceptionComponent(*PerceptionComponent);
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
    ConfigurePerception();
}

void AALAIController::BeginPlay()
{
    Super::BeginPlay();
    if (GetWorld()) GetWorld()->GetTimerManager().SetTimer(DecisionTimer, this, &AALAIController::RunDecision, DecisionInterval + FMath::FRandRange(-0.04f, 0.04f), true);
}

void AALAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    ConfigureFromArchetype();
    if (AALInfantryCharacter* Enemy = GetEnemy())
    {
        if (Enemy->AIStateComponent) Enemy->AIStateComponent->SetState(Enemy->PatrolRoute ? EALAIState::Patrol : EALAIState::Idle);
    }
}

void AALAIController::OnUnPossess()
{
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(DecisionTimer);
        GetWorld()->GetTimerManager().ClearTimer(PatrolWaitTimer);
    }
    if (AALInfantryCharacter* Enemy = GetEnemy())
    {
        if (Enemy->CombatComponent) Enemy->CombatComponent->StopCombat();
        if (Enemy->CoverComponent) Enemy->CoverComponent->ReleaseCover();
    }
    StopMovement();
    Super::OnUnPossess();
}

void AALAIController::ConfigurePerception()
{
    if (!SightConfig || !HearingConfig || !PerceptionComponent) return;
    SightConfig->SightRadius = SightRadius;
    SightConfig->LoseSightRadius = LoseSightRadius;
    SightConfig->PeripheralVisionAngleDegrees = 70.0f;
    SightConfig->MaxAge = 1.25f;
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    HearingConfig->HearingRange = HearingRadius;
    HearingConfig->MaxAge = 1.5f;
    HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
    HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
    HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
    PerceptionComponent->ConfigureSense(*SightConfig);
    PerceptionComponent->ConfigureSense(*HearingConfig);
    PerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
    if (!bPerceptionBound)
    {
        PerceptionComponent->OnTargetPerceptionUpdated.AddUniqueDynamic(this, &AALAIController::OnPerceptionUpdated);
        bPerceptionBound = true;
    }
}

void AALAIController::ConfigureFromArchetype()
{
    if (AALInfantryCharacter* Enemy = GetEnemy())
    {
        if (const UALAIArchetypeData* Archetype = Enemy->Archetype)
        {
            SightRadius = Archetype->SightRange;
            LoseSightRadius = Archetype->LoseSightRange;
            HearingRadius = Archetype->HearingRange;
            DecisionInterval = Archetype->DecisionInterval;
            ConfigurePerception();
            if (Enemy->CombatComponent) Enemy->CombatComponent->ConfigureFromArchetype(Archetype);
            if (Enemy->GetCharacterMovement()) Enemy->GetCharacterMovement()->MaxWalkSpeed = Archetype->PatrolSpeed;
        }
    }
}

void AALAIController::SetDebugFrozen(bool bFrozen)
{
#if !UE_BUILD_SHIPPING
    bDebugFrozen = bFrozen;
    if (bDebugFrozen)
    {
        StopMovement();
        if (AALInfantryCharacter* Enemy = GetEnemy()) if (Enemy->CombatComponent) Enemy->CombatComponent->StopCombat();
    }
#else
    bDebugFrozen = false;
#endif
}

void AALAIController::SetState(EALAIState NewState)
{
    if (AALInfantryCharacter* Enemy = GetEnemy())
    {
        if (Enemy->AIStateComponent)
        {
            const EALAIState Previous = Enemy->AIStateComponent->GetState();
            Enemy->AIStateComponent->SetState(NewState);
            if (Previous != EALAIState::Combat && NewState == EALAIState::Combat && GetWorld())
            {
                if (UALNoiseSystemSubsystem* NoiseSystem = GetWorld()->GetSubsystem<UALNoiseSystemSubsystem>()) NoiseSystem->ReportNoise(Enemy, Enemy->GetActorLocation(), 0.7f, TEXT("AIAlert"));
            }
        }
    }
}

EALAIState AALAIController::GetState() const
{
    if (const AALInfantryCharacter* Enemy = GetEnemy()) return Enemy->AIStateComponent ? Enemy->AIStateComponent->GetState() : EALAIState::Idle;
    return EALAIState::Dead;
}

AActor* AALAIController::GetCurrentTarget() const
{
    if (const AALInfantryCharacter* Enemy = GetEnemy()) return Enemy->SenseComponent ? Enemy->SenseComponent->GetCurrentTarget() : nullptr;
    return nullptr;
}

FVector AALAIController::GetLastKnownTargetLocation() const
{
    if (const AALInfantryCharacter* Enemy = GetEnemy()) return Enemy->SenseComponent ? Enemy->SenseComponent->GetLastKnownLocation() : FVector::ZeroVector;
    return FVector::ZeroVector;
}

AALInfantryCharacter* AALAIController::GetEnemy() const { return Cast<AALInfantryCharacter>(GetPawn()); }
bool AALAIController::IsDead() const { return GetState() == EALAIState::Dead || (GetEnemy() && GetEnemy()->HealthComponent && GetEnemy()->HealthComponent->IsDead()); }

void AALAIController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    AALInfantryCharacter* Enemy = GetEnemy();
    if (!Enemy || IsDead() || !IsValid(Actor) || Actor == Enemy) return;
    const bool bHearing = Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>();
    const FVector Location = Stimulus.StimulusLocation;
    if (Stimulus.WasSuccessfullySensed())
    {
        if (Enemy->SenseComponent)
        {
            if (bHearing) Enemy->SenseComponent->ProcessHearingStimulus(Actor, Location);
            else Enemy->SenseComponent->ProcessSightStimulus(Actor, true, Location);
        }
    }
    else if (Enemy->SenseComponent && !bHearing)
    {
        Enemy->SenseComponent->ProcessSightStimulus(Actor, false, Location);
    }
}

bool AALAIController::IsTargetWithinSight() const
{
    const AALInfantryCharacter* Enemy = GetEnemy();
    return Enemy && Enemy->SenseComponent && Enemy->SenseComponent->IsTargetVisible();
}

void AALAIController::RunDecision()
{
    if (bDebugFrozen) return;
    if (!GetPawn() || IsDead())
    {
        if (GetWorld()) GetWorld()->GetTimerManager().ClearTimer(DecisionTimer);
        return;
    }
    AALInfantryCharacter* Enemy = GetEnemy();
    if (!Enemy || !Enemy->AIStateComponent || !Enemy->SenseComponent) return;
    if (Enemy->SignificanceComponent)
    {
        if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0)) Enemy->SignificanceComponent->UpdateSignificance(FVector::Dist(Enemy->GetActorLocation(), PlayerPawn->GetActorLocation()));
        if (Enemy->SignificanceComponent->GetSignificance() == EALAISignificance::Inactive) return;
    }
    Enemy->SenseComponent->DecayAwareness(DecisionInterval);
    ProcessNoiseEvents();
    if (Enemy->SenseComponent->IsTargetVisible() && IsValid(Enemy->SenseComponent->GetCurrentTarget()))
    {
        AActor* VisibleTarget = Enemy->SenseComponent->GetCurrentTarget();
        Enemy->SenseComponent->ProcessSightStimulus(VisibleTarget, true, VisibleTarget->GetActorLocation());
    }
    EvaluateDetectionState();
    switch (GetState())
    {
        case EALAIState::Idle: ExecutePatrol(); break;
        case EALAIState::Patrol: ExecutePatrol(); break;
        case EALAIState::Suspicious: ExecuteSearch(); break;
        case EALAIState::Alert: ExecuteSearch(); break;
        case EALAIState::Combat: ExecuteCombat(); break;
        case EALAIState::Search: ExecuteSearch(); break;
        case EALAIState::Returning: ExecuteReturning(); break;
        case EALAIState::Dead: break;
    }
}

void AALAIController::ProcessNoiseEvents()
{
    AALInfantryCharacter* Enemy = GetEnemy();
    if (!Enemy || !Enemy->SenseComponent || !GetWorld()) return;
    UALNoiseSystemSubsystem* NoiseSystem = GetWorld()->GetSubsystem<UALNoiseSystemSubsystem>();
    if (!NoiseSystem) return;
    const float Now = GetWorld()->GetTimeSeconds();
    TArray<FALNoiseEvent> RecentEvents;
    NoiseSystem->GetRecentEvents(LastProcessedNoiseTime, RecentEvents);
    LastProcessedNoiseTime = Now;
    const float Hearing = Enemy->Archetype ? Enemy->Archetype->HearingRange : HearingRadius;
    for (const FALNoiseEvent& Event : RecentEvents)
    {
        AActor* Source = Event.Source.Get();
        if (!IsValid(Source) || Source == Enemy) continue;
        const float EffectiveRange = Hearing * FMath::Max(Event.Loudness, 0.1f);
        if (FVector::DistSquared(Event.Location, Enemy->GetActorLocation()) <= FMath::Square(EffectiveRange))
        {
            Enemy->SenseComponent->ProcessHearingStimulus(Source, Event.Location);
        }
    }
}

void AALAIController::EvaluateDetectionState()
{
    AALInfantryCharacter* Enemy = GetEnemy();
    if (!Enemy || !Enemy->SenseComponent) return;
    const float Awareness = Enemy->SenseComponent->GetAwareness();
    const bool bVisible = Enemy->SenseComponent->IsTargetVisible();
    const UALAIArchetypeData* Archetype = Enemy->Archetype;
    const float SuspiciousThreshold = Archetype ? Archetype->SuspiciousThreshold : 0.3f;
    const float AlertThreshold = Archetype ? Archetype->AlertThreshold : 0.7f;
    const float CombatThreshold = Archetype ? Archetype->CombatThreshold : 1.0f;
    if (bVisible && Awareness >= CombatThreshold) { SetState(EALAIState::Combat); return; }
    if (Awareness >= AlertThreshold) { if (GetState() != EALAIState::Combat) SetState(EALAIState::Alert); return; }
    if (Awareness >= SuspiciousThreshold) { if (GetState() == EALAIState::Idle || GetState() == EALAIState::Patrol) SetState(EALAIState::Suspicious); return; }
    if (!bVisible && (GetState() == EALAIState::Alert || GetState() == EALAIState::Combat)) SetState(EALAIState::Search);
}

void AALAIController::ExecutePatrol()
{
    AALInfantryCharacter* Enemy = GetEnemy();
    if (!Enemy || !Enemy->PatrolRoute || Enemy->PatrolRoute->GetPointCount() == 0) return;
    if (bPatrolWaiting || bMoveRequestActive) return;
    const FVector Destination = Enemy->PatrolRoute->GetPoint(PatrolPointIndex);
    RequestMoveTo(Destination, 90.0f);
}

void AALAIController::ExecuteSearch()
{
    AALInfantryCharacter* Enemy = GetEnemy();
    if (!Enemy || !Enemy->SenseComponent) return;
    if (Enemy->SenseComponent->IsTargetVisible()) { SetState(EALAIState::Combat); bSearchMoveRequested = false; return; }
    if (GetState() == EALAIState::Suspicious || GetState() == EALAIState::Alert)
    {
        if (Enemy->AIStateComponent->GetStateTime() > 1.2f) SetState(EALAIState::Search);
        return;
    }
    if (GetState() == EALAIState::Search)
    {
        const float SearchDuration = Enemy->Archetype ? Enemy->Archetype->SearchDuration : 8.0f;
        if (Enemy->AIStateComponent->GetStateTime() > SearchDuration) { SetState(EALAIState::Returning); bSearchMoveRequested = false; return; }
        if (!bSearchMoveRequested) { RequestMoveTo(Enemy->SenseComponent->GetLastKnownLocation(), 120.0f); bSearchMoveRequested = true; }
    }
}

void AALAIController::ExecuteReturning()
{
    AALInfantryCharacter* Enemy = GetEnemy();
    if (!Enemy || !Enemy->PatrolRoute || Enemy->PatrolRoute->GetPointCount() == 0) { SetState(EALAIState::Patrol); return; }
    if (!bMoveRequestActive) RequestMoveTo(Enemy->PatrolRoute->GetPoint(PatrolPointIndex), 120.0f);
}

void AALAIController::ExecuteCombat()
{
    AALInfantryCharacter* Enemy = GetEnemy();
    AActor* Target = GetCurrentTarget();
    if (!Enemy || !Target || !Enemy->CombatComponent || !Enemy->SenseComponent) { SetState(EALAIState::Search); return; }
    if (!Enemy->SenseComponent->IsTargetVisible()) { Enemy->CombatComponent->SetTargetVisible(false); SetState(EALAIState::Search); return; }
    Enemy->CombatComponent->SetTargetVisible(true);
    Enemy->CombatComponent->StartCombat(Target);
    if (!Enemy->CoverComponent || !Enemy->CoverComponent->HasCover())
    {
        const float Preference = Enemy->Archetype ? Enemy->Archetype->CoverPreference : 0.35f;
        const float Radius = Enemy->Archetype ? Enemy->Archetype->CoverSearchRadius : 900.0f;
        if (FMath::FRand() <= Preference && Enemy->CoverComponent)
        {
            if (AALCoverPoint* Cover = Enemy->CoverComponent->FindAndReserveCover(Radius, false)) RequestMoveTo(Cover->GetActorLocation(), 100.0f);
        }
    }
}

void AALAIController::RequestMoveTo(const FVector& Location, float AcceptanceRadius)
{
    if (IsDead() || bMoveRequestActive || Location.IsNearlyZero()) return;
    ActiveMoveDestination = Location;
    bMoveRequestActive = true;
    MoveToLocation(Location, AcceptanceRadius, true);
}

void AALAIController::BeginPatrolWait()
{
    bPatrolWaiting = true;
    bMoveRequestActive = false;
    const AALInfantryCharacter* Enemy = GetEnemy();
    const float Wait = Enemy && Enemy->PatrolRoute ? Enemy->PatrolRoute->GetWaitTime(PatrolPointIndex) : 0.5f;
    if (GetWorld()) GetWorld()->GetTimerManager().SetTimer(PatrolWaitTimer, this, &AALAIController::FinishPatrolWait, Wait, false);
}

void AALAIController::FinishPatrolWait()
{
    bPatrolWaiting = false;
    if (const AALInfantryCharacter* Enemy = GetEnemy())
    {
        if (Enemy->PatrolRoute) PatrolPointIndex = Enemy->PatrolRoute->GetNextIndex(PatrolPointIndex);
        if (PatrolPointIndex == INDEX_NONE) SetState(EALAIState::Returning);
    }
}

void AALAIController::TransitionToPatrol()
{
    bSearchMoveRequested = false;
    bCombatMoveRequested = false;
    SetState(EALAIState::Patrol);
}

void AALAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
    Super::OnMoveCompleted(RequestID, Result);
    bMoveRequestActive = false;
    if (!Result.IsSuccess()) return;
    if (GetState() == EALAIState::Patrol) BeginPatrolWait();
    else if (GetState() == EALAIState::Search) bSearchMoveRequested = true;
    else if (GetState() == EALAIState::Returning) TransitionToPatrol();
}
