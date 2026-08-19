#pragma once
#include "CoreMinimal.h"
#include "AIController.h"
#include "ALAIController.generated.h"

UENUM(BlueprintType)
enum class EALAIState : uint8 { Idle, Patrol, Suspicious, Alert, Combat, Search };

UCLASS()
class ASHLINEAI_API AALAIController : public AAIController
{
    GENERATED_BODY()
public:
    AALAIController();
    virtual void Tick(float DeltaSeconds) override;
    UPROPERTY(BlueprintReadOnly) EALAIState State = EALAIState::Idle;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float HearingRadius = 1200.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float SightRadius = 1800.0f;
    UFUNCTION(BlueprintCallable) void SetState(EALAIState NewState);
protected:
    float StateTime = 0.0f;
};
