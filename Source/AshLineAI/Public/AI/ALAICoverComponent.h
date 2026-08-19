#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ALAICoverComponent.generated.h"

class AALCoverPoint;

UCLASS(ClassGroup=(ASHLINE), meta=(BlueprintSpawnableComponent))
class ASHLINEAI_API UALAICoverComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UALAICoverComponent();
    AALCoverPoint* FindAndReserveCover(float SearchRadius, bool bPreferCrouch);
    void ReleaseCover();
    UFUNCTION(BlueprintPure, Category="ASH LINE|AI") AALCoverPoint* GetCurrentCover() const { return CurrentCover.Get(); }
    UFUNCTION(BlueprintPure, Category="ASH LINE|AI") bool HasCover() const { return CurrentCover.IsValid(); }
protected:
    TWeakObjectPtr<AALCoverPoint> CurrentCover;
};
