# ASH LINE v0.0.1 APK BUILD REPORT — Prompt 09

## Final status

> **BLOCKED: UNREAL ENGINE INSTALLATION REQUIRED**
>
> Android SDK, Build Tools, NDK, ADB, and the project validation path were prepared successfully. The actual APK cannot be produced because Unreal Engine 5.4.x, UnrealBuildTool, UnrealHeaderTool, and UnrealEditor-Cmd are not installed and no installable Unreal package is available in this Linux Sandbox.

## Repository

| Field | Result |
|---|---|
| Repository | `Mtzallqmy/ash-line-prologue` |
| Branch | `release/combat-prototype-v0.0.1` |
| Base | Corrected Combat Prototype release branch |
| Latest source branch before this report | `7c1110b` |

## Environment installation results

| Requirement | Result |
|---|---|
| Java | Java 21.0.11 already available |
| ADB | Installed from Ubuntu package `adb` 34.0.4 |
| Android SDK root | `/usr/lib/android-sdk` |
| Android platform | API 34 installed at `/usr/lib/android-sdk/platforms/android-34` |
| Android Build Tools | 34.0.0 installed at `/usr/lib/android-sdk/build-tools/34.0.0` |
| Android NDK | r25c / 25.2.9519653 installed at `/usr/lib/android-sdk/ndk/25.2.9519653` |
| `adb devices` | Runs successfully; no Android device connected |
| Unreal Engine | Not installed |
| UnrealBuildTool | Not found |
| UnrealHeaderTool | Not found |
| UnrealEditor-Cmd | Not found |
| Visual Studio C++ | Not applicable in this Linux Sandbox; Windows toolchain unavailable |
| PowerShell | Not installed |

The Android-side installation was real and verified. The NDK installer downloaded and installed the package from the official Google mirror selected during package setup. No SDK, NDK, or Unreal binaries were added to the Git repository.

## Paths searched for Unreal

The following local paths were checked and did not contain Unreal Engine:

```text
/opt/UnrealEngine
/home/ubuntu/UnrealEngine
/home/ubuntu/EpicGames
```

A broader local filesystem search for `UnrealEditor`, `UnrealEditor-Cmd`, `UnrealBuildTool`, and `UnrealBuildTool.exe` also returned no result. Protected mounted paths were not scanned because the sandbox forbids recursive operations over those mounts.

## Validation result

The pre-build gate passed:

```text
validate_project.py              PASS
validate_content_system.py       PASS
validate_prompt02.py             PASS
validate_prompt03.py             PASS
validate_prompt04.py             PASS
validate_prompt05.py             PASS
validate_build_references.py      PASS
static_surface_check.py          PASS
Python syntax                    PASS
UProject JSON                    PASS
git diff --check                 PASS
```

## Build attempts

| Target | Exit | Actual result |
|---|---:|---|
| Pre-build validation | `0` | PASS |
| Editor build | `2` | `UNREAL BUILD ENVIRONMENT NOT AVAILABLE: set UE_ROOT to a UE 5.4 installation.` |
| Android Development | `2` | `ANDROID BUILD BLOCKED: Set UE_ROOT to the installed Unreal Engine 5.4 directory.` |
| Android Shipping | `2` | `ANDROID BUILD BLOCKED: Set UE_ROOT to the installed Unreal Engine 5.4 directory.` |
| System Python asset generator | `1` | Correctly refused; must run through Unreal Editor Python |
| APK search | — | No APK found |

Installing Android SDK/NDK did not change the result because all Unreal build gates stop before UAT when `UE_ROOT` and UnrealBuildTool are absent.

## Assets and APK

No `.umap`, `.uasset`, or APK was created. `Scripts/Editor/CreatePrototypeAssets.py` remains ready to run through:

```text
UnrealEditor-Cmd ASH_LINE.uproject -run=pythonscript -script=Scripts/Editor/CreatePrototypeAssets.py -unattended -nop4 -log
```

The expected APK path remains:

```text
Releases/Android/0.0.1/APK/AshLine_CombatPrototype_v0.0.1_android_arm64.apk
```

Because no APK exists, APK size, ABI, manifest, signature, SHA-256, installation, launch, Logcat, Runtime Smoke Test, FPS, and RAM measurements are **not available** and are intentionally not fabricated.

## Exact remaining action

Run the project on a Windows machine or runner that has Unreal Engine 5.4.x with Android Target Platform installed. Then set `UE_ROOT`, open or generate project files, run `BuildEditor.ps1`, execute the Editor Python generator, and run `BuildAndroidPrototype.ps1 -Configuration Development` followed by Shipping. The existing scripts will generate the APK and reports only after real BuildCookRun succeeds.

**FIRST PLAYABLE APK: NOT READY.**
