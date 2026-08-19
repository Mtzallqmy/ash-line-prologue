#include "ALPackageValidator.h"
#include "Misc/Paths.h"

namespace ALContentValidation
{
    static bool IsValidPackageId(const FString& PackageId)
    {
        if (PackageId.IsEmpty() || PackageId.Contains(TEXT("..")) || PackageId.Contains(TEXT("/")) || PackageId.Contains(TEXT("\\"))) return false;
        for (TCHAR Character : PackageId) if (!(FChar::IsAlnum(Character) || Character == TEXT('.') || Character == TEXT('_') || Character == TEXT('-'))) return false;
        return true;
    }
}

bool UALPackageValidator::ValidateManifest(const FALContentManifest& Manifest, const FString& GameVersion, bool bRequireSignature, EALContentError& OutError, FString& OutMessage) const
{
    OutError = EALContentError::None;
    OutMessage.Reset();
    if (Manifest.SchemaVersion != 1) { OutError = EALContentError::UnsupportedSchema; OutMessage = TEXT("Unsupported content manifest schema."); return false; }
    if (bRequireSignature && Manifest.ManifestSignature.IsEmpty()) { OutError = EALContentError::InvalidSignature; OutMessage = TEXT("Official packages require a signed manifest."); return false; }
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
    if (Package.ContentRoot.Contains(TEXT("..")) || FPaths::IsRelative(Package.ContentRoot) == false && !Package.ContentRoot.StartsWith(TEXT("/Game/"))) { OutError = EALContentError::PathTraversal; OutMessage = TEXT("Unsafe content root."); return false; }
    if (bRequireSignature && (Package.Signature.IsEmpty() || Package.PublicKeyId.IsEmpty())) { OutError = EALContentError::InvalidSignature; OutMessage = TEXT("Signed package metadata is incomplete."); return false; }
    if (!Package.MinGameVersion.IsEmpty() && GameVersion < Package.MinGameVersion) { OutError = EALContentError::IncompatibleVersion; OutMessage = TEXT("Package requires a newer game version."); return false; }
    if (!Package.MaxGameVersion.IsEmpty() && GameVersion > Package.MaxGameVersion) { OutError = EALContentError::IncompatibleVersion; OutMessage = TEXT("Package is not compatible with this game version."); return false; }
    for (const FString& Dependency : Package.Dependencies) if (!ALContentValidation::IsValidPackageId(Dependency)) { OutError = EALContentError::InvalidManifest; OutMessage = TEXT("Invalid dependency ID."); return false; }
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
        OutMessage = TEXT("Package SHA-256 does not match the signed manifest.");
        return false;
    }
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
