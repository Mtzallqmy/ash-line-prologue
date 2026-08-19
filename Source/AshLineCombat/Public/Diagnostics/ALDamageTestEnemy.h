#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ALDamageTestEnemy.generated.h"

UCLASS(Blueprintable)
class ASHLINECOMBAT_API AALDamageTestEnemy : public ACharacter
{
    GENERATED_BODY()
public:
    AALDamageTestEnemy();
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Health") TObjectPtr<class UALHealthComponent> HealthComponent;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Death") float CorpseLifetime = 0.0f;
protected:
    virtual void BeginPlay() override;
    UFUNCTION() void HandleDeath();
    bool bDeathHandled = false;
};
