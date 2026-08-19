#include "ALContentManagerSubsystem.h"
#include "ALPackageValidator.h"
#include "ALPackageMountManager.h"
#include "ALLocalContentDeliveryService.h"
#include "HAL/FileManager.h"
#include "HAL/PlatformFileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonWriter.h"

DEFINE_LOG_CATEGORY_STATIC(LogAshLineContent, Log, All);

namespace ALContentManagerInternal
{
    static FString GetString(const TSharedPtr<FJsonObject>& Object, const TCHAR* Name) { FString Value; if (Object.IsValid()) Object->TryGetStringField(Name, Value); return Value; }
    static int32 GetInt(const TSharedPtr<FJsonObject>& Object, const TCHAR* Name, int32 DefaultValue) { double Value = DefaultValue; if (Object.IsValid()) Object->TryGetNumberField(Name, Value); return static_cast<int32>(Value); }
    static int64 GetInt64(const TSharedPtr<FJsonObject>& Object, const TCHAR* Name, int64 DefaultValue) { double Value = static_cast<double>(DefaultValue); if (Object.IsValid()) Object->TryGetNumberField(Name, Value); return static_cast<int64>(Value); }
    static bool GetBool(const TSharedPtr<FJsonObject>& Object, const TCHAR* Name, bool DefaultValue) { bool Value = DefaultValue; if (Object.IsValid()) Object->TryGetBoolField(Name, Value); return Value; }
    static EALPackageType PackageTypeFromString(const FString& Value) { const UEnum* Enum = StaticEnum<EALPackageType>(); const int64 Number = Enum ? Enum->GetValueByNameString(Value) : INDEX_NONE; return Number == INDEX_NONE ? EALPackageType::Core : static_cast<EALPackageType>(Number); }
}

void UALContentManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    InitializeContentSystem();
}

void UALContentManagerSubsystem::Deinitialize()
{
    ClearTempFiles();
    Super::Deinitialize();
}

bool UALContentManagerSubsystem::InitializeContentSystem()
{
    if (bInitialized) return true;
    ContentRoot = FPaths::Combine(FPaths::ProjectPersistentDownloadDir(), TEXT("AshLine"));
    InstalledRoot = FPaths::Combine(ContentRoot, TEXT("Content/Installed"));
    DownloadsRoot = FPaths::Combine(ContentRoot, TEXT("Content/Downloads"));
    TempRoot = FPaths::Combine(ContentRoot, TEXT("Content/Temp"));
    PatchesRoot = FPaths::Combine(ContentRoot, TEXT("Content/Patches"));
    ManifestRoot = FPaths::Combine(ContentRoot, TEXT("Manifests"));
    RegistryPath = FPaths::Combine(ManifestRoot, TEXT("registry.json"));
    for (const FString& Directory : {ContentRoot, InstalledRoot, DownloadsRoot, TempRoot, PatchesRoot, ManifestRoot}) IFileManager::Get().MakeDirectory(*Directory, true);
    Registry = NewObject<UALPackageRegistry>(this);
    Validator = NewObject<UALPackageValidator>(this);
    MountManager = NewObject<UALPackageMountManager>(this);
    LocalService = NewObject<UALLocalContentDeliveryService>(this);
    if (!Registry->Load(RegistryPath)) Registry->RebuildFromDirectory(InstalledRoot, RegistryPath);
    LoadGlobalManifest();
    RefreshInstalledPackages();
    bInitialized = true;
    return true;
}

