#include "ALAIController.h"
#include "GameFramework/Pawn.h"
AALAIController::AALAIController() { PrimaryActorTick.bCanEverTick = true; }
void AALAIController::SetState(EALAIState NewState) { State = NewState; StateTime = 0.0f; }
void AALAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds); StateTime += DeltaSeconds;
    if ((State == EALAIState::Search || State == EALAIState::Suspicious) && StateTime > 8.0f) SetState(EALAIState::Patrol);
}
