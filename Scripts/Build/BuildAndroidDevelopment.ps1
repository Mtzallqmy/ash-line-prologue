[CmdletBinding()]
param(
    [string]$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
)

$ErrorActionPreference = 'Stop'
& (Join-Path $PSScriptRoot 'BuildAndroidPrototype.ps1') -ProjectRoot $ProjectRoot -Configuration Development
