#include "ALCombatPrototypeHUD.h"

#include "ALCombatPrototypeGameMode.h"
#include "ALMissionSubsystem.h"
#include "ALPlayerCharacter.h"
#include "Components/ALHealthComponent.h"
#include "Components/ALWeaponComponent.h"
#include "ALWeaponBase.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

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

    if (UALMissionSubsystem* MissionSubsystem = GetWorld() ? GetWorld()->GetSubsystem<UALMissionSubsystem>() : nullptr)
    {
        MissionSubsystem->OnMissionStarted.AddUniqueDynamic(this, &AALCombatPrototypeHUD::HandleMissionStarted);
        MissionSubsystem->OnMissionCompleted.AddUniqueDynamic(this, &AALCombatPrototypeHUD::HandleMissionCompleted);
        MissionSubsystem->OnObjectiveActivated.AddUniqueDynamic(this, &AALCombatPrototypeHUD::HandleObjectiveActivated);
        MissionSubsystem->OnObjectiveProgressChanged.AddUniqueDynamic(this, &AALCombatPrototypeHUD::HandleObjectiveProgressChanged);

        if (MissionSubsystem->HasActiveMission()) HandleMissionStarted(MissionSubsystem->ActiveMissionId);
        if (MissionSubsystem->HasActiveObjective())
        {
            HandleObjectiveActivated(MissionSubsystem->ActiveMissionId, MissionSubsystem->ActiveObjectiveIndex);
            HandleObjectiveProgressChanged(MissionSubsystem->ActiveMissionId, MissionSubsystem->ActiveObjectiveIndex, MissionSubsystem->ActiveObjectiveProgress);
        }
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

void AALCombatPrototypeHUD::HandleMissionStarted(FName MissionId)
{
    SetMissionDisplay(MissionId);
}

void AALCombatPrototypeHUD::HandleMissionCompleted(FName MissionId)
{
    SetMissionCompleteDisplay(MissionId);
}

void AALCombatPrototypeHUD::HandleObjectiveActivated(FName MissionId, int32 ObjectiveIndex)
{
    SetMissionDisplay(MissionId);
    SetObjectiveDisplay(ObjectiveIndex);
}

void AALCombatPrototypeHUD::HandleObjectiveProgressChanged(FName MissionId, int32 ObjectiveIndex, float Progress)
{
    SetMissionDisplay(MissionId);
    SetObjectiveDisplay(ObjectiveIndex);
    SetObjectiveProgressDisplay(Progress);
}

void AALCombatPrototypeHUD::HandlePrototypeCompleted()
{
    SetPrototypeCompleteDisplay();
}