bool UALContentManagerSubsystem::LoadGlobalManifest()
{
    const FString ManifestPath = FPaths::Combine(FPaths::ProjectContentDir(), TEXT("AshLine/Data/content_manifest.json"));
    FString Text;
    if (!FFileHelper::LoadFileToString(Text, *ManifestPath)) return false;
    TSharedPtr<FJsonObject> Root;
    if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Text), Root) || !Root.IsValid()) return false;
    GlobalManifest.SchemaVersion = ALContentManagerInternal::GetInt(Root, TEXT("schemaVersion"), 1);
    GlobalManifest.GameVersion = ALContentManagerInternal::GetString(Root, TEXT("gameVersion"));
    GlobalManifest.ContentVersion = ALContentManagerInternal::GetInt(Root, TEXT("contentVersion"), 1);
    GlobalManifest.ManifestSignature = ALContentManagerInternal::GetString(Root, TEXT("manifestSignature"));
    GlobalManifest.Packages.Reset();
    const TArray<TSharedPtr<FJsonValue>>* Values = nullptr;
    if (!Root->TryGetArrayField(TEXT("packages"), Values)) return true;
    for (const TSharedPtr<FJsonValue>& Value : *Values)
    {
        const TSharedPtr<FJsonObject> Object = Value->AsObject();
        if (!Object.IsValid()) continue;
        FALContentPackage Package;
        Package.PackageId = ALContentManagerInternal::GetString(Object, TEXT("id"));
        if (Package.PackageId.IsEmpty()) Package.PackageId = ALContentManagerInternal::GetString(Object, TEXT("packageId"));
        Package.PackageType = ALContentManagerInternal::PackageTypeFromString(ALContentManagerInternal::GetString(Object, TEXT("packageType")));
        Package.Version = ALContentManagerInternal::GetInt(Object, TEXT("version"), 1);
        Package.MinGameVersion = ALContentManagerInternal::GetString(Object, TEXT("minGameVersion"));
        Package.MaxGameVersion = ALContentManagerInternal::GetString(Object, TEXT("maxGameVersion"));
        Package.DownloadSizeBytes = ALContentManagerInternal::GetInt64(Object, TEXT("downloadSizeBytes"), ALContentManagerInternal::GetInt64(Object, TEXT("downloadSize"), 0));
        Package.InstalledSizeBytes = ALContentManagerInternal::GetInt64(Object, TEXT("installedSizeBytes"), ALContentManagerInternal::GetInt64(Object, TEXT("installedSize"), 0));
        Package.MountPriority = ALContentManagerInternal::GetInt(Object, TEXT("mountPriority"), 0);
        Package.SHA256 = ALContentManagerInternal::GetString(Object, TEXT("sha256"));
        Package.Signature = ALContentManagerInternal::GetString(Object, TEXT("signature"));
        Package.PublicKeyId = ALContentManagerInternal::GetString(Object, TEXT("publicKeyId"));
        Package.ContentRoot = ALContentManagerInternal::GetString(Object, TEXT("contentRoot"));
        Package.Url = ALContentManagerInternal::GetString(Object, TEXT("url"));
        Package.AssetDeliveryIdentifier = ALContentManagerInternal::GetString(Object, TEXT("assetDeliveryIdentifier"));
        Package.bRequired = ALContentManagerInternal::GetBool(Object, TEXT("required"), false);
        const TArray<TSharedPtr<FJsonValue>>* Dependencies = nullptr;
        if (Object->TryGetArrayField(TEXT("dependencies"), Dependencies)) for (const TSharedPtr<FJsonValue>& Dependency : *Dependencies) Package.Dependencies.Add(Dependency->AsString());
        if (Object->TryGetArrayField(TEXT("optionalDependencies"), Dependencies)) for (const TSharedPtr<FJsonValue>& Dependency : *Dependencies) Package.OptionalDependencies.Add(Dependency->AsString());
        GlobalManifest.Packages.Add(Package);
    }
    return true;
}

bool UALContentManagerSubsystem::RefreshInstalledPackages()
{
    if (!Registry) return false;
    if (!Registry->Load(RegistryPath)) Registry->RebuildFromDirectory(InstalledRoot, RegistryPath);
    return true;
}

bool UALContentManagerSubsystem::ResolvePackage(const FString& PackageId, FALContentPackage& OutPackage) const
{
    for (const FALContentPackage& Package : GlobalManifest.Packages) if (Package.PackageId == PackageId) { OutPackage = Package; return true; }
    for (const FALInstalledPackageRecord& Record : Registry->GetRecords()) if (Record.PackageId == PackageId) { OutPackage.PackageId = Record.PackageId; OutPackage.Version = Record.Version; OutPackage.SHA256 = Record.SHA256; OutPackage.InstalledPath = Record.Path; OutPackage.State = Record.State; OutPackage.bMounted = Record.bMounted; OutPackage.Dependencies = Record.Dependencies; return true; }
    return false;
}

bool UALContentManagerSubsystem::IsPackageInstalled(const FString& PackageId) const
{
    FALInstalledPackageRecord Record;
    return Registry && Registry->GetRecord(PackageId, Record) && (Record.State == EALPackageState::Installed || Record.State == EALPackageState::Mounted);
}

bool UALContentManagerSubsystem::IsPackageMounted(const FString& PackageId) const { return MountManager && MountManager->IsMounted(PackageId); }
EALPackageState UALContentManagerSubsystem::GetPackageState(const FString& PackageId) const { FALInstalledPackageRecord Record; return Registry && Registry->GetRecord(PackageId, Record) ? Record.State : EALPackageState::NotInstalled; }

