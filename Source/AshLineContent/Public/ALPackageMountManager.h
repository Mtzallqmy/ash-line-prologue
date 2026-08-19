#pragma once
#include "CoreMinimal.h"
#include "ALContentManifest.h"
#include "ALPackageMountManager.generated.h"

class UALDirectoryPackageMountBackend;

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
    UPROPERTY() TObjectPtr<UALDirectoryPackageMountBackend> DirectoryBackend;
    IALPackageMountBackend* Backend = nullptr;
};

UCLASS()
class ASHLINECONTENT_API UALDirectoryPackageMountBackend : public UObject, public IALPackageMountBackend
{
    GENERATED_BODY()
public:
    virtual bool Mount(const FALContentPackage& Package, const FString& PackagePath, FString& OutMessage) override;
    virtual bool Unmount(const FALContentPackage& Package, const FString& PackagePath, FString& OutMessage) override;
};
