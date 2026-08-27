#include "ALMissionSubsystem.h"
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
    OnObjectiveActivated.Broadcast(ActiveMissionId, ActiveObjectiveIndex);
    OnObjectiveProgressChanged.Broadcast(ActiveMissionId, ActiveObjectiveIndex, ActiveObjectiveProgress);
}

void UALMissionSubsystem::SetActiveObjectiveProgress(float Progress)
{
    if (!HasActiveObjective()) return;
    const float ClampedProgress = FMath::Clamp(Progress, 0.0f, 1.0f);
    if (FMath::IsNearlyEqual(ActiveObjectiveProgress, ClampedProgress)) return;
    ActiveObjectiveProgress = ClampedProgress;
    OnObjectiveProgressChanged.Broadcast(ActiveMissionId, ActiveObjectiveIndex, ActiveObjectiveProgress);
}

void UALMissionSubsystem::CompleteActiveObjective()
{
    if (!HasActiveObjective()) return;
    SetActiveObjectiveProgress(1.0f);
    OnObjectiveCompleted.Broadcast(ActiveMissionId, ActiveObjectiveIndex);
}
