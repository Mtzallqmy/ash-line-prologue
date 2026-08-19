#pragma once
#include "CoreMinimal.h"
#include "ALContentManifest.generated.h"

UENUM(BlueprintType)
enum class EALPackageState : uint8
{
    Unknown,
    NotInstalled,
    Queued,
    Downloading,
    Downloaded,
    Verifying,
    Installing,
    Installed,
    Mounting,
    Mounted,
    Updating,
    Corrupted,
    Incompatible,
    Failed
};

UENUM(BlueprintType)
enum class EALPackageType : uint8
{
    Core,
    City,
    District,
    Mission,
    WeaponPack,
    VehiclePack,
    DronePack,
    AudioPack,
    VoicePack,
    Localization,
    Patch
};

UENUM(BlueprintType)
enum class EALContentError : uint8
{
    None,
    FileNotFound,
    InvalidManifest,
    UnsupportedSchema,
    InvalidSignature,
    HashMismatch,
    IncompatibleVersion,
    MissingDependency,
    CircularDependency,
    InsufficientStorage,
    MountFailed,
    CopyFailed,
    NetworkFailure,
    Cancelled,
    PathTraversal,
    SizeLimitExceeded,
    Busy
};

USTRUCT(BlueprintType)
struct ASHLINECONTENT_API FALContentPackage
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Package") FString PackageId;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Package") EALPackageType PackageType = EALPackageType::Core;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Package") int32 Version = 1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Package") FString MinGameVersion = TEXT("0.1.0");
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Package") FString MaxGameVersion;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Package") int64 DownloadSizeBytes = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Package") int64 InstalledSizeBytes = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Package") int32 MountPriority = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Package") FString SHA256;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Package") FString Signature;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Package") FString PublicKeyId;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Package") FString ContentRoot;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Package") TArray<FString> EntryAssets;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Package") TArray<FString> Dependencies;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Package") TArray<FString> OptionalDependencies;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Package") bool bRequired = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Package") FString Url;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Package") FString AssetDeliveryIdentifier;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Package|Patch") int32 PatchFromVersion = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Package|Patch") int64 PatchSizeBytes = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Package|Runtime") FString InstalledPath;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Package|Runtime") EALPackageState State = EALPackageState::NotInstalled;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Package|Runtime") bool bMounted = false;
};

USTRUCT(BlueprintType)
struct ASHLINECONTENT_API FALContentManifest
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Manifest") int32 SchemaVersion = 1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Manifest") FString GameVersion = TEXT("0.1.0");
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Manifest") int32 ContentVersion = 1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Manifest") FString ManifestSignature;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Manifest") TArray<FALContentPackage> Packages;
};
