#include "ALPackageValidator.h"
#include "ALSemanticVersion.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/SecureHash.h"

namespace ALContentValidation
{
    static bool IsValidPackageId(const FString& PackageId)
    {
        if (PackageId.IsEmpty() || PackageId.Contains(TEXT("..")) || PackageId.Contains(TEXT("/")) || PackageId.Contains(TEXT("\\"))) return false;
        for (TCHAR Character : PackageId) if (!(FChar::IsAlnum(Character) || Character == TEXT('.') || Character == TEXT('_') || Character == TEXT('-'))) return false;
        return true;
    }

    static bool ParseVersion(const FString& Text, FALSemanticVersion& OutVersion)
    {
        return !Text.IsEmpty() && FALSemanticVersion::TryParse(Text, OutVersion);
    }
}

bool UALPackageValidator::ValidateManifest(const FALContentManifest& Manifest, const FString& GameVersion, bool bRequireSignature, EALContentError& OutError, FString& OutMessage) const
{
    OutError = EALContentError::None;
    OutMessage.Reset();
    if (Manifest.SchemaVersion != 1) { OutError = EALContentError::UnsupportedSchema; OutMessage = TEXT("Unsupported content manifest schema."); return false; }
    if (bRequireSignature)
    {
        OutError = EALContentError::InvalidSignature;
        OutMessage = TEXT("Cryptographic manifest signature verification is not implemented; signed packages are blocked outside Development.");
        return false;
    }
    TSet<FString> Seen;
    for (const FALContentPackage& Package : Manifest.Packages)
    {
        if (Seen.Contains(Package.PackageId)) { OutError = EALContentError::InvalidManifest; OutMessage = TEXT("Duplicate package ID in manifest."); return false; }
        Seen.Add(Package.PackageId);
        if (!ValidatePackage(Package, GameVersion, bRequireSignature, OutError, OutMessage)) return false;
    }
    for (const FALContentPackage& Package : Manifest.Packages)
    {
        TArray<FString> Dependencies;
        if (!ResolveDependencies(Package.PackageId, Manifest, Dependencies, OutError, OutMessage)) return false;
    }
    return true;
}

bool UALPackageValidator::ValidatePackage(const FALContentPackage& Package, const FString& GameVersion, bool bRequireSignature, EALContentError& OutError, FString& OutMessage) const
{
    OutError = EALContentError::None;
    OutMessage.Reset();
    if (!ALContentValidation::IsValidPackageId(Package.PackageId)) { OutError = EALContentError::InvalidManifest; OutMessage = TEXT("Invalid package ID."); return false; }
    if (Package.DownloadSizeBytes < 0 || Package.InstalledSizeBytes < 0 || Package.InstalledSizeBytes > MaxPackageSizeBytes) { OutError = EALContentError::SizeLimitExceeded; OutMessage = TEXT("Package size exceeds configured limits."); return false; }
    if (Package.ContentRoot.Contains(TEXT("..")) || (!FPaths::IsRelative(Package.ContentRoot) && !Package.ContentRoot.StartsWith(TEXT("/Game/")))) { OutError = EALContentError::PathTraversal; OutMessage = TEXT("Unsafe content root."); return false; }
    if (bRequireSignature)
    {
        OutError = EALContentError::InvalidSignature;
        OutMessage = TEXT("Cryptographic package signature verification is not implemented; signed packages are blocked outside Development.");
        return false;
    }
    if (!Package.Signature.IsEmpty() && Package.PublicKeyId.IsEmpty()) { OutError = EALContentError::InvalidSignature; OutMessage = TEXT("Package signature metadata is incomplete."); return false; }

    FALSemanticVersion CurrentVersion;
    if (!ALContentValidation::ParseVersion(GameVersion, CurrentVersion)) { OutError = EALContentError::IncompatibleVersion; OutMessage = TEXT("Game version is not valid semantic version Major.Minor.Patch."); return false; }
    FALSemanticVersion MinVersion;
    if (!Package.MinGameVersion.IsEmpty())
    {
        if (!ALContentValidation::ParseVersion(Package.MinGameVersion, MinVersion)) { OutError = EALContentError::InvalidManifest; OutMessage = TEXT("Package minimum game version is invalid."); return false; }
        if (CurrentVersion < MinVersion) { OutError = EALContentError::IncompatibleVersion; OutMessage = TEXT("Package requires a newer game version."); return false; }
    }
    if (!Package.MaxGameVersion.IsEmpty())
    {
        FALSemanticVersion MaxVersion;
        if (!ALContentValidation::ParseVersion(Package.MaxGameVersion, MaxVersion)) { OutError = EALContentError::InvalidManifest; OutMessage = TEXT("Package maximum game version is invalid."); return false; }
        if (!Package.MinGameVersion.IsEmpty() && MaxVersion < MinVersion) { OutError = EALContentError::InvalidManifest; OutMessage = TEXT("Package version range is inverted."); return false; }
        if (MaxVersion < CurrentVersion) { OutError = EALContentError::IncompatibleVersion; OutMessage = TEXT("Package is not compatible with this game version."); return false; }
    }
    for (const FString& Dependency : Package.Dependencies) if (!ALContentValidation::IsValidPackageId(Dependency)) { OutError = EALContentError::InvalidManifest; OutMessage = TEXT("Invalid dependency ID."); return false; }
    for (const FString& Dependency : Package.OptionalDependencies) if (!ALContentValidation::IsValidPackageId(Dependency)) { OutError = EALContentError::InvalidManifest; OutMessage = TEXT("Invalid optional dependency ID."); return false; }
    return true;
}

