#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ALDamageDebugLibrary.generated.h"

UCLASS()
class ASHLINECOMBAT_API UALDamageDebugLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Development") static bool DamageActor(AActor* Target, float Amount);
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Development") static bool HealActor(AActor* Target, float Amount);
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Development") static bool KillActor(AActor* Target);
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Development") static bool SetGodMode(AActor* Target, bool bEnabled);
};
