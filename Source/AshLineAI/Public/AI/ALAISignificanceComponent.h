#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ALAISignificanceComponent.generated.h"

UENUM(BlueprintType)
enum class EALAISignificance : uint8
{
    Near,
    Medium,
    Far,
    Inactive
};

UCLASS(ClassGroup=(ASHLINE), meta=(BlueprintSpawnableComponent))
class ASHLINEAI_API UALAISignificanceComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UALAISignificanceComponent();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|AI") void UpdateSignificance(float DistanceToPlayer);
    UFUNCTION(BlueprintPure, Category="ASH LINE|AI") EALAISignificance GetSignificance() const { return Significance; }
    UFUNCTION(BlueprintPure, Category="ASH LINE|AI") float GetUpdateMultiplier() const;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AI|Significance") float NearDistance = 1800.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AI|Significance") float MediumDistance = 4500.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AI|Significance") float FarDistance = 8000.0f;
protected:
    EALAISignificance Significance = EALAISignificance::Near;
};
