#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Components/ALHealthComponent.h"
#include "Damage/ALDamageData.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FALHealthComponentInvariantTest, "ASHLINE.Combat.HealthComponent.Invariants", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FALHealthComponentInvariantTest::RunTest(const FString& Parameters)
{
    UALHealthComponent* Health = NewObject<UALHealthComponent>();
    Health->MaxHealth = 100.0f;
    Health->CurrentHealth = 100.0f;
    Health->bIsDead = false;
    Health->bInvulnerable = false;

    FALDamageData Damage;
    Damage.BaseDamage = 25.0f;
    TestEqual(TEXT("100 damage 25 leaves 75"), Health->ApplyDamage(Damage), 25.0f);
    TestEqual(TEXT("Health after 25 damage"), Health->GetCurrentHealth(), 75.0f);

    Damage.BaseDamage = 100.0f;
    TestEqual(TEXT("Lethal damage is clamped to remaining health"), Health->ApplyDamage(Damage), 75.0f);
    TestTrue(TEXT("Health enters dead state once at zero"), Health->IsDead());
    TestEqual(TEXT("Damage after death is ignored"), Health->ApplyDamage(Damage), 0.0f);
    TestEqual(TEXT("Healing after death is ignored"), Health->ApplyHealing(25.0f), 0.0f);

    Health->ResetHealth();
    TestTrue(TEXT("Reset restores alive state"), Health->IsAlive());
    TestEqual(TEXT("Reset restores full health"), Health->GetCurrentHealth(), 100.0f);

    Damage.BaseDamage = 25.0f;
    Health->ApplyDamage(Damage);
    TestEqual(TEXT("Healing restores health after damage"), Health->ApplyHealing(10.0f), 10.0f);
    TestEqual(TEXT("Healing result is clamped and observable"), Health->GetCurrentHealth(), 85.0f);

    Health->SetInvulnerable(true);
    TestEqual(TEXT("Invulnerability blocks damage"), Health->ApplyDamage(Damage), 0.0f);
    Health->SetInvulnerable(false);
    return true;
}

#endif
