#include "ALAssetManager.h"
#include "Engine/StreamableManager.h"

UALAssetManager& UALAssetManager::Get()
{
    UALAssetManager& Manager = static_cast<UALAssetManager&>(UAssetManager::Get());
    return Manager;
}

void UALAssetManager::StartInitialLoading()
{
    Super::StartInitialLoading();
}
