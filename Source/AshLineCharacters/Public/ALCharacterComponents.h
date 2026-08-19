#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ALCharacterComponents.generated.h"

UCLASS(ClassGroup=(ASHLINE), meta=(BlueprintSpawnableComponent))
class ASHLINECHARACTERS_API UALInventoryComponent : public UActorComponent
{ GENERATED_BODY() public: UPROPERTY(BlueprintReadWrite) TArray<FName> ItemIds; UFUNCTION(BlueprintCallable) bool HasItem(FName Id) const { return ItemIds.Contains(Id); } UFUNCTION(BlueprintCallable) void AddItem(FName Id) { ItemIds.AddUnique(Id); } };

UCLASS(ClassGroup=(ASHLINE), meta=(BlueprintSpawnableComponent))
class ASHLINECHARACTERS_API UALEquipmentComponent : public UActorComponent
{ GENERATED_BODY() public: UPROPERTY(BlueprintReadOnly) FName EquippedId = NAME_None; UFUNCTION(BlueprintCallable) void Equip(FName Id) { EquippedId = Id; } };

UCLASS(ClassGroup=(ASHLINE), meta=(BlueprintSpawnableComponent))
class ASHLINECHARACTERS_API UALInteractionComponent : public UActorComponent
{ GENERATED_BODY() public: UPROPERTY(EditAnywhere, BlueprintReadWrite) float InteractionRange = 250.0f; UFUNCTION(BlueprintCallable) AActor* FindInteractable() const; };

UCLASS(ClassGroup=(ASHLINE), meta=(BlueprintSpawnableComponent))
class ASHLINECHARACTERS_API UALPlayerStateComponent : public UActorComponent
{ GENERATED_BODY() public: UPROPERTY(BlueprintReadWrite) int32 Experience = 0; UPROPERTY(BlueprintReadWrite) FName ActiveMissionId = NAME_None; };

UCLASS(ClassGroup=(ASHLINE), meta=(BlueprintSpawnableComponent))
class ASHLINECHARACTERS_API UALCombatComponent : public UActorComponent
{ GENERATED_BODY() public: UFUNCTION(BlueprintCallable) bool FireCurrentWeapon(); UFUNCTION(BlueprintCallable) void ReloadCurrentWeapon(); UPROPERTY(BlueprintReadOnly) TObjectPtr<class AALWeaponBase> CurrentWeapon; UFUNCTION(BlueprintCallable) void SetCurrentWeapon(AALWeaponBase* Weapon) { CurrentWeapon = Weapon; } };
