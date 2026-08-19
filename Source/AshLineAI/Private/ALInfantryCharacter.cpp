#include "ALInfantryCharacter.h"
#include "ALHealthComponent.h"
AALInfantryCharacter::AALInfantryCharacter() { HealthComponent = CreateDefaultSubobject<UALHealthComponent>(TEXT("HealthComponent")); }
