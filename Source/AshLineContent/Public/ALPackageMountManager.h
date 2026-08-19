#pragma once
#include "CoreMinimal.h"
#include "ALContentManifest.h"
#include "ALPackageMountManager.generated.h"

class UALDevelopmentDirectoryPackageMountBackend;

class IALPackageMountBackend
{
public:
    virtual ~IALPackageMountBackend() = default;
    virtual bool Mount(const FALContentPackage& Package, const FString& PackagePath, FString& OutMessage) = 0;
    virtual bool Unmount(const FALContentPackage& Package, const FString& PackagePath, FString& OutMessage) = 0;
};

UCLASS()
class ASHLINECONTENT_API UALPackageMountManager : public UObject
{
    GENERATED_BODY()
public:
    bool MountPackage(const FALContentPackage& Package, const FString& PackagePath, FString& OutMessage);
    bool UnmountPackage(const FALContentPackage& Package, const FString& PackagePath, FString& OutMessage);
    bool IsMounted(const FString& PackageId) const;
    TArray<FString> GetMountedPackages() const;
    bool RegisterAssetRoots(const FALContentPackage& Package);

protected:
    TMap<FString, FString> MountedPackages;
    UPROPERTY() TObjectPtr<UALDevelopmentDirectoryPackageMountBackend> DevelopmentDirectoryBackend;
    IALPackageMountBackend* Backend = nullptr;
};

// Development-only directory backend. Production Pak/IoStore backends must implement IALPackageMountBackend separately.
UCLASS()
class ASHLINECONTENT_API UALDevelopmentDirectoryPackageMountBackend : public UObject, public IALPackageMountBackend
{
    GENERATED_BODY()
public:
    virtual bool Mount(const FALContentPackage& Package, const FString& PackagePath, FString& OutMessage) override;
    virtual bool Unmount(const FALContentPackage& Package, const FString& PackagePath, FString& OutMessage) override;
};
