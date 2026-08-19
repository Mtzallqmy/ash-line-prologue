#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ALCharacterComponents.generated.h"

UCLASS(ClassGroup=(ASHLINE), meta=(BlueprintSpawnableComponent))
class ASHLINECHARACTERS_API UALInventoryComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, Category="Inventory") TArray<FName> ItemIds;
    UFUNCTION(BlueprintCallable, Category="Inventory") bool HasItem(FName Id) const { return ItemIds.Contains(Id); }
    UFUNCTION(BlueprintCallable, Category="Inventory") void AddItem(FName Id) { ItemIds.AddUnique(Id); }
};

UCLASS(ClassGroup=(ASHLINE), meta=(BlueprintSpawnableComponent))
class ASHLINECHARACTERS_API UALEquipmentComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadOnly, Category="Equipment") FName EquippedId = NAME_None;
    UFUNCTION(BlueprintCallable, Category="Equipment") void Equip(FName Id) { EquippedId = Id; }
};

UCLASS(ClassGroup=(ASHLINE), meta=(BlueprintSpawnableComponent))
class ASHLINECHARACTERS_API UALCombatComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category="Combat") bool FireCurrentWeapon();
    UFUNCTION(BlueprintCallable, Category="Combat") void ReloadCurrentWeapon();
    UPROPERTY(BlueprintReadOnly, Category="Combat") TObjectPtr<class AALWeaponBase> CurrentWeapon;
    UFUNCTION(BlueprintCallable, Category="Combat") void SetCurrentWeapon(AALWeaponBase* Weapon) { CurrentWeapon = Weapon; }
};
