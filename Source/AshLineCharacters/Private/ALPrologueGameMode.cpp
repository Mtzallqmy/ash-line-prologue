#include "ALPrologueGameMode.h"
#include "ALPlayerCharacter.h"
#include "ALPlayerController.h"

AALPrologueGameMode::AALPrologueGameMode()
{
    PlayerControllerClass = AALPlayerController::StaticClass();
    DefaultPawnClass = AALPlayerCharacter::StaticClass();
    ProloguePlayerClass = AALPlayerCharacter::StaticClass();
}

void AALPrologueGameMode::HandlePlayerRestartRequested()
{
    AALPlayerCharacter* Player = nullptr;
    if (GetWorld() && GetWorld()->GetFirstPlayerController()) Player = Cast<AALPlayerCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
    if (!IsValid(Player)) return;
    const FTransform RestartTransform = bUseConfiguredCheckpoint ? CheckpointTransform : Player->GetActorTransform();
    Player->ApplyCheckpointRestart(RestartTransform);
}
