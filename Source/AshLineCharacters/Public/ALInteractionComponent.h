#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ALInteractionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FALInteractionAvailabilityChanged, bool, bAvailable, AActor*, Target);

UCLASS(ClassGroup=(ASHLINE), meta=(BlueprintSpawnableComponent))
class ASHLINECHARACTERS_API UALInteractionComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UALInteractionComponent();
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category="ASH LINE|Interaction") void TryInteract();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Interaction") AActor* FindInteractable() const;
    UFUNCTION(BlueprintPure, Category="ASH LINE|Interaction") AActor* GetCurrentTarget() const { return CurrentTarget.Get(); }
    UFUNCTION(BlueprintPure, Category="ASH LINE|Interaction") FText GetCurrentInteractionText() const;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction") float InteractionRange = 250.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction") float TraceInterval = 0.10f;
    UPROPERTY(BlueprintAssignable, Category="Interaction") FALInteractionAvailabilityChanged OnInteractionAvailabilityChanged;

    void SetInteractionEnabled(bool bEnabled);
protected:
    void RefreshTarget();
    TWeakObjectPtr<AActor> CurrentTarget;
    float TimeSinceTrace = 0.0f;
    bool bInteractionEnabled = true;
};
