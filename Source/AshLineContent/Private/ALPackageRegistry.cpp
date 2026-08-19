#include "ALPackageRegistry.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"

namespace ALContentRegistry
{
    static FString StateToString(EALPackageState State)
    {
        return StaticEnum<EALPackageState>() ? StaticEnum<EALPackageState>()->GetNameStringByValue(static_cast<int64>(State)) : TEXT("Unknown");
    }
    static EALPackageState StateFromString(const FString& Value)
    {
        if (const UEnum* Enum = StaticEnum<EALPackageState>()) return static_cast<EALPackageState>(Enum->GetValueByNameString(Value));
        return EALPackageState::Unknown;
    }
}

bool UALPackageRegistry::Load(const FString& RegistryPath)
{
    FString Text;
    if (!FFileHelper::LoadFileToString(Text, *RegistryPath)) return false;
    TSharedPtr<FJsonObject> Root;
    if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Text), Root) || !Root.IsValid()) return false;
    Records.Reset();
    const TArray<TSharedPtr<FJsonValue>>* Values = nullptr;
    if (!Root->TryGetArrayField(TEXT("packages"), Values)) return true;
    for (const TSharedPtr<FJsonValue>& Value : *Values)
    {
        const TSharedPtr<FJsonObject> Object = Value->AsObject();
        if (!Object.IsValid()) continue;
        FALInstalledPackageRecord Record;
        Object->TryGetStringField(TEXT("packageId"), Record.PackageId);
        Object->TryGetNumberField(TEXT("version"), Record.Version);
        Object->TryGetStringField(TEXT("path"), Record.Path);
        Object->TryGetStringField(TEXT("sha256"), Record.SHA256);
        Object->TryGetStringField(TEXT("state"), Text);
        Record.State = ALContentRegistry::StateFromString(Text);
        Object->TryGetBoolField(TEXT("mounted"), Record.bMounted);
        Object->TryGetNumberField(TEXT("installedSizeBytes"), Record.InstalledSizeBytes);
        Object->TryGetStringField(TEXT("installDateUtc"), Record.InstallDateUtc);
        const TArray<TSharedPtr<FJsonValue>>* Dependencies = nullptr;
        if (Object->TryGetArrayField(TEXT("dependencies"), Dependencies)) for (const TSharedPtr<FJsonValue>& Dependency : *Dependencies) Record.Dependencies.Add(Dependency->AsString());
        if (!Record.PackageId.IsEmpty()) Records.Add(Record);
    }
    return true;
}

bool UALPackageRegistry::Save(const FString& RegistryPath) const
{
    TSharedPtr<FJsonObject> Root = MakeShared<FJsonObject>();
    TArray<TSharedPtr<FJsonValue>> Values;
    for (const FALInstalledPackageRecord& Record : Records)
    {
        TSharedPtr<FJsonObject> Object = MakeShared<FJsonObject>();
        Object->SetStringField(TEXT("packageId"), Record.PackageId);
        Object->SetNumberField(TEXT("version"), Record.Version);
        Object->SetStringField(TEXT("path"), Record.Path);
        Object->SetStringField(TEXT("sha256"), Record.SHA256);
        Object->SetStringField(TEXT("state"), ALContentRegistry::StateToString(Record.State));
        Object->SetBoolField(TEXT("mounted"), Record.bMounted);
        Object->SetNumberField(TEXT("installedSizeBytes"), Record.InstalledSizeBytes);
        Object->SetStringField(TEXT("installDateUtc"), Record.InstallDateUtc);
        TArray<TSharedPtr<FJsonValue>> Dependencies;
        for (const FString& Dependency : Record.Dependencies) Dependencies.Add(MakeShared<FJsonValueString>(Dependency));
        Object->SetArrayField(TEXT("dependencies"), Dependencies);
        Values.Add(MakeShared<FJsonValueObject>(Object));
    }
    Root->SetArrayField(TEXT("packages"), Values);
    FString Output;
    if (!FJsonSerializer::Serialize(Root.ToSharedRef(), TJsonWriterFactory<>::Create(&Output))) return false;
    IFileManager::Get().MakeDirectory(*FPaths::GetPath(RegistryPath), true);
    return FFileHelper::SaveStringToFile(Output, *RegistryPath);
}

bool UALPackageRegistry::RebuildFromDirectory(const FString& InstalledRoot, const FString& RegistryPath)
{
    Records.Reset();
    TArray<FString> ManifestFiles;
    IFileManager::Get().FindFilesRecursive(ManifestFiles, *InstalledRoot, TEXT("manifest.json"), true, false);
    for (const FString& ManifestPath : ManifestFiles)
    {
        FString Text;
        if (!FFileHelper::LoadFileToString(Text, *ManifestPath)) continue;
        TSharedPtr<FJsonObject> Root;
        if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Text), Root) || !Root.IsValid()) continue;
        FALInstalledPackageRecord Record;
        Root->TryGetStringField(TEXT("packageId"), Record.PackageId);
        Root->TryGetNumberField(TEXT("version"), Record.Version);
        Root->TryGetStringField(TEXT("sha256"), Record.SHA256);
        Record.Path = FPaths::GetPath(ManifestPath);
        Record.State = EALPackageState::Installed;
        Root->TryGetNumberField(TEXT("installedSize"), Record.InstalledSizeBytes);
        if (!Record.PackageId.IsEmpty()) Records.Add(Record);
    }
    return Save(RegistryPath);
}

void UALPackageRegistry::SetRecord(const FALInstalledPackageRecord& Record)
{
    for (FALInstalledPackageRecord& Existing : Records) if (Existing.PackageId == Record.PackageId) { Existing = Record; return; }
    Records.Add(Record);
}

bool UALPackageRegistry::RemoveRecord(const FString& PackageId)
{
    return Records.RemoveAll([&](const FALInstalledPackageRecord& Record) { return Record.PackageId == PackageId; }) > 0;
}

bool UALPackageRegistry::GetRecord(const FString& PackageId, FALInstalledPackageRecord& OutRecord) const
{
    for (const FALInstalledPackageRecord& Record : Records) if (Record.PackageId == PackageId) { OutRecord = Record; return true; }
    return false;
}

bool UALPackageRegistry::HasRecord(const FString& PackageId) const
{
    FALInstalledPackageRecord Ignored;
    return GetRecord(PackageId, Ignored);
}

TArray<FALInstalledPackageRecord> UALPackageRegistry::GetRecords() const { return Records; }
