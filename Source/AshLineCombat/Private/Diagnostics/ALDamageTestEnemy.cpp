#include "Diagnostics/ALDamageTestEnemy.h"
#include "Components/ALHealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AALDamageTestEnemy::AALDamageTestEnemy()
{
    HealthComponent = CreateDefaultSubobject<UALHealthComponent>(TEXT("HealthComponent"));
}

void AALDamageTestEnemy::BeginPlay()
{
    Super::BeginPlay();
    if (HealthComponent) HealthComponent->OnDeath.AddDynamic(this, &AALDamageTestEnemy::HandleDeath);
}

void AALDamageTestEnemy::HandleDeath()
{
    if (bDeathHandled) return;
    bDeathHandled = true;
    GetCharacterMovement()->DisableMovement();
    SetActorEnableCollision(false);
    if (CorpseLifetime > 0.0f) SetLifeSpan(CorpseLifetime);
}
