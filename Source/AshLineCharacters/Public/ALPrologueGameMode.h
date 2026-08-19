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

    UFUNCTION(BlueprintCallable, Category="ASH LINE|Death") void HandlePlayerRestartRequested();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ASH LINE|Player") TSubclassOf<class AALPlayerCharacter> ProloguePlayerClass;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ASH LINE|Death") bool bUseConfiguredCheckpoint = false;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ASH LINE|Death", meta=(EditCondition="bUseConfiguredCheckpoint")) FTransform CheckpointTransform;
};
