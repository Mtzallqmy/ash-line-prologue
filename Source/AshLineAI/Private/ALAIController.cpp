#include "ALAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AIPerceptionTypes.h"

AALAIController::AALAIController()
{
    PrimaryActorTick.bCanEverTick = true;
    PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
    SetPerceptionComponent(*PerceptionComponent);

    UAISenseConfig_Sight* Sight = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    Sight->SightRadius = SightRadius;
    Sight->LoseSightRadius = SightRadius * 1.25f;
    Sight->PeripheralVisionAngleDegrees = 70.0f;
    Sight->DetectionByAffiliation.bDetectEnemies = true;
    Sight->DetectionByAffiliation.bDetectFriendlies = true;
    Sight->DetectionByAffiliation.bDetectNeutrals = true;

    UAISenseConfig_Hearing* Hearing = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
    Hearing->HearingRange = HearingRadius;
    Hearing->DetectionByAffiliation.bDetectEnemies = true;
    Hearing->DetectionByAffiliation.bDetectFriendlies = true;
    Hearing->DetectionByAffiliation.bDetectNeutrals = true;

    PerceptionComponent->ConfigureSense(*Sight);
    PerceptionComponent->ConfigureSense(*Hearing);
    PerceptionComponent->SetDominantSense(Sight->GetSenseImplementation());
    PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AALAIController::OnPerceptionUpdated);
}

void AALAIController::SetState(EALAIState NewState)
{
    State = NewState;
    StateTime = 0.0f;
}

void AALAIController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if (!IsValid(Actor)) return;
    if (Stimulus.WasSuccessfullySensed())
    {
        CurrentTarget = Actor;
        const bool bHeard = Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>();
        SetState(bHeard ? EALAIState::Suspicious : EALAIState::Combat);
    }
    else if (CurrentTarget == Actor)
    {
        SetState(EALAIState::Search);
    }
}

void AALAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    StateTime += DeltaSeconds;
    if (State == EALAIState::Combat && IsValid(CurrentTarget))
    {
        SetFocus(CurrentTarget);
        MoveToActor(CurrentTarget, 700.0f, true);
    }
    else if (State == EALAIState::Search && StateTime > 8.0f)
    {
        ClearFocus(EAIFocusPriority::Gameplay);
        CurrentTarget = nullptr;
        SetState(EALAIState::Patrol);
    }
    else if (State == EALAIState::Suspicious && StateTime > 3.0f)
    {
        SetState(EALAIState::Alert);
    }
    else if (State == EALAIState::Alert && StateTime > 4.0f)
    {
        SetState(EALAIState::Search);
    }
}
