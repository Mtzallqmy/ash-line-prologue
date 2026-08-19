#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ALDamageTypes.h"
#include "ALHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FALHealthChanged, float, NewHealth, float, Delta);

UCLASS(ClassGroup=(ASHLINE), meta=(BlueprintSpawnableComponent))
class ASHLINECHARACTERS_API UALHealthComponent : public UActorComponent, public IALDamageableInterface
{
    GENERATED_BODY()
public:
    UALHealthComponent();
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Health") float MaxHealth = 100.0f;
    UPROPERTY(BlueprintReadOnly, Category="Health") float CurrentHealth = 100.0f;
    UPROPERTY(BlueprintAssignable) FALHealthChanged OnHealthChanged;
    UFUNCTION(BlueprintCallable) void RestoreFullHealth();
    virtual bool ReceiveALDamage_Implementation(const FALDamageEvent& Event) override;
    UFUNCTION(BlueprintPure) bool IsDead() const { return CurrentHealth <= 0.0f; }
};
