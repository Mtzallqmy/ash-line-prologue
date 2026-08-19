#pragma once
#include "CoreMinimal.h"
#include "AIController.h"
#include "ALAIController.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;

UENUM(BlueprintType)
enum class EALAIState : uint8 { Idle, Patrol, Suspicious, Alert, Combat, Search };

UCLASS()
class ASHLINEAI_API AALAIController : public AAIController
{
    GENERATED_BODY()
public:
    AALAIController();
    virtual void Tick(float DeltaSeconds) override;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<UAIPerceptionComponent> PerceptionComponent;
    UPROPERTY(BlueprintReadOnly) EALAIState State = EALAIState::Idle;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float HearingRadius = 1200.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float SightRadius = 1800.0f;
    UFUNCTION(BlueprintCallable) void SetState(EALAIState NewState);
    UFUNCTION() void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
protected:
    float StateTime = 0.0f;
    UPROPERTY() TObjectPtr<AActor> CurrentTarget;
};
