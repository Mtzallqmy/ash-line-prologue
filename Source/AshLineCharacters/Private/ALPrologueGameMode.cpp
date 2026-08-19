#include "ALPrologueGameMode.h"
#include "ALPlayerCharacter.h"
#include "ALPlayerController.h"

AALPrologueGameMode::AALPrologueGameMode()
{
    PlayerControllerClass = AALPlayerController::StaticClass();
    DefaultPawnClass = AALPlayerCharacter::StaticClass();
    ProloguePlayerClass = AALPlayerCharacter::StaticClass();
}
