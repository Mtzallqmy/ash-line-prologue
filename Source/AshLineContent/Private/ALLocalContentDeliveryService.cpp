#include "ALLocalContentDeliveryService.h"
bool UALLocalContentDeliveryService::CheckManifest_Implementation(FALContentManifest& OutManifest) { OutManifest = Manifest; return Manifest.Packages.Num() > 0; }
TArray<FString> UALLocalContentDeliveryService::GetInstalledPackages_Implementation() { return InstalledPackages.Array(); }
bool UALLocalContentDeliveryService::IsPackageInstalled_Implementation(const FString& PackageId) { return InstalledPackages.Contains(PackageId); }
bool UALLocalContentDeliveryService::RequestPackage_Implementation(const FString& PackageId) { InstalledPackages.Add(PackageId); return true; }
float UALLocalContentDeliveryService::GetDownloadProgress_Implementation(const FString& PackageId) { return InstalledPackages.Contains(PackageId) ? 1.0f : 0.0f; }
bool UALLocalContentDeliveryService::MountPackage_Implementation(const FString& PackageId) { return InstalledPackages.Contains(PackageId); }
bool UALLocalContentDeliveryService::UnmountPackage_Implementation(const FString& PackageId) { return InstalledPackages.Contains(PackageId); }
bool UALLocalContentDeliveryService::VerifyPackage_Implementation(const FString& PackageId) { return InstalledPackages.Contains(PackageId); }
