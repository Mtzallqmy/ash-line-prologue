#include "ALGameInstance.h"
#include "ALSaveGame.h"
#include "Kismet/GameplayStatics.h"

bool UALGameInstance::SaveProgress()
{
    if (!CurrentSave) CurrentSave = Cast<UALSaveGame>(UGameplayStatics::CreateSaveGameObject(UALSaveGame::StaticClass()));
    return UGameplayStatics::SaveGameToSlot(CurrentSave, TEXT("ASHLINE_PROFILE_0"), 0);
}

bool UALGameInstance::LoadProgress()
{
    CurrentSave = Cast<UALSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("ASHLINE_PROFILE_0"), 0));
    if (!CurrentSave) CurrentSave = Cast<UALSaveGame>(UGameplayStatics::CreateSaveGameObject(UALSaveGame::StaticClass()));
    return CurrentSave != nullptr;
}

void UALGameInstance::SetGraphicsProfile(int32 ProfileIndex)
{
    if (!CurrentSave) LoadProgress();
    if (CurrentSave) CurrentSave->Settings.Add(TEXT("GraphicsProfile"), FString::FromInt(FMath::Clamp(ProfileIndex, 0, 2)));
}
