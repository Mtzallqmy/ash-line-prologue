#pragma once
#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ALNoiseSystemSubsystem.generated.h"

USTRUCT(BlueprintType)
struct ASHLINECORE_API FALNoiseEvent
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadOnly) TWeakObjectPtr<AActor> Source;
    UPROPERTY(BlueprintReadOnly) FVector Location = FVector::ZeroVector;
    UPROPERTY(BlueprintReadOnly) float Loudness = 1.0f;
    UPROPERTY(BlueprintReadOnly) float TimeSeconds = 0.0f;
    UPROPERTY(BlueprintReadOnly) FName Tag = NAME_None;
};

UCLASS()
class ASHLINECORE_API UALNoiseSystemSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()
public:
    void ReportNoise(AActor* Source, const FVector& Location, float Loudness, FName Tag);
    void GetRecentEvents(float SinceTime, TArray<FALNoiseEvent>& OutEvents) const;
protected:
    TArray<FALNoiseEvent> Events;
};
