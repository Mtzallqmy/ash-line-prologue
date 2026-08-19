#include "ALContentPlatformBridge.h"
#include "Misc/Paths.h"

FString UALContentPlatformBridge::GetAppSpecificContentRoot()
{
    return FPaths::Combine(FPaths::ProjectPersistentDownloadDir(), TEXT("AshLine"));
}

bool UALContentPlatformBridge::OpenFilePickerForALPack()
{
#if PLATFORM_ANDROID
    // Android Storage Access Framework JNI bridge belongs here; no shared Content logic is placed in Kotlin.
    return false;
#else
    return false;
#endif
}

bool UALContentPlatformBridge::IsSupportedPackageFile(const FString& FileReference)
{
    return FileReference.EndsWith(TEXT(".alpack"), ESearchCase::IgnoreCase) || FPaths::DirectoryExists(FileReference);
}
