#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ALPatrolRoute.generated.h"

UCLASS(Blueprintable)
class ASHLINEAI_API AALPatrolRoute : public AActor
{
    GENERATED_BODY()
public:
    AALPatrolRoute();
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Patrol") TArray<FVector> PatrolPoints;
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Patrol") TArray<float> WaitTimes;
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Patrol") bool bLoop = true;
    UFUNCTION(BlueprintPure, Category="ASH LINE|AI") int32 GetPointCount() const { return PatrolPoints.Num(); }
    UFUNCTION(BlueprintPure, Category="ASH LINE|AI") FVector GetPoint(int32 Index) const;
    UFUNCTION(BlueprintPure, Category="ASH LINE|AI") float GetWaitTime(int32 Index) const;
    UFUNCTION(BlueprintPure, Category="ASH LINE|AI") int32 GetNextIndex(int32 Index) const;
};