bool UALContentManagerSubsystem::SetPackageState(const FString& PackageId, EALPackageState State)
{
    FALInstalledPackageRecord Record;
    if (!Registry || !Registry->GetRecord(PackageId, Record)) return false;
    Record.State = State;
    Record.bMounted = State == EALPackageState::Mounted;
    Registry->SetRecord(Record);
    Registry->Save(RegistryPath);
    OnPackageStateChanged.Broadcast(PackageId, State);
    return true;
}

bool UALContentManagerSubsystem::FailPackage(const FString& PackageId, EALContentError Error, const FString& Message)
{
    SetPackageState(PackageId, EALPackageState::Failed);
    UE_LOG(LogAshLineContent, Warning, TEXT("Package %s failed: %s"), *PackageId, *Message);
    OnPackageError.Broadcast(PackageId, Error, Message);
    return false;
}

bool UALContentManagerSubsystem::RequestPackage(const FString& PackageId)
{
    return IsPackageInstalled(PackageId) ? MountPackage(PackageId) : false;
}

bool UALContentManagerSubsystem::ImportPackage(const FString& FileReference)
{
    if (!Validator || FileReference.IsEmpty() || !FPaths::DirectoryExists(FileReference)) { OnPackageError.Broadcast(FileReference, EALContentError::FileNotFound, TEXT("Development import expects an ALPACK directory with manifest.json.")); return false; }
    const FString SourceManifest = FPaths::Combine(FileReference, TEXT("manifest.json"));
    FALContentPackage Package;
    if (!ParsePackageManifest(SourceManifest, Package)) { OnPackageError.Broadcast(FileReference, EALContentError::InvalidManifest, TEXT("Could not read ALPACK manifest.")); return false; }
    EALContentError Error; FString Message;
    if (!Validator->ValidatePackage(Package, GlobalManifest.GameVersion.IsEmpty() ? TEXT("0.1.0") : GlobalManifest.GameVersion, bAllowUnsignedDevelopmentPackages ? false : true, Error, Message)) { OnPackageError.Broadcast(Package.PackageId, Error, Message); return false; }
    GlobalManifest.Packages.RemoveAll([&](const FALContentPackage& Existing) { return Existing.PackageId == Package.PackageId; });
    GlobalManifest.Packages.Add(Package);
    const FString TempPackageRoot = FPaths::Combine(TempRoot, Package.PackageId);
    IFileManager::Get().DeleteDirectory(*TempPackageRoot, false, true);
    if (!IFileManager::Get().CopyDirectoryTree(*TempPackageRoot, *FileReference, true)) { OnPackageError.Broadcast(Package.PackageId, EALContentError::CopyFailed, TEXT("Failed to copy ALPACK to temporary storage.")); return false; }
    if (!InstallPackage(Package.PackageId)) return false;
    return MountPackage(Package.PackageId);
}

bool UALContentManagerSubsystem::ParsePackageManifest(const FString& ManifestPath, FALContentPackage& OutPackage) const
{
    FString Text;
    if (!FFileHelper::LoadFileToString(Text, *ManifestPath)) return false;
    TSharedPtr<FJsonObject> Object;
    if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Text), Object) || !Object.IsValid()) return false;
    OutPackage.PackageId = ALContentManagerInternal::GetString(Object, TEXT("packageId"));
    OutPackage.PackageType = ALContentManagerInternal::PackageTypeFromString(ALContentManagerInternal::GetString(Object, TEXT("packageType")));
    OutPackage.Version = ALContentManagerInternal::GetInt(Object, TEXT("version"), 1);
    OutPackage.MinGameVersion = ALContentManagerInternal::GetString(Object, TEXT("minGameVersion"));
    OutPackage.MaxGameVersion = ALContentManagerInternal::GetString(Object, TEXT("maxGameVersion"));
    OutPackage.DownloadSizeBytes = ALContentManagerInternal::GetInt64(Object, TEXT("downloadSize"), 0);
    OutPackage.InstalledSizeBytes = ALContentManagerInternal::GetInt64(Object, TEXT("installedSize"), 0);
    OutPackage.MountPriority = ALContentManagerInternal::GetInt(Object, TEXT("mountPriority"), 0);
    OutPackage.SHA256 = ALContentManagerInternal::GetString(Object, TEXT("sha256"));
    OutPackage.Signature = ALContentManagerInternal::GetString(Object, TEXT("signature"));
    OutPackage.PublicKeyId = ALContentManagerInternal::GetString(Object, TEXT("publicKeyId"));
    OutPackage.ContentRoot = ALContentManagerInternal::GetString(Object, TEXT("contentRoot"));
    OutPackage.bRequired = ALContentManagerInternal::GetBool(Object, TEXT("required"), false);
    const TArray<TSharedPtr<FJsonValue>>* Dependencies = nullptr;
    if (Object->TryGetArrayField(TEXT("dependencies"), Dependencies)) for (const TSharedPtr<FJsonValue>& Dependency : *Dependencies) OutPackage.Dependencies.Add(Dependency->AsString());
    return !OutPackage.PackageId.IsEmpty();
}

