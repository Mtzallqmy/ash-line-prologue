#include "Components/ALRecoilComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "Engine/World.h"
#include "TimerManager.h"

UALRecoilComponent::UALRecoilComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UALRecoilComponent::ApplyRecoil(const FALRecoilSettings& Settings, bool bMobileProfile)
{
    const float Scale = bMobileProfile ? Settings.MobileRecoilScale : 1.0f;
    const float Pitch = FMath::FRandRange(Settings.PitchMin, Settings.PitchMax) * Scale;
    const float Yaw = FMath::FRandRange(Settings.YawMin, Settings.YawMax) * Scale;
    CurrentPitch += Pitch;
    CurrentYaw += Yaw;

    if (APawn* Pawn = Cast<APawn>(GetOwner()))
    {
        if (AController* Controller = Pawn->GetController())
        {
            Controller->AddPitchInput(-Pitch);
            Controller->AddYawInput(Yaw);
        }
    }

    if (GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimer(RecoveryTimer, this, &UALRecoilComponent::RecoverRecoil, 0.016f, true);
    }
}

void UALRecoilComponent::RecoverRecoil()
{
    const float Recovery = 0.08f;
    const float PitchDelta = FMath::Min(FMath::Abs(CurrentPitch), Recovery) * FMath::Sign(CurrentPitch);
    const float YawDelta = FMath::Min(FMath::Abs(CurrentYaw), Recovery) * FMath::Sign(CurrentYaw);
    CurrentPitch -= PitchDelta;
    CurrentYaw -= YawDelta;

    if (APawn* Pawn = Cast<APawn>(GetOwner()))
    {
        if (AController* Controller = Pawn->GetController())
        {
            Controller->AddPitchInput(PitchDelta);
            Controller->AddYawInput(-YawDelta);
        }
    }

    if (FMath::IsNearlyZero(CurrentPitch, 0.01f) && FMath::IsNearlyZero(CurrentYaw, 0.01f)) ResetRecoil();
}

void UALRecoilComponent::ResetRecoil()
{
    CurrentPitch = 0.0f;
    CurrentYaw = 0.0f;
    if (GetWorld()) GetWorld()->GetTimerManager().ClearTimer(RecoveryTimer);
}
