#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ALHealthConfig.generated.h"

USTRUCT(BlueprintType)
struct ASHLINECOMBAT_API FALFallDamageSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Fall Damage") float MinimumFallSpeed = 900.0f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Fall Damage") float FatalFallSpeed = 2200.0f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Fall Damage") float MaxFallDamage = 100.0f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Fall Damage") TSoftObjectPtr<class UCurveFloat> DamageCurve;
};

UCLASS(BlueprintType)
class ASHLINECOMBAT_API UALHealthConfig : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Health") float MaxHealth = 100.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Health") float InitialInvulnerabilityDuration = 0.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Fall Damage") FALFallDamageSettings FallDamage;
};