bool UALPackageValidator::ValidatePath(const FString& RelativePath, EALContentError& OutError, FString& OutMessage) const
{
    OutError = EALContentError::None;
    OutMessage.Reset();
    if (RelativePath.IsEmpty() || RelativePath.Contains(TEXT("..")) || FPaths::IsDrive(RelativePath) || RelativePath.StartsWith(TEXT("/")) || RelativePath.StartsWith(TEXT("\\")))
    {
        OutError = EALContentError::PathTraversal;
        OutMessage = TEXT("Package path is absolute or contains traversal segments.");
        return false;
    }
    return true;
}

bool UALPackageValidator::VerifySHA256(const FString& ExpectedHash, const FString& ActualHash, EALContentError& OutError, FString& OutMessage) const
{
    OutError = EALContentError::None;
    OutMessage.Reset();
    if (ExpectedHash.IsEmpty() || ActualHash.IsEmpty() || !ExpectedHash.Equals(ActualHash, ESearchCase::IgnoreCase))
    {
        OutError = EALContentError::HashMismatch;
        OutMessage = TEXT("Package SHA-256 does not match the verified content digest.");
        return false;
    }
    return true;
}

bool UALPackageValidator::ComputeDevelopmentDirectoryHash(const FString& DirectoryPath, FString& OutHash, FString& OutMessage) const
{
    OutHash.Reset();
    OutMessage.Reset();
    const FString Root = FPaths::DirectoryExists(FPaths::Combine(DirectoryPath, TEXT("content"))) ? FPaths::Combine(DirectoryPath, TEXT("content")) : DirectoryPath;
    if (!FPaths::DirectoryExists(Root)) { OutMessage = TEXT("Development package directory does not exist."); return false; }

    TArray<FString> Files;
    IFileManager::Get().FindFilesRecursive(Files, *Root, TEXT("*"), true, false);
    Files.Sort();
    if (Files.Num() > MaxFileCount) { OutMessage = TEXT("Development package contains too many files."); return false; }

    FSHA256 Hash;
    int64 TotalBytes = 0;
    for (const FString& FilePath : Files)
    {
        FString RelativePath = FilePath;
        FPaths::MakePathRelativeTo(RelativePath, *Root);
        EALContentError PathError = EALContentError::None;
        FString PathMessage;
        if (!ValidatePath(RelativePath, PathError, PathMessage)) { OutMessage = PathMessage; return false; }
        FTCHARToUTF8 RelativeUtf8(*RelativePath);
        Hash.Update(reinterpret_cast<const uint8*>(RelativeUtf8.Get()), RelativeUtf8.Length());
        const uint8 Separator = 0;
        Hash.Update(&Separator, 1);

        TArray<uint8> Bytes;
        if (!FFileHelper::LoadFileToArray(Bytes, *FilePath)) { OutMessage = FString::Printf(TEXT("Could not read package file: %s"), *RelativePath); return false; }
        TotalBytes += Bytes.Num();
        if (TotalBytes > MaxPackageSizeBytes) { OutMessage = TEXT("Development package exceeds configured size limit."); return false; }
        if (Bytes.Num() > 0) Hash.Update(Bytes.GetData(), Bytes.Num());
    }

    uint8 Digest[32];
    Hash.Final();
    Hash.GetHash(Digest);
    OutHash = BytesToHex(Digest, UE_ARRAY_COUNT(Digest));
    return true;
}

bool UALPackageValidator::ResolveDependencies(const FString& PackageId, const FALContentManifest& Manifest, TArray<FString>& OutDependencies, EALContentError& OutError, FString& OutMessage) const
{
    OutDependencies.Reset();
    OutError = EALContentError::None;
    OutMessage.Reset();
    TMap<FString, const FALContentPackage*> Packages;
    for (const FALContentPackage& Package : Manifest.Packages) Packages.Add(Package.PackageId, &Package);
    TSet<FString> Visiting;
    TSet<FString> Visited;
    TFunction<bool(const FString&)> Visit = [&](const FString& Id)
    {
        if (Visited.Contains(Id)) return true;
        if (Visiting.Contains(Id)) { OutError = EALContentError::CircularDependency; OutMessage = TEXT("Circular package dependency detected."); return false; }
        const FALContentPackage* Package = Packages.FindRef(Id);
        if (!Package) { OutError = EALContentError::MissingDependency; OutMessage = FString::Printf(TEXT("Missing package dependency: %s"), *Id); return false; }
        Visiting.Add(Id);
        for (const FString& Dependency : Package->Dependencies) if (!Visit(Dependency)) return false;
        Visiting.Remove(Id);
        Visited.Add(Id);
        if (Id != PackageId) OutDependencies.AddUnique(Id);
        return true;
    };
    return Visit(PackageId);
}
