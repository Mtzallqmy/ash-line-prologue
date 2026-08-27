#pragma once
#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ALMissionSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FALMissionEvent, FName, MissionId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FALMissionObjectiveEvent, FName, MissionId, int32, ObjectiveIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FALMissionObjectiveProgressEvent, FName, MissionId, int32, ObjectiveIndex, float, Progress);
UCLASS()
class ASHLINEMISSIONS_API UALMissionSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintAssignable) FALMissionEvent OnMissionStarted;
    UPROPERTY(BlueprintAssignable) FALMissionEvent OnMissionCompleted;
    UPROPERTY(BlueprintAssignable) FALMissionObjectiveEvent OnObjectiveActivated;
    UPROPERTY(BlueprintAssignable) FALMissionObjectiveEvent OnObjectiveCompleted;
    UPROPERTY(BlueprintAssignable) FALMissionObjectiveProgressEvent OnObjectiveProgressChanged;
    UPROPERTY(BlueprintReadOnly) FName ActiveMissionId = NAME_None;
    UPROPERTY(BlueprintReadOnly) int32 ActiveObjectiveIndex = INDEX_NONE;
    UPROPERTY(BlueprintReadOnly) float ActiveObjectiveProgress = 0.0f;
    UFUNCTION(BlueprintCallable) void StartMission(FName MissionId);
    UFUNCTION(BlueprintCallable) void CompleteMission(FName MissionId);
    UFUNCTION(BlueprintCallable) void CancelActiveMission();
    UFUNCTION(BlueprintCallable) void ActivateObjective(int32 ObjectiveIndex);
    UFUNCTION(BlueprintCallable) void SetActiveObjectiveProgress(float Progress);
    UFUNCTION(BlueprintCallable) void CompleteActiveObjective();
    UFUNCTION(BlueprintPure) bool HasActiveMission() const { return !ActiveMissionId.IsNone(); }
    UFUNCTION(BlueprintPure) bool HasActiveObjective() const { return HasActiveMission() && ActiveObjectiveIndex != INDEX_NONE; }
};
