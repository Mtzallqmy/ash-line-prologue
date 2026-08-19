#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ALPrologueGameMode.generated.h"

UCLASS(Blueprintable)
class ASHLINECHARACTERS_API AALPrologueGameMode : public AGameModeBase
{
    GENERATED_BODY()
public:
    AALPrologueGameMode();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ASH LINE|Player") TSubclassOf<class AALPlayerCharacter> ProloguePlayerClass;
};
