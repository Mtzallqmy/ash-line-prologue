#pragma once
#include "CoreMinimal.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "AI/ALAIStateComponent.h"
#include "ALAIController.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;
class AALInfantryCharacter;

UCLASS()
class ASHLINEAI_API AALAIController : public AAIController
{
    GENERATED_BODY()
public:
    AALAIController();
    virtual void BeginPlay() override;
    virtual void OnPossess(APawn* InPawn) override;
    virtual void OnUnPossess() override;
    virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI") TObjectPtr<UAIPerceptionComponent> PerceptionComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI") TObjectPtr<UAISenseConfig_Sight> SightConfig;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI") TObjectPtr<UAISenseConfig_Hearing> HearingConfig;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI|Performance") float DecisionInterval = 0.35f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI|Performance") float PerceptionInterval = 0.25f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI|Detection") float SightRadius = 1800.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI|Detection") float LoseSightRadius = 2250.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI|Detection") float HearingRadius = 1200.0f;

    UFUNCTION(BlueprintCallable, Category="ASH LINE|AI") void SetState(EALAIState NewState);
    UFUNCTION(BlueprintPure, Category="ASH LINE|AI") EALAIState GetState() const;
    UFUNCTION(BlueprintPure, Category="ASH LINE|AI") AActor* GetCurrentTarget() const;
    UFUNCTION(BlueprintPure, Category="ASH LINE|AI") FVector GetLastKnownTargetLocation() const;
    UFUNCTION(BlueprintCallable, Category="ASH LINE|AI|Development") void SetDebugFrozen(bool bFrozen);
    UFUNCTION(BlueprintPure, Category="ASH LINE|AI|Development") bool IsDebugFrozen() const { return bDebugFrozen; }
    UFUNCTION() void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

protected:
    void ConfigureFromArchetype();
    void ConfigurePerception();
    void RunDecision();
    void EvaluateDetectionState();
    void ProcessNoiseEvents();
    void ExecutePatrol();
    void ExecuteSearch();
    void ExecuteReturning();
    void ExecuteCombat();
    void RequestMoveTo(const FVector& Location, float AcceptanceRadius = 80.0f);
    void BeginPatrolWait();
    void FinishPatrolWait();
    void TransitionToPatrol();
    AALInfantryCharacter* GetEnemy() const;
    bool IsDead() const;
    bool IsTargetWithinSight() const;

    FTimerHandle DecisionTimer;
    FTimerHandle PatrolWaitTimer;
    bool bMoveRequestActive = false;
    bool bPatrolWaiting = false;
    bool bSearchMoveRequested = false;
    bool bCombatMoveRequested = false;
    bool bPerceptionBound = false;
    int32 PatrolPointIndex = 0;
    FVector ActiveMoveDestination = FVector::ZeroVector;
    float LastProcessedNoiseTime = 0.0f;
    bool bDebugFrozen = false;
};
