#include "ALPlayerCharacter.h"
#include "ALHealthComponent.h"
#include "ALCharacterComponents.h"
AALPlayerCharacter::AALPlayerCharacter()
{
    HealthComponent = CreateDefaultSubobject<UALHealthComponent>(TEXT("HealthComponent"));
    CombatComponent = CreateDefaultSubobject<UALCombatComponent>(TEXT("CombatComponent"));
    InventoryComponent = CreateDefaultSubobject<UALInventoryComponent>(TEXT("InventoryComponent"));
    EquipmentComponent = CreateDefaultSubobject<UALEquipmentComponent>(TEXT("EquipmentComponent"));
    InteractionComponent = CreateDefaultSubobject<UALInteractionComponent>(TEXT("InteractionComponent"));
    PlayerStateComponent = CreateDefaultSubobject<UALPlayerStateComponent>(TEXT("PlayerStateComponent"));
}
