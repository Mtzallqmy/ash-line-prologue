#include "ALPackageMountManager.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "HAL/FileManager.h"
#include "Misc/Paths.h"
#include "Misc/PackageName.h"
#include "Modules/ModuleManager.h"
#include "ALAssetManager.h"

bool UALDirectoryPackageMountBackend::Mount(const FALContentPackage& Package, const FString& PackagePath, FString& OutMessage)
{
    if (!FPaths::DirectoryExists(PackagePath)) { OutMessage = TEXT("Package directory does not exist."); return false; }
    if (Package.ContentRoot.IsEmpty()) { OutMessage = TEXT("Package content root is empty."); return false; }
    OutMessage = TEXT("Directory package mounted in development mode.");
    return true;
}

bool UALDirectoryPackageMountBackend::Unmount(const FALContentPackage& Package, const FString& PackagePath, FString& OutMessage)
{
    OutMessage = TEXT("Directory package unmounted.");
    return true;
}

bool UALPackageMountManager::MountPackage(const FALContentPackage& Package, const FString& PackagePath, FString& OutMessage)
{
    if (MountedPackages.Contains(Package.PackageId)) { OutMessage = TEXT("Package is already mounted."); return true; }
    if (!DirectoryBackend) DirectoryBackend = NewObject<UALDirectoryPackageMountBackend>(this);
    Backend = DirectoryBackend;
    if (!Backend || !Backend->Mount(Package, PackagePath, OutMessage)) return false;
    if (!RegisterAssetRoots(Package)) { OutMessage = TEXT("Failed to register package asset roots."); return false; }
    MountedPackages.Add(Package.PackageId, PackagePath);
    return true;
}

bool UALPackageMountManager::UnmountPackage(const FALContentPackage& Package, const FString& PackagePath, FString& OutMessage)
{
    if (!MountedPackages.Contains(Package.PackageId)) { OutMessage = TEXT("Package is not mounted."); return true; }
    if (!Backend || !Backend->Unmount(Package, PackagePath, OutMessage)) return false;
    MountedPackages.Remove(Package.PackageId);
    return true;
}

bool UALPackageMountManager::IsMounted(const FString& PackageId) const { return MountedPackages.Contains(PackageId); }
TArray<FString> UALPackageMountManager::GetMountedPackages() const { TArray<FString> Result; MountedPackages.GetKeys(Result); return Result; }

bool UALPackageMountManager::RegisterAssetRoots(const FALContentPackage& Package)
{
    for (const FString& Root : Package.EntryAssets)
    {
        if (!Root.IsEmpty() && Root.StartsWith(TEXT("/Game/"))) UALAssetManager::Get().RegisterMountedPackageRoot(Root);
    }
    return true;
}
