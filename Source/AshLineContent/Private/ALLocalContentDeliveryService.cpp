#include "ALLocalContentDeliveryService.h"
#include "HAL/FileManager.h"
#include "Misc/Paths.h"

bool UALLocalContentDeliveryService::FetchManifest_Implementation(FALContentManifest& OutManifest) { OutManifest = Manifest; return Manifest.Packages.Num() > 0; }
bool UALLocalContentDeliveryService::CheckManifest_Implementation(FALContentManifest& OutManifest) { return FetchManifest_Implementation(OutManifest); }
TArray<FString> UALLocalContentDeliveryService::GetInstalledPackages_Implementation() { return InstalledPackages.Array(); }
bool UALLocalContentDeliveryService::IsPackageInstalled_Implementation(const FString& PackageId) { return InstalledPackages.Contains(PackageId); }
bool UALLocalContentDeliveryService::RequestPackage_Implementation(const FString& PackageId)
{
    for (const FALContentPackage& Package : Manifest.Packages) if (Package.PackageId == PackageId) { PackageStates.FindOrAdd(PackageId) = EALPackageState::Downloaded; return true; }
    return false;
}
bool UALLocalContentDeliveryService::ImportPackage_Implementation(const FString& FileReference)
{
    return ImportDevelopmentPackageDirectory(FileReference);
}

bool UALLocalContentDeliveryService::ImportDevelopmentPackageDirectory(const FString& DirectoryReference)
{
    if (!FPaths::DirectoryExists(DirectoryReference)) return false;
    return true;
}

bool UALLocalContentDeliveryService::ImportPackageFile(const FString& FileReference)
{
    return false;
}
bool UALLocalContentDeliveryService::PauseDownload_Implementation(const FString& PackageId) { return PackageStates.Contains(PackageId) && PackageStates[PackageId] == EALPackageState::Downloading; }
bool UALLocalContentDeliveryService::ResumeDownload_Implementation(const FString& PackageId) { return PackageStates.Contains(PackageId); }
bool UALLocalContentDeliveryService::CancelDownload_Implementation(const FString& PackageId) { PackageStates.Remove(PackageId); return true; }
float UALLocalContentDeliveryService::GetDownloadProgress_Implementation(const FString& PackageId) { return IsPackageInstalled_Implementation(PackageId) ? 1.0f : 0.0f; }
bool UALLocalContentDeliveryService::MountPackage_Implementation(const FString& PackageId) { if (!IsPackageInstalled_Implementation(PackageId)) return false; PackageStates.FindOrAdd(PackageId) = EALPackageState::Mounted; return true; }
bool UALLocalContentDeliveryService::UnmountPackage_Implementation(const FString& PackageId) { if (!IsPackageInstalled_Implementation(PackageId)) return false; PackageStates.FindOrAdd(PackageId) = EALPackageState::Installed; return true; }
bool UALLocalContentDeliveryService::VerifyPackage_Implementation(const FString& PackageId) { return IsPackageInstalled_Implementation(PackageId); }
FString UALLocalContentDeliveryService::GetPackageFilePath_Implementation(const FString& PackageId) { return FPaths::Combine(LocalRoot, PackageId); }
