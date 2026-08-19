#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Damage/ALDamageData.h"
#include "ALAISenseComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FALAITargetEvent, AActor*, Target);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FALAIAwarenessChanged, AActor*, Target, float, Awareness);

UCLASS(ClassGroup=(ASHLINE), meta=(BlueprintSpawnableComponent))
class ASHLINEAI_API UALAISenseComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UALAISenseComponent();
    void ProcessSightStimulus(AActor* Actor, bool bSensed, const FVector& StimulusLocation);
    void ProcessHearingStimulus(AActor* Actor, const FVector& StimulusLocation);
    void ProcessDamageAwareness(const FALDamageData& DamageData);
    void DecayAwareness(float DeltaSeconds);
    void ClearTarget();

    UFUNCTION(BlueprintPure, Category="ASH LINE|AI") AActor* GetCurrentTarget() const { return CurrentTarget.Get(); }
    UFUNCTION(BlueprintPure, Category="ASH LINE|AI") FVector GetLastKnownLocation() const { return LastKnownLocation; }
    UFUNCTION(BlueprintPure, Category="ASH LINE|AI") float GetAwareness() const { return Awareness; }
    UFUNCTION(BlueprintPure, Category="ASH LINE|AI") bool IsTargetVisible() const { return bTargetVisible; }
    UFUNCTION(BlueprintPure, Category="ASH LINE|AI") float GetLastSeenTime() const { return LastSeenTime; }

    UPROPERTY(BlueprintAssignable, Category="ASH LINE|AI") FALAITargetEvent OnTargetDetected;
    UPROPERTY(BlueprintAssignable, Category="ASH LINE|AI") FALAITargetEvent OnTargetLost;
    UPROPERTY(BlueprintAssignable, Category="ASH LINE|AI") FALAIAwarenessChanged OnAwarenessChanged;
    UPROPERTY(BlueprintAssignable, Category="ASH LINE|AI") FALAITargetEvent OnNoiseHeard;

protected:
    void SetAwareness(float NewAwareness);
    TWeakObjectPtr<AActor> CurrentTarget;
    FVector LastKnownLocation = FVector::ZeroVector;
    float Awareness = 0.0f;
    float LastSeenTime = -1.0f;
    bool bTargetVisible = false;
};
