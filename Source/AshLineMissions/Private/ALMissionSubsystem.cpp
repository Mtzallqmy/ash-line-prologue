#include "ALMissionSubsystem.h"
#include "ALMissionSubsystem.h"

#include "ALSaveGameSubsystem.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"

void UALMissionSubsystem::PersistActiveMissionProgress(bool bCompleted)
{
    if (!HasActiveMission() || !GetWorld() || !GetWorld()->GetGameInstance()) return;
    if (UALSaveGameSubsystem* SaveSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UALSaveGameSubsystem>())
    {
        SaveSubsystem->RecordMissionProgress(ActiveMissionId, ActiveObjectiveIndex, bCompleted);
        SaveSubsystem->SaveCurrent();
    }
}

void UALMissionSubsystem::StartMission(FName MissionId)
{
    if (MissionId.IsNone()) return;
    ActiveMissionId = MissionId;
    ActiveObjectiveIndex = INDEX_NONE;
    ActiveObjectiveProgress = 0.0f;
    OnMissionStarted.Broadcast(MissionId);
}

void UALMissionSubsystem::CompleteMission(FName MissionId)
{
    if (ActiveMissionId != MissionId) return;
    OnMissionCompleted.Broadcast(MissionId);
    PersistActiveMissionProgress(true);
    ActiveMissionId = NAME_None;
    ActiveObjectiveIndex = INDEX_NONE;
    ActiveObjectiveProgress = 0.0f;
}

void UALMissionSubsystem::CancelActiveMission()
{
    ActiveMissionId = NAME_None;
    ActiveObjectiveIndex = INDEX_NONE;
    ActiveObjectiveProgress = 0.0f;
}

void UALMissionSubsystem::ActivateObjective(int32 ObjectiveIndex)
{
    if (!HasActiveMission() || ObjectiveIndex < 0) return;
    ActiveObjectiveIndex = ObjectiveIndex;
    ActiveObjectiveProgress = 0.0f;
    PersistActiveMissionProgress(false);
    OnObjectiveActivated.Broadcast(ActiveMissionId, ActiveObjectiveIndex);
    OnObjectiveProgressChanged.Broadcast(ActiveMissionId, ActiveObjectiveIndex, ActiveObjectiveProgress);
}

void UALMissionSubsystem::SetActiveObjectiveProgress(float Progress)
{
    if (!HasActiveObjective()) return;
    const float ClampedProgress = FMath::Clamp(Progress, 0.0f, 1.0f);
    if (FMath::IsNearlyEqual(ActiveObjectiveProgress, ClampedProgress)) return;
    ActiveObjectiveProgress = ClampedProgress;
    PersistActiveMissionProgress(false);
    OnObjectiveProgressChanged.Broadcast(ActiveMissionId, ActiveObjectiveIndex, ActiveObjectiveProgress);
}

void UALMissionSubsystem::CompleteActiveObjective()
{
    if (!HasActiveObjective()) return;
    SetActiveObjectiveProgress(1.0f);
    OnObjectiveCompleted.Broadcast(ActiveMissionId, ActiveObjectiveIndex);
}