bool UALContentManagerSubsystem::EnsureDependenciesInstalled(const FALContentPackage& Package)
{
    for (const FString& Dependency : Package.Dependencies) if (!IsPackageInstalled(Dependency)) return false;
    return true;
}

bool UALContentManagerSubsystem::InstallPackage(const FString& PackageId)
{
    FALContentPackage Package;
    if (!ResolvePackage(PackageId, Package)) return FailPackage(PackageId, EALContentError::FileNotFound, TEXT("Package is not present in the manifest or temporary import area."));
    if (!EnsureDependenciesInstalled(Package)) return FailPackage(PackageId, EALContentError::MissingDependency, TEXT("Required dependency is not installed."));
    const FString TempPackageRoot = FPaths::Combine(TempRoot, PackageId);
    if (!FPaths::DirectoryExists(TempPackageRoot)) return FailPackage(PackageId, EALContentError::FileNotFound, TEXT("Temporary package directory is missing."));
    const FString InstalledPackageRoot = GetPackageRoot(PackageId);
    SetPackageState(PackageId, EALPackageState::Installing);
    IFileManager::Get().DeleteDirectory(*InstalledPackageRoot, false, true);
    if (!IFileManager::Get().CopyDirectoryTree(*InstalledPackageRoot, *TempPackageRoot, true)) return FailPackage(PackageId, EALContentError::CopyFailed, TEXT("Atomic package copy failed."));
    IFileManager::Get().DeleteDirectory(*TempPackageRoot, false, true);
    FALInstalledPackageRecord Record;
    Record.PackageId = Package.PackageId;
    Record.Version = Package.Version;
    Record.Path = InstalledPackageRoot;
    Record.SHA256 = Package.SHA256;
    Record.State = EALPackageState::Installed;
    Record.InstalledSizeBytes = Package.InstalledSizeBytes;
    Record.Dependencies = Package.Dependencies;
    Record.InstallDateUtc = FDateTime::UtcNow().ToIso8601();
    Registry->SetRecord(Record);
    Registry->Save(RegistryPath);
    OnPackageInstalled.Broadcast(PackageId);
    OnPackageProgress.Broadcast(PackageId, 1.0f);
    OnPackageStateChanged.Broadcast(PackageId, EALPackageState::Installed);
    return true;
}

bool UALContentManagerSubsystem::MountPackage(const FString& PackageId)
{
    FALContentPackage Package; FALInstalledPackageRecord Record;
    if (!ResolvePackage(PackageId, Package) || !Registry->GetRecord(PackageId, Record)) return FailPackage(PackageId, EALContentError::FileNotFound, TEXT("Package is not installed."));
    if (!EnsureDependenciesInstalled(Package)) return FailPackage(PackageId, EALContentError::MissingDependency, TEXT("Dependency must be installed before mount."));
    if (!VerifyPackage(PackageId)) return false;
    SetPackageState(PackageId, EALPackageState::Mounting);
    FString Message;
    if (!MountManager->MountPackage(Package, Record.Path, Message)) return FailPackage(PackageId, EALContentError::MountFailed, Message);
    SetPackageState(PackageId, EALPackageState::Mounted);
    OnPackageMounted.Broadcast(PackageId);
    return true;
}

bool UALContentManagerSubsystem::UnmountPackage(const FString& PackageId)
{
    FALContentPackage Package; FALInstalledPackageRecord Record;
    if (!ResolvePackage(PackageId, Package) || !Registry->GetRecord(PackageId, Record)) return false;
    if (Package.bRequired) return FailPackage(PackageId, EALContentError::Busy, TEXT("Required package cannot be unmounted."));
    FString Message;
    if (!MountManager->UnmountPackage(Package, Record.Path, Message)) return FailPackage(PackageId, EALContentError::MountFailed, Message);
    SetPackageState(PackageId, EALPackageState::Installed);
    return true;
}

