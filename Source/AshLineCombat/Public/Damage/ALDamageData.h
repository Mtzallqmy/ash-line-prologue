#pragma once
#include "CoreMinimal.h"
#include "ALDamageTypes.h"
#include "ALDamageData.generated.h"

USTRUCT(BlueprintType)
struct ASHLINECOMBAT_API FALDamageData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Damage") float BaseDamage = 0.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Damage") EALDamageType DamageType = EALDamageType::Generic;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Damage") TWeakObjectPtr<AController> Instigator;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Damage") TWeakObjectPtr<AActor> DamageCauser;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hit") FVector HitLocation = FVector::ZeroVector;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hit") FVector HitNormal = FVector::UpVector;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hit") FName BoneName = NAME_None;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hit") bool bHasHitResult = false;

    // Extension points for armor, tags, surfaces, and impact responses in later prompts.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Future Modifiers") float ArmorPenetration = 0.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Future Modifiers") FVector Impulse = FVector::ZeroVector;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Future Modifiers") FName SurfaceType = NAME_None;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Future Modifiers") float CriticalMultiplier = 1.0f;
};
