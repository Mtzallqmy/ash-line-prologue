#include "ALHealthComponent.h"
UALHealthComponent::UALHealthComponent() { PrimaryComponentTick.bCanEverTick = false; }
void UALHealthComponent::RestoreFullHealth() { const float Delta = MaxHealth - CurrentHealth; CurrentHealth = MaxHealth; OnHealthChanged.Broadcast(CurrentHealth, Delta); }
bool UALHealthComponent::ReceiveALDamage_Implementation(const FALDamageEvent& Event) { const float Old = CurrentHealth; CurrentHealth = FMath::Clamp(CurrentHealth - FMath::Max(Event.Amount, 0.0f), 0.0f, MaxHealth); OnHealthChanged.Broadcast(CurrentHealth, CurrentHealth - Old); return CurrentHealth != Old; }
