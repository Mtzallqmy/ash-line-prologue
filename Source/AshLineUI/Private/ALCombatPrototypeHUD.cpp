#include "ALCombatPrototypeHUD.h"

#include "ALCombatPrototypeGameMode.h"
#include "ALPlayerCharacter.h"
#include "Components/ALHealthComponent.h"
#include "Components/ALWeaponComponent.h"
#include "ALWeaponBase.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"

void AALCombatPrototypeHUD::BeginPlay()
{
    Super::BeginPlay();

    if (CombatWidgetClass && GetOwningPlayerController())
    {
        CombatWidget = CreateWidget<UUserWidget>(GetOwningPlayerController(), CombatWidgetClass);
        if (CombatWidget) CombatWidget->AddToViewport(0);
    }

    SetBuildLabelDisplay(BuildLabel);
    AALPlayerCharacter* Player = GetOwningPlayerController() ? Cast<AALPlayerCharacter>(GetOwningPlayerController()->GetPawn()) : nullptr;
    if (Player)
    {
        if (Player->HealthComponent)
        {
            Player->HealthComponent->OnHealthChanged.AddUniqueDynamic(this, &AALCombatPrototypeHUD::HandleHealthChanged);
            HandleHealthChanged(Player->HealthComponent->GetCurrentHealth(), Player->HealthComponent->GetCurrentHealth(), Player->HealthComponent->GetMaxHealth(), 0.0f);
        }
        if (Player->WeaponComponent)
        {
            Player->WeaponComponent->OnAmmoChanged.AddUniqueDynamic(this, &AALCombatPrototypeHUD::HandleAmmoChanged);
            if (Player->WeaponComponent->GetCurrentWeapon())
            {
                HandleAmmoChanged(Player->WeaponComponent->GetCurrentWeapon()->GetAmmoInMagazine(), Player->WeaponComponent->GetCurrentWeapon()->GetReserveAmmo());
            }
        }
    }

    if (AALCombatPrototypeGameMode* GameMode = GetWorld() ? GetWorld()->GetAuthGameMode<AALCombatPrototypeGameMode>() : nullptr)
    {
        GameMode->OnEnemyCountChanged.AddUniqueDynamic(this, &AALCombatPrototypeHUD::HandleEnemyCountChanged);
        GameMode->OnPrototypeCompleted.AddUniqueDynamic(this, &AALCombatPrototypeHUD::HandlePrototypeCompleted);
        HandleEnemyCountChanged(GameMode->GetDefeatedEnemyCount(), GameMode->GetRemainingEnemyCount());
        if (GameMode->IsPrototypeComplete()) HandlePrototypeCompleted();
    }
}

void AALCombatPrototypeHUD::HandleHealthChanged(float OldHealth, float NewHealth, float MaxHealth, float Delta)
{
    SetHealthDisplay(NewHealth, MaxHealth, Delta);
}

void AALCombatPrototypeHUD::HandleAmmoChanged(int32 MagazineAmmo, int32 ReserveAmmo)
{
    SetAmmoDisplay(MagazineAmmo, ReserveAmmo);
}

void AALCombatPrototypeHUD::HandleEnemyCountChanged(int32 DefeatedCount, int32 RemainingCount)
{
    SetEnemyCountDisplay(DefeatedCount, RemainingCount);
}

void AALCombatPrototypeHUD::HandlePrototypeCompleted()
{
    SetPrototypeCompleteDisplay();
}
