#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ALContentManifest.h"
#include "ALPackageRegistry.h"
#include "ALContentManagerSubsystem.generated.h"

class UALPackageValidator;
class UALPackageMountManager;
class UALLocalContentDeliveryService;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FALContentStateChanged, FString, PackageId, EALPackageState, State);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FALContentProgressEvent, FString, PackageId, float, Progress);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FALContentErrorEvent, FString, PackageId, EALContentError, Error, FString, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FALContentPackageEvent, FString, PackageId);

UCLASS()
class ASHLINECONTENT_API UALContentManagerSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable, Category="ASH LINE|Content") bool InitializeContentSystem();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Content") bool RefreshInstalledPackages();
    UFUNCTION(BlueprintPure, Category="ASH LINE|Content") bool IsPackageInstalled(const FString& PackageId) const;
    UFUNCTION(BlueprintPure, Category="ASH LINE|Content") bool IsPackageMounted(const FString& PackageId) const;
    UFUNCTION(BlueprintPure, Category="ASH LINE|Content") EALPackageState GetPackageState(const FString& PackageId) const;
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Content") bool RequestPackage(const FString& PackageId);
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Content") bool ImportPackage(const FString& FileReference);
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Content") bool InstallPackage(const FString& PackageId);
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Content") bool MountPackage(const FString& PackageId);
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Content") bool UnmountPackage(const FString& PackageId);
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Content") bool RemovePackage(const FString& PackageId);
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Content") bool UpdatePackage(const FString& PackageId);
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Content") bool VerifyPackage(const FString& PackageId);
    UFUNCTION(BlueprintPure, Category="ASH LINE|Content") float GetDownloadProgress(const FString& PackageId) const;
    UFUNCTION(BlueprintPure, Category="ASH LINE|Content") TArray<FString> GetRequiredDependencies(const FString& PackageId) const;
    UFUNCTION(BlueprintPure, Category="ASH LINE|Content") int64 GetInstalledContentSize() const;
    UFUNCTION(BlueprintPure, Category="ASH LINE|Content") int64 GetPackageInstalledSize(const FString& PackageId) const;
    UFUNCTION(BlueprintPure, Category="ASH LINE|Content") int64 GetFreeStorage() const;
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Content") void ClearTempFiles();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Content") void ClearFailedDownloads();
    UFUNCTION(BlueprintCallable, Category="ASH LINE|Content") void ClearUnusedCache();
    UFUNCTION(BlueprintPure, Category="ASH LINE|Content") FString GetInstalledRoot() const { return InstalledRoot; }
    UFUNCTION(BlueprintPure, Category="ASH LINE|Content") FString GetRegistryPath() const { return RegistryPath; }

    UPROPERTY(BlueprintAssignable, Category="ASH LINE|Content") FALContentStateChanged OnPackageStateChanged;
    UPROPERTY(BlueprintAssignable, Category="ASH LINE|Content") FALContentProgressEvent OnPackageProgress;
    UPROPERTY(BlueprintAssignable, Category="ASH LINE|Content") FALContentPackageEvent OnPackageInstalled;
    UPROPERTY(BlueprintAssignable, Category="ASH LINE|Content") FALContentPackageEvent OnPackageMounted;
    UPROPERTY(BlueprintAssignable, Category="ASH LINE|Content") FALContentPackageEvent OnPackageRemoved;
    UPROPERTY(BlueprintAssignable, Category="ASH LINE|Content") FALContentErrorEvent OnPackageError;

protected:
    bool LoadGlobalManifest();
    bool ParsePackageManifest(const FString& ManifestPath, FALContentPackage& OutPackage) const;
    bool ResolvePackage(const FString& PackageId, FALContentPackage& OutPackage) const;
    bool SetPackageState(const FString& PackageId, EALPackageState State);
    bool FailPackage(const FString& PackageId, EALContentError Error, const FString& Message);
    bool EnsureDependenciesInstalled(const FALContentPackage& Package);
    FString GetPackageRoot(const FString& PackageId) const;
    FString GetPackageManifestPath(const FString& PackageId) const;

    UPROPERTY() TObjectPtr<UALPackageRegistry> Registry;
    UPROPERTY() TObjectPtr<UALPackageValidator> Validator;
    UPROPERTY() TObjectPtr<UALPackageMountManager> MountManager;
    UPROPERTY() TObjectPtr<UALLocalContentDeliveryService> LocalService;
    UPROPERTY() FALContentManifest GlobalManifest;
    FString ContentRoot;
    FString InstalledRoot;
    FString DownloadsRoot;
    FString TempRoot;
    FString PatchesRoot;
    FString ManifestRoot;
    FString RegistryPath;
    bool bInitialized = false;
    bool bAllowUnsignedDevelopmentPackages = true;
};
