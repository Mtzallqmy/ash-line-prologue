#include "ALSaveGameSubsystem.h"

#include "Kismet/GameplayStatics.h"

void UALSaveGameSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    LoadOrCreateSave();
}

bool UALSaveGameSubsystem::LoadOrCreateSave()
{
    CurrentSave = nullptr;
    if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, UserIndex))
    {
        CurrentSave = Cast<UALSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, UserIndex));
    }
    if (!CurrentSave)
    {
        CurrentSave = Cast<UALSaveGame>(UGameplayStatics::CreateSaveGameObject(UALSaveGame::StaticClass()));
    }
    return IsValid(CurrentSave);
}

bool UALSaveGameSubsystem::SaveCurrent()
{
    return CurrentSave && UGameplayStatics::SaveGameToSlot(CurrentSave, SaveSlotName, UserIndex);
}

FALMissionProgress* UALSaveGameSubsystem::FindMutableMissionProgress(FName MissionId)
{
    if (!CurrentSave || MissionId.IsNone()) return nullptr;
    for (FALMissionProgress& Progress : CurrentSave->Missions)
    {
        if (Progress.MissionId == MissionId) return &Progress;
    }
    return nullptr;
}

void UALSaveGameSubsystem::RecordMissionProgress(FName MissionId, int32 ObjectiveIndex, bool bCompleted)
{
    if (MissionId.IsNone() || (!CurrentSave && !LoadOrCreateSave())) return;
    FALMissionProgress* Progress = FindMutableMissionProgress(MissionId);
    if (!Progress)
    {
        FALMissionProgress NewProgress;
        NewProgress.MissionId = MissionId;
        CurrentSave->Missions.Add(NewProgress);
        Progress = &CurrentSave->Missions.Last();
    }
    Progress->ObjectiveIndex = FMath::Max(0, ObjectiveIndex);
    Progress->bCompleted = bCompleted;
}

bool UALSaveGameSubsystem::GetMissionProgress(FName MissionId, FALMissionProgress& OutProgress) const
{
    if (!CurrentSave || MissionId.IsNone()) return false;
    for (const FALMissionProgress& Progress : CurrentSave->Missions)
    {
        if (Progress.MissionId == MissionId)
        {
            OutProgress = Progress;
            return true;
        }
    }
    return false;
}
