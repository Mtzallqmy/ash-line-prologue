#include "ALRemoteContentServices.h"

#define AL_REMOTE_SERVICE_UNAVAILABLE() return false

bool UALHttpContentService::FetchManifest_Implementation(FALContentManifest&) { AL_REMOTE_SERVICE_UNAVAILABLE(); }
bool UALHttpContentService::CheckManifest_Implementation(FALContentManifest&) { AL_REMOTE_SERVICE_UNAVAILABLE(); }
TArray<FString> UALHttpContentService::GetInstalledPackages_Implementation() { return {}; }
bool UALHttpContentService::IsPackageInstalled_Implementation(const FString&) { return false; }
bool UALHttpContentService::RequestPackage_Implementation(const FString&) { AL_REMOTE_SERVICE_UNAVAILABLE(); }
bool UALHttpContentService::ImportPackage_Implementation(const FString&) { AL_REMOTE_SERVICE_UNAVAILABLE(); }
bool UALHttpContentService::PauseDownload_Implementation(const FString&) { AL_REMOTE_SERVICE_UNAVAILABLE(); }
bool UALHttpContentService::ResumeDownload_Implementation(const FString&) { AL_REMOTE_SERVICE_UNAVAILABLE(); }
bool UALHttpContentService::CancelDownload_Implementation(const FString&) { AL_REMOTE_SERVICE_UNAVAILABLE(); }
float UALHttpContentService::GetDownloadProgress_Implementation(const FString&) { return 0.0f; }
bool UALHttpContentService::MountPackage_Implementation(const FString&) { AL_REMOTE_SERVICE_UNAVAILABLE(); }
bool UALHttpContentService::UnmountPackage_Implementation(const FString&) { AL_REMOTE_SERVICE_UNAVAILABLE(); }
bool UALHttpContentService::VerifyPackage_Implementation(const FString&) { AL_REMOTE_SERVICE_UNAVAILABLE(); }
FString UALHttpContentService::GetPackageFilePath_Implementation(const FString&) { return FString(); }

bool UALPlayAssetDeliveryService::FetchManifest_Implementation(FALContentManifest&) { AL_REMOTE_SERVICE_UNAVAILABLE(); }
bool UALPlayAssetDeliveryService::CheckManifest_Implementation(FALContentManifest&) { AL_REMOTE_SERVICE_UNAVAILABLE(); }
TArray<FString> UALPlayAssetDeliveryService::GetInstalledPackages_Implementation() { return {}; }
bool UALPlayAssetDeliveryService::IsPackageInstalled_Implementation(const FString&) { return false; }
bool UALPlayAssetDeliveryService::RequestPackage_Implementation(const FString&) { AL_REMOTE_SERVICE_UNAVAILABLE(); }
bool UALPlayAssetDeliveryService::ImportPackage_Implementation(const FString&) { AL_REMOTE_SERVICE_UNAVAILABLE(); }
bool UALPlayAssetDeliveryService::PauseDownload_Implementation(const FString&) { AL_REMOTE_SERVICE_UNAVAILABLE(); }
bool UALPlayAssetDeliveryService::ResumeDownload_Implementation(const FString&) { AL_REMOTE_SERVICE_UNAVAILABLE(); }
bool UALPlayAssetDeliveryService::CancelDownload_Implementation(const FString&) { AL_REMOTE_SERVICE_UNAVAILABLE(); }
float UALPlayAssetDeliveryService::GetDownloadProgress_Implementation(const FString&) { return 0.0f; }
bool UALPlayAssetDeliveryService::MountPackage_Implementation(const FString&) { AL_REMOTE_SERVICE_UNAVAILABLE(); }
bool UALPlayAssetDeliveryService::UnmountPackage_Implementation(const FString&) { AL_REMOTE_SERVICE_UNAVAILABLE(); }
bool UALPlayAssetDeliveryService::VerifyPackage_Implementation(const FString&) { AL_REMOTE_SERVICE_UNAVAILABLE(); }
FString UALPlayAssetDeliveryService::GetPackageFilePath_Implementation(const FString&) { return FString(); }

#undef AL_REMOTE_SERVICE_UNAVAILABLE
