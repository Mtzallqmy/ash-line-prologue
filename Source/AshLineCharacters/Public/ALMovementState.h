#pragma once
#include "CoreMinimal.h"
#include "ALMovementState.generated.h"

UENUM(BlueprintType)
enum class EALMovementState : uint8
{
    Walking,
    Sprinting,
    Crouching,
    Airborne,
    Locked
};
