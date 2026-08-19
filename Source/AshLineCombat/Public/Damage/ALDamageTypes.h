#pragma once
#include "CoreMinimal.h"
#include "ALDamageTypes.generated.h"

UENUM(BlueprintType)
enum class EALDamageType : uint8
{
    Generic,
    Bullet,
    Explosion,
    Melee,
    Fall,
    Fire,
    Vehicle,
    Environmental
};