bool UALContentManagerSubsystem::RemovePackage(const FString& PackageId)
{
    FALContentPackage Package; FALInstalledPackageRecord Record;
    if (!ResolvePackage(PackageId, Package) || !Registry->GetRecord(PackageId, Record)) return false;
    if (Package.bRequired) return FailPackage(PackageId, EALContentError::Busy, TEXT("Required package cannot be removed."));
    for (const FALInstalledPackageRecord& Other : Registry->GetRecords()) if (Other.PackageId != PackageId && Other.Dependencies.Contains(PackageId)) return FailPackage(PackageId, EALContentError::Busy, TEXT("Package is required by another installed package."));
    if (IsPackageMounted(PackageId) && !UnmountPackage(PackageId)) return false;
    if (!IFileManager::Get().DeleteDirectory(*Record.Path, false, true)) return FailPackage(PackageId, EALContentError::CopyFailed, TEXT("Package files could not be removed."));
    Registry->RemoveRecord(PackageId);
    Registry->Save(RegistryPath);
    OnPackageRemoved.Broadcast(PackageId);
    return true;
}

bool UALContentManagerSubsystem::UpdatePackage(const FString& PackageId)
{
    const FString UpdateRoot = FPaths::Combine(DownloadsRoot, PackageId);
    if (!FPaths::DirectoryExists(UpdateRoot)) return FailPackage(PackageId, EALContentError::FileNotFound, TEXT("No downloaded update is available."));
    const FString TempRootForUpdate = FPaths::Combine(TempRoot, PackageId);
    IFileManager::Get().DeleteDirectory(*TempRootForUpdate, false, true);
    if (!IFileManager::Get().CopyDirectoryTree(*TempRootForUpdate, *UpdateRoot, true)) return FailPackage(PackageId, EALContentError::CopyFailed, TEXT("Update staging copy failed."));
    return InstallPackage(PackageId) && MountPackage(PackageId);
}

bool UALContentManagerSubsystem::VerifyPackage(const FString& PackageId)
{
    FALContentPackage Package; FALInstalledPackageRecord Record;
    if (!ResolvePackage(PackageId, Package) || !Registry->GetRecord(PackageId, Record)) return false;
    EALContentError Error; FString Message;
    const bool bValid = Validator->ValidatePackage(Package, GlobalManifest.GameVersion.IsEmpty() ? TEXT("0.1.0") : GlobalManifest.GameVersion, bAllowUnsignedDevelopmentPackages ? false : true, Error, Message);
    if (!bValid) return FailPackage(PackageId, Error, Message);
    return true;
}

float UALContentManagerSubsystem::GetDownloadProgress(const FString& PackageId) const { return IsPackageInstalled(PackageId) ? 1.0f : 0.0f; }
TArray<FString> UALContentManagerSubsystem::GetRequiredDependencies(const FString& PackageId) const { FALContentPackage Package; return ResolvePackage(PackageId, Package) ? Package.Dependencies : TArray<FString>(); }
int64 UALContentManagerSubsystem::GetInstalledContentSize() const { int64 Total = 0; if (Registry) for (const FALInstalledPackageRecord& Record : Registry->GetRecords()) Total += Record.InstalledSizeBytes; return Total; }
int64 UALContentManagerSubsystem::GetPackageInstalledSize(const FString& PackageId) const { FALInstalledPackageRecord Record; return Registry && Registry->GetRecord(PackageId, Record) ? Record.InstalledSizeBytes : 0; }
int64 UALContentManagerSubsystem::GetFreeStorage() const { return IFileManager::Get().GetDiskFreeSpace(*ContentRoot); }
void UALContentManagerSubsystem::ClearTempFiles() { IFileManager::Get().DeleteDirectory(*TempRoot, false, true); IFileManager::Get().MakeDirectory(*TempRoot, true); }
void UALContentManagerSubsystem::ClearFailedDownloads() { IFileManager::Get().DeleteDirectory(*DownloadsRoot, false, true); IFileManager::Get().MakeDirectory(*DownloadsRoot, true); }
void UALContentManagerSubsystem::ClearUnusedCache() { }
FString UALContentManagerSubsystem::GetPackageRoot(const FString& PackageId) const { return FPaths::Combine(InstalledRoot, PackageId); }
FString UALContentManagerSubsystem::GetPackageManifestPath(const FString& PackageId) const { return FPaths::Combine(GetPackageRoot(PackageId), TEXT("manifest.json")); }
