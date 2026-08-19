#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ALWeaponBase.generated.h"

class UALWeaponDataAsset;

UCLASS(Blueprintable)
class ASHLINEWEAPONS_API AALWeaponBase : public AActor
{
    GENERATED_BODY()
public:
    AALWeaponBase();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Weapon") bool Fire(const FVector& Origin, const FVector& Direction, AActor* InstigatorActor);
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Weapon") void Reload();
    UFUNCTION(BlueprintPure, Category="ASH LINE|Weapon") int32 GetAmmo() const { return CurrentAmmo; }
    UFUNCTION(BlueprintPure, Category="ASH LINE|Weapon") int32 GetMagazineSize() const;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon") TObjectPtr<UALWeaponDataAsset> Data;
protected:
    UPROPERTY(VisibleAnywhere) TObjectPtr<USkeletalMeshComponent> Mesh;
    UPROPERTY(BlueprintReadOnly) int32 CurrentAmmo = 0;
    float NextFireTime = 0.0f;
};
