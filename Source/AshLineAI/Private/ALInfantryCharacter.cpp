#include "ALInfantryCharacter.h"
#include "Components/ALHealthComponent.h"
#include "Components/ALWeaponComponent.h"
#include "ALWeaponBase.h"
#include "AI/ALAISenseComponent.h"
#include "AI/ALAICombatComponent.h"
#include "AI/ALAICoverComponent.h"
#include "AI/ALAISignificanceComponent.h"
#include "Data/ALAIArchetypeData.h"
#include "ALAIController.h"
#include "World/ALPatrolRoute.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"

AALInfantryCharacter::AALInfantryCharacter()
{
    HealthComponent = CreateDefaultSubobject<UALHealthComponent>(TEXT("HealthComponent"));
    WeaponComponent = CreateDefaultSubobject<UALWeaponComponent>(TEXT("WeaponComponent"));
    AIStateComponent = CreateDefaultSubobject<UALAIStateComponent>(TEXT("AIStateComponent"));
    SenseComponent = CreateDefaultSubobject<UALAISenseComponent>(TEXT("SenseComponent"));
    CombatComponent = CreateDefaultSubobject<UALAICombatComponent>(TEXT("CombatComponent"));
    CoverComponent = CreateDefaultSubobject<UALAICoverComponent>(TEXT("CoverComponent"));
    SignificanceComponent = CreateDefaultSubobject<UALAISignificanceComponent>(TEXT("SignificanceComponent"));
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    AIControllerClass = AALAIController::StaticClass();
}

void AALInfantryCharacter::BeginPlay()
{
    Super::BeginPlay();
    if (HealthComponent)
    {
        HealthComponent->OnDamageReceived.AddDynamic(this, &AALInfantryCharacter::HandleHealthDamage);
        HealthComponent->OnDeath.AddDynamic(this, &AALInfantryCharacter::HandleHealthDeath);
        if (Archetype)
        {
            if (Archetype->HealthConfig) HealthComponent->HealthConfig = Archetype->HealthConfig;
            if (Archetype->HealthConfig) HealthComponent->SetMaxHealth(Archetype->HealthConfig->MaxHealth);
        }
    }
    if (CombatComponent && Archetype) CombatComponent->ConfigureFromArchetype(Archetype);
    if (Archetype && Archetype->SquadId != NAME_None && SquadId == NAME_None) SquadId = Archetype->SquadId;
    if (WeaponComponent && WeaponClass && Archetype && !Archetype->WeaponData.IsNull())
    {
        UALWeaponDataAsset* WeaponData = Archetype->WeaponData.LoadSynchronous();
        if (WeaponData)
        {
            SpawnedWeapon = GetWorld()->SpawnActorDeferred<AALWeaponBase>(WeaponClass, GetActorTransform(), this, this, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
            if (SpawnedWeapon)
            {
                SpawnedWeapon->Data = WeaponData;
                SpawnedWeapon->FinishSpawning(GetActorTransform());
                SpawnedWeapon->InitializeWeapon(this);
                WeaponComponent->EquipWeapon(SpawnedWeapon, true);
            }
        }
    }
}

void AALInfantryCharacter::HandleHealthDamage(FALDamageData DamageData)
{
    if (bDeathHandled || !SenseComponent) return;
    SenseComponent->ProcessDamageAwareness(DamageData);
    if (AIStateComponent) AIStateComponent->SetState(EALAIState::Alert);
}

void AALInfantryCharacter::HandleHealthDeath()
{
    if (bDeathHandled) return;
    bDeathHandled = true;
    if (AIStateComponent) AIStateComponent->SetState(EALAIState::Dead);
    if (CombatComponent) CombatComponent->StopCombat();
    if (CoverComponent) CoverComponent->ReleaseCover();
    if (WeaponComponent) WeaponComponent->SetWeaponInputEnabled(false);
    GetCharacterMovement()->DisableMovement();
    SetActorEnableCollision(false);
    OnEnemyKilled.Broadcast(this);
    if (CorpseLifetime > 0.0f) SetLifeSpan(CorpseLifetime);
}
