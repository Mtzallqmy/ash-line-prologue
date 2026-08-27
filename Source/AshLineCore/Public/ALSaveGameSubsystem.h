#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ALSaveGame.h"
#include "ALSaveGameSubsystem.generated.h"

class UALSaveGame;

UCLASS()
class ASHLINECORE_API UALSaveGameSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable, Category="ASH LINE|Save") bool LoadOrCreateSave();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Save") bool SaveCurrent();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Save") void RecordMissionProgress(FName MissionId, int32 ObjectiveIndex, bool bCompleted);
    UFUNCTION(BlueprintPure, Category="ASH LINE|Save") bool GetMissionProgress(FName MissionId, FALMissionProgress& OutProgress) const;
    UFUNCTION(BlueprintPure, Category="ASH LINE|Save") bool IsSaveReady() const { return IsValid(CurrentSave); }

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ASH LINE|Save") FString SaveSlotName = TEXT("ASH_LINE_Profile");
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ASH LINE|Save") int32 UserIndex = 0;

protected:
    UPROPERTY() TObjectPtr<UALSaveGame> CurrentSave;

    FALMissionProgress* FindMutableMissionProgress(FName MissionId);
};
