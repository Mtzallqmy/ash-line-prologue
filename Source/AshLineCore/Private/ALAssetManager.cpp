#include "ALAssetManager.h"
#include "Engine/StreamableManager.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Modules/ModuleManager.h"

UALAssetManager& UALAssetManager::Get()
{
    UALAssetManager& Manager = static_cast<UALAssetManager&>(UAssetManager::Get());
    return Manager;
}

void UALAssetManager::StartInitialLoading()
{
    Super::StartInitialLoading();
}

bool UALAssetManager::RegisterMountedPackageRoot(const FString& RootPath)
{
    if (RootPath.IsEmpty() || MountedPackageRoots.Contains(RootPath)) return true;
    FAssetRegistryModule& RegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
    RegistryModule.Get().ScanPathsSynchronous({RootPath}, true);
    MountedPackageRoots.Add(RootPath);
    return true;
}

bool UALAssetManager::UnregisterMountedPackageRoot(const FString& RootPath)
{
    return MountedPackageRoots.Remove(RootPath) > 0;
}
