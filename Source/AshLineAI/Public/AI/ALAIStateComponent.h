#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ALAIStateComponent.generated.h"

UENUM(BlueprintType)
enum class EALAIState : uint8
{
    Idle,
    Patrol,
    Suspicious,
    Alert,
    Combat,
    Search,
    Returning,
    Dead
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FALAIStateChanged, EALAIState, PreviousState, EALAIState, NewState);

UCLASS(ClassGroup=(ASHLINE), meta=(BlueprintSpawnableComponent))
class ASHLINEAI_API UALAIStateComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UALAIStateComponent();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|AI") void SetState(EALAIState NewState);
    UFUNCTION(BlueprintPure, Category="ASH LINE|AI") EALAIState GetState() const { return State; }
    UFUNCTION(BlueprintPure, Category="ASH LINE|AI") float GetStateTime() const;
    UFUNCTION(BlueprintCallable, Category="ASH LINE|AI") void MarkStateTime(float InStateTime);
    UPROPERTY(BlueprintAssignable, Category="ASH LINE|AI") FALAIStateChanged OnStateChanged;
protected:
    EALAIState State = EALAIState::Idle;
    float StateStartedAt = 0.0f;
};
