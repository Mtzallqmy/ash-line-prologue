#pragma once
#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ALMissionSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FALMissionEvent, FName, MissionId);
UCLASS()
class ASHLINEMISSIONS_API UALMissionSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintAssignable) FALMissionEvent OnMissionStarted;
    UPROPERTY(BlueprintAssignable) FALMissionEvent OnMissionCompleted;
    UPROPERTY(BlueprintReadOnly) FName ActiveMissionId = NAME_None;
    UFUNCTION(BlueprintCallable) void StartMission(FName MissionId);
    UFUNCTION(BlueprintCallable) void CompleteMission(FName MissionId);
};
