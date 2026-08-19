#include "Diagnostics/ALAIDebugLibrary.h"
#include "ALAIController.h"
#include "ALInfantryCharacter.h"
#include "AI/ALAISenseComponent.h"
#include "AI/ALAICoverComponent.h"
#include "Components/ALHealthComponent.h"
#include "Components/ALWeaponComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Controller.h"
#include "Engine/World.h"

void UALAIDebugLibrary::SetAIFrozen(AActor* Actor, bool bFrozen)
{
#if !UE_BUILD_SHIPPING
    if (AALInfantryCharacter* Enemy = Cast<AALInfantryCharacter>(Actor))
    {
        if (AALAIController* Controller = Cast<AALAIController>(Enemy->GetController())) Controller->SetDebugFrozen(bFrozen);
    }
#endif
}

void UALAIDebugLibrary::DrawAIDebug(AActor* Actor, float Duration)
{
#if !UE_BUILD_SHIPPING
    AALInfantryCharacter* Enemy = Cast<AALInfantryCharacter>(Actor);
    if (!Enemy || !Enemy->GetWorld()) return;
    const FString Label = FString::Printf(TEXT("State: %s | Awareness: %.2f | HP: %.0f/%.0f | Weapon: %s"), *GetAIStateLabel(Actor), Enemy->SenseComponent ? Enemy->SenseComponent->GetAwareness() : 0.0f, Enemy->HealthComponent ? Enemy->HealthComponent->GetCurrentHealth() : 0.0f, Enemy->HealthComponent ? Enemy->HealthComponent->GetMaxHealth() : 0.0f, Enemy->WeaponComponent && Enemy->WeaponComponent->GetCurrentWeapon() ? *Enemy->WeaponComponent->GetCurrentWeapon()->GetWeaponId().ToString() : TEXT("None"));
    DrawDebugString(Enemy->GetWorld(), Enemy->GetActorLocation() + FVector(0.0f, 0.0f, 120.0f), Label, nullptr, FColor::Yellow, Duration, true);
    if (Enemy->SenseComponent && Enemy->SenseComponent->GetCurrentTarget()) DrawDebugLine(Enemy->GetActorLocation(), Enemy->SenseComponent->GetCurrentTarget()->GetActorLocation(), FColor::Red, false, Duration, 0, 2.0f);
    if (Enemy->SenseComponent) DrawDebugSphere(Enemy->GetWorld(), Enemy->SenseComponent->GetLastKnownLocation(), 35.0f, 8, FColor::Orange, false, Duration);
    if (Enemy->CoverComponent && Enemy->CoverComponent->GetCurrentCover()) DrawDebugSphere(Enemy->GetWorld(), Enemy->CoverComponent->GetCurrentCover()->GetActorLocation(), 60.0f, 8, FColor::Green, false, Duration);
#endif
}

FString UALAIDebugLibrary::GetAIStateLabel(AActor* Actor)
{
    const AALInfantryCharacter* Enemy = Cast<AALInfantryCharacter>(Actor);
    if (!Enemy || !Enemy->AIStateComponent) return TEXT("Unknown");
    const UEnum* Enum = StaticEnum<EALAIState>();
    return Enum ? Enum->GetNameStringByValue(static_cast<int64>(Enemy->AIStateComponent->GetState())) : TEXT("Unknown");
}
