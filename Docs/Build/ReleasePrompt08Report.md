# ASH LINE v0.0.1 RELEASE REPORT — Prompt 08

## Final release status

> **APK RELEASE READY = NO**
>
> **CODE READY / BUILD BLOCKED** — source and pre-build validation pass, but Unreal Engine and the Android toolchain are absent from the execution environment. No APK, `.umap`, or `.uasset` was fabricated.

## Repository and branch

| Field | Result |
|---|---|
| Repository | `https://github.com/Mtzallqmy/ash-line-prologue` |
| Release branch | `release/combat-prototype-v0.0.1` |
| Latest branch commit | `8319367` — `feat: wire mobile touch and input asset defaults` |
| Published | Branch must be pushed after final documentation commit |
| Target | ASH LINE Combat Prototype v0.0.1 |

## Environment

| Tool | Result |
|---|---|
| Unreal Engine 5.4/5.4.4 | Not installed |
| UnrealBuildTool | Not available |
| UnrealHeaderTool | Not available |
| Visual Studio C++ | Not available in Linux Sandbox |
| Android SDK | Not installed |
| Android NDK | Not installed |
| JDK | Java 21 available, but insufficient without UE/SDK/NDK |
| `adb` | Not available |
| PowerShell | Not available |
| `UE_ROOT` | Empty |
| `ANDROID_HOME` | Empty |
| `ANDROID_NDK_HOME` | Empty |

## Implemented Prompt 08 changes

تم إنشاء فرع الإصدار، وإزالة `UALCombatComponent` القديم الذي كان يستدعي API غير موجودة مثل `CurrentWeapon->Fire()` و`CurrentWeapon->Reload()`. أصبح `UALWeaponComponent` هو Source of Truth الوحيد لإطلاق النار وإعادة التعبئة والتبديل.

تمت إضافة Player loadout فعلي في C++ يعتمد على Deferred Spawn، ويقبل `PrimaryWeaponClass/Data` و`SidearmWeaponClass/Data` وDevelopment SMG. بعد تعيين Blueprint defaults، يضمن التسلسل إنشاء AR وPistol ثم تجهيز AR كسلاح حالي.

تم إصلاح Enemy weapon spawn ليعيّن `WeaponData` قبل اكتمال BeginPlay، ثم يجهز `WeaponComponent` بسلاح صالح. أضيفت Plugins `PythonScriptPlugin` و`EditorScriptingUtilities` كـEditor-only tooling، دون اعتماد Runtime Gameplay عليها.

تم توسيع `CreatePrototypeAssets.py` ليولّد `BP_ALPlayerController`، و`BP_WPN_AR_001`، و`BP_WPN_SMG_001`، و`BP_WPN_PST_001`، ويربط Input Actions بالـPlayerController، ويربط Player loadout وEnemy WeaponClass وGameMode defaults وHUD وMobileTouchWidget.

تمت إضافة W/A/S/D mappings مع Negate وSwizzle metadata، وإضافة إنشاء MobileTouchLayer من PlayerController عبر `MobileTouchWidgetClass` مع بقاء منطق Gameplay مشتركًا بين PC وAndroid.

## Validation

نجحت الفحوصات التالية:

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

## Unreal build result

| Target | Exit | Result |
|---|---:|---|
| Pre-build validation | `0` | PASS |
| `BuildEditor.sh` | `2` | `UNREAL BUILD ENVIRONMENT NOT AVAILABLE: set UE_ROOT to a UE 5.4 installation.` |
| Windows `BuildEditor.ps1` | Not executable | PowerShell and UE unavailable |
| Unreal Editor Compile | Not run | UBT/UHT unavailable |
| Automation tests | Not run | Unreal unavailable |

## Assets

لا توجد أصول Unreal ثنائية فعلية في المستودع أو Sandbox. السكربت الجاهز للتشغيل داخل Unreal Editor هو:

```text
Scripts/Editor/CreatePrototypeAssets.py
```

تشغيله باستخدام Python النظام مرفوض عمدًا برسالة واضحة، ويجب تشغيله عبر UnrealEditor-Cmd بعد تثبيت Unreal والـEditor plugins. لذلك حالة Map وInput/Data Assets وBlueprints هي **GENERATOR READY / ASSET CREATION BLOCKED**.

## Android build result

| Target | Exit | Result |
|---|---:|---|
| Development APK | `2` | `ANDROID BUILD BLOCKED: Set UE_ROOT to the installed Unreal Engine 5.4 directory.` |
| Shipping APK | `2` | `ANDROID BUILD BLOCKED: Set UE_ROOT to the installed Unreal Engine 5.4 directory.` |
| APK path | — | No file produced |
| ABI | Configured `arm64-v8a` only; not verified from APK |
| Package | Configured `com.ashline.game`; not verified from APK |
| Min SDK | Configured `26`; not verified from APK |
| Version | Configured `0.0.1 (1)`; not verified from APK |
| Signature | Not available |
| APK size | Not available |
| SHA-256 | Not available |

## Device and performance

لم يتوفر جهاز Android أو `adb`. لذلك لم يتم تنفيذ Install أو Launch أو Logcat أو Runtime Smoke Test أو Touch Multi-Touch Test أو AI Combat Test أو Death/Restart Test أو Performance Test.

| Metric | Result |
|---|---|
| Android device | Not connected |
| Android version | Not measured |
| GPU | Not measured |
| FPS with 3 AI | Not measured |
| FPS with 6 AI | Not measured |
| Peak RAM | Not measured |
| Crash/ANR scan | Not run |

## Release gate

| Gate | Status |
|---|---|
| C++ Compile | BLOCKED — Unreal unavailable |
| UHT | BLOCKED — Unreal unavailable |
| Combat map | BLOCKED — generator ready, no `.umap` |
| Input | Source/generator ready, no `.uasset` |
| Player loadout | Source/generator ready, no runtime execution |
| Enemy loadout | Source/generator ready, no runtime execution |
| HUD | Source/generator ready, no widget execution |
| Touch | Source/generator ready, no device test |
| AI | Static validation PASS, runtime not tested |
| Damage | Static validation PASS, runtime not tested |
| Death | Static validation PASS, runtime not tested |
| Restart | Source ready, runtime not tested |
| Android Cook | BLOCKED |
| Android Package | BLOCKED |
| APK ARM64 | NOT PRODUCED |
| Install/Launch | NOT RUN |
| Size <=500 MB | NOT MEASURED |
| No blocker crash | NOT TESTED |

## Exact next commands on a Windows UE machine

```powershell
$env:UE_ROOT = "C:\Program Files\Epic Games\UE_5.4"
$env:ANDROID_HOME = "C:\Users\<user>\AppData\Local\Android\Sdk"
$env:ANDROID_NDK_HOME = "$env:ANDROID_HOME\ndk\<ue-compatible-version>"

UnrealEditor-Cmd.exe ASH_LINE.uproject `
  -run=pythonscript `
  -script=Scripts/Editor/CreatePrototypeAssets.py `
  -unattended -nop4

Scripts/Build/BuildEditor.ps1
Scripts/Build/BuildAndroidPrototype.ps1 -Configuration Development
adb devices
adb install -r Releases/Android/0.0.1/APK/AshLine_CombatPrototype_v0.0.1_android_arm64_development.apk
Scripts/Build/BuildAndroidPrototype.ps1 -Configuration Shipping
```

**FIRST PLAYABLE APK: NOT READY.**
