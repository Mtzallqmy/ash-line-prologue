#include "ALSemanticVersion.h"

bool FALSemanticVersion::TryParse(const FString& Text, FALSemanticVersion& OutVersion)
{
    OutVersion = FALSemanticVersion();
    FString Normalized = Text.TrimStartAndEnd();
    if (Normalized.StartsWith(TEXT("v"), ESearchCase::IgnoreCase)) Normalized.RightChopInline(1);
    TArray<FString> Parts;
    Normalized.ParseIntoArray(Parts, TEXT("."), true);
    if (Parts.Num() != 3) return false;

    int32 ParsedMajor = 0;
    int32 ParsedMinor = 0;
    int32 ParsedPatch = 0;
    if (!LexTryParseString(ParsedMajor, *Parts[0]) || !LexTryParseString(ParsedMinor, *Parts[1]) || !LexTryParseString(ParsedPatch, *Parts[2])) return false;
    if (ParsedMajor < 0 || ParsedMinor < 0 || ParsedPatch < 0) return false;

    OutVersion.Major = ParsedMajor;
    OutVersion.Minor = ParsedMinor;
    OutVersion.Patch = ParsedPatch;
    return true;
}

FString FALSemanticVersion::ToString() const
{
    return FString::Printf(TEXT("%d.%d.%d"), Major, Minor, Patch);
}

bool FALSemanticVersion::operator==(const FALSemanticVersion& Other) const
{
    return Major == Other.Major && Minor == Other.Minor && Patch == Other.Patch;
}

bool FALSemanticVersion::operator!=(const FALSemanticVersion& Other) const
{
    return !(*this == Other);
}

bool FALSemanticVersion::operator<(const FALSemanticVersion& Other) const
{
    if (Major != Other.Major) return Major < Other.Major;
    if (Minor != Other.Minor) return Minor < Other.Minor;
    return Patch < Other.Patch;
}

bool FALSemanticVersion::operator<=(const FALSemanticVersion& Other) const
{
    return *this < Other || *this == Other;
}

bool FALSemanticVersion::operator>(const FALSemanticVersion& Other) const
{
    return Other < *this;
}

bool FALSemanticVersion::operator>=(const FALSemanticVersion& Other) const
{
    return Other < *this || *this == Other;
}
