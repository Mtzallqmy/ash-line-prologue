#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ALDamageTypes.generated.h"

UENUM(BlueprintType)
enum class EALDamageType : uint8
{
    Ballistic,
    Explosion,
    Melee,
    Environmental
};

USTRUCT(BlueprintType)
struct FALDamageEvent
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite) float Amount = 0.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) EALDamageType Type = EALDamageType::Ballistic;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector Origin = FVector::ZeroVector;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FName InstigatorId = NAME_None;
};

UINTERFACE(BlueprintType)
class ASHLINECORE_API UALDamageableInterface : public UInterface
{
    GENERATED_BODY()
};

class ASHLINECORE_API IALDamageableInterface
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="ASH LINE|Damage")
    bool ReceiveALDamage(const FALDamageEvent& Event);
};
