#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "../Damage/ALDamageData.h"
#include "ALDamageReceiver.generated.h"

UINTERFACE(BlueprintType)
class ASHLINECOMBAT_API UALDamageReceiver : public UInterface
{
    GENERATED_BODY()
};

class ASHLINECOMBAT_API IALDamageReceiver
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="ASH LINE|Damage")
    bool CanReceiveALDamage(const FALDamageData& DamageData) const;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="ASH LINE|Damage")
    bool ReceiveALDamage(const FALDamageData& DamageData);
};
