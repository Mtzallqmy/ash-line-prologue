#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ALInfantryCharacter.generated.h"

UCLASS(Blueprintable)
class ASHLINEAI_API AALInfantryCharacter : public ACharacter
{
    GENERATED_BODY()
public:
    AALInfantryCharacter();
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<class UALHealthComponent> HealthComponent;
};
