#pragma once
#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ALDamageTypes.h"
#include "ALDamageSystemSubsystem.generated.h"

UCLASS()
class ASHLINECOMBAT_API UALDamageSystemSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Damage") bool ApplyDamage(AActor* Target, const FALDamageEvent& Event);
};
