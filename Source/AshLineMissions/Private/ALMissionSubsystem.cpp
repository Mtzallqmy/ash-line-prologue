#include "ALMissionSubsystem.h"
void UALMissionSubsystem::StartMission(FName MissionId) { ActiveMissionId = MissionId; OnMissionStarted.Broadcast(MissionId); }
void UALMissionSubsystem::CompleteMission(FName MissionId) { if (ActiveMissionId == MissionId) { OnMissionCompleted.Broadcast(MissionId); ActiveMissionId = NAME_None; } }
