#pragma once
#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "ALSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FALMissionProgress
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FName MissionId = NAME_None;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 ObjectiveIndex = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bCompleted = false;
};

UCLASS(BlueprintType)
class ASHLINECORE_API UALSaveGame : public USaveGame
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite) int32 SaveSchemaVersion = 1;
    UPROPERTY(BlueprintReadWrite) TArray<FALMissionProgress> Missions;
    UPROPERTY(BlueprintReadWrite) TArray<FName> UnlockedEquipment;
    UPROPERTY(BlueprintReadWrite) TMap<FName, FString> Settings;
    UPROPERTY(BlueprintReadWrite) int32 PlayerExperience = 0;
};
