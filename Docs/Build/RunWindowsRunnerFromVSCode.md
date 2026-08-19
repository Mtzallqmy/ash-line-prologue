# تشغيل Windows Self-hosted Runner من Visual Studio Code

هذا الدليل مخصص لجهاز Windows فعلي مجهز بـ Unreal Engine 5.4.4 وأدوات Android. افتح Visual Studio Code بصفة **Administrator**، لأن سكربت التسجيل يتحقق من صلاحيات المسؤول وقد يثبت Runner كخدمة Windows.

## 1. فتح المشروع في VS Code

افتح قائمة Start، ابحث عن Visual Studio Code، اضغط بزر الفأرة الأيمن، واختر **Run as administrator**. من داخل VS Code افتح Terminal جديدًا، واختر **PowerShell**.

إذا لم يكن المشروع موجودًا على جهاز Windows، نفذ الأوامر التالية في Terminal:

```powershell
Set-ExecutionPolicy -Scope Process Bypass -Force

New-Item -ItemType Directory -Path C:\Projects -Force | Out-Null
Set-Location C:\Projects

git clone https://github.com/Mtzallqmy/ash-line-prologue.git
Set-Location C:\Projects\ash-line-prologue
```

إذا كان المشروع موجودًا مسبقًا، استخدم:

```powershell
Set-Location C:\Projects\ash-line-prologue
git pull origin master
```

يجب أن تكون نسخة السكربت الأخيرة من commit `f8b54ee` أو أحدث.

## 2. تحديد مسارات البيئة

عدّل القيم التالية حسب تثبيت جهازك. قيمة `UE_ROOT` هي المجلد الذي يحتوي مجلد `Engine` مباشرة، وليست مجلد `Engine` نفسه.

```powershell
$UE_ROOT = 'C:\Program Files\Epic Games\UE_5.4'
$ANDROID_HOME = 'C:\Android\Sdk'
$ANDROID_NDK_HOME = 'C:\Android\Sdk\ndk\25.2.9519653'
$JAVA_HOME = 'C:\Program Files\Eclipse Adoptium\jdk-21'
```

أمثلة بديلة شائعة:

```text
Unreal Engine: C:\Program Files\Epic Games\UE_5.4
Android SDK:   C:\Users\<WindowsUser>\AppData\Local\Android\Sdk
Android NDK:   C:\Users\<WindowsUser>\AppData\Local\Android\Sdk\ndk\25.2.9519653
Java/JDK:      C:\Program Files\Android\Android Studio\jbr
```

## 3. فحص الملفات المطلوبة قبل التسجيل

نفذ الأوامر التالية. يجب أن تظهر `True` لكل فحص:

```powershell
Test-Path "$UE_ROOT\Engine\Build\BatchFiles\RunUAT.bat"
Test-Path "$UE_ROOT\Engine\Binaries\Win64\UnrealEditor-Cmd.exe"
Test-Path "$ANDROID_HOME\platform-tools\adb.exe"
Test-Path "$ANDROID_HOME\platforms\android-34\android.jar"
Test-Path "$ANDROID_HOME\build-tools\34.0.0\aapt2.exe"
Test-Path "$ANDROID_NDK_HOME\source.properties"
Test-Path "$JAVA_HOME\bin\java.exe"
```

للعثور على UnrealBuildTool إذا لم تعرف مكانه:

```powershell
Get-ChildItem "$UE_ROOT\Engine\Binaries\DotNET" -Filter UnrealBuildTool.exe -Recurse -ErrorAction SilentlyContinue | Select-Object -First 5 FullName
```

لعرض إصدارات الأدوات:

```powershell
& "$JAVA_HOME\bin\java.exe" -version
& "$ANDROID_HOME\platform-tools\adb.exe" version
Get-Content "$ANDROID_NDK_HOME\source.properties"
```

إذا ظهر `False` في أي فحص، صحح المسار أو ثبت المكوّن الناقص قبل الانتقال إلى الخطوة التالية.

## 4. التأكد من تثبيت Android SDK

يجب أن يحتوي Android SDK على API 34 وBuild Tools 34.0.0. يمكن تثبيتها من Android Studio عبر **SDK Manager**، أو باستخدام `sdkmanager.bat` إذا كان Command-line Tools مثبتًا:

```powershell
$sdkmanager = "$ANDROID_HOME\cmdline-tools\latest\bin\sdkmanager.bat"
& $sdkmanager 'platform-tools' 'platforms;android-34' 'build-tools;34.0.0' 'ndk;25.2.9519653'
```

إذا لم يكن المسار `cmdline-tools\latest` موجودًا، استخدم Android Studio لإكمال التثبيت، ثم حدّد المسار الصحيح لـ SDK وNDK.

## 5. فتح صفحة Registration Token

في المتصفح افتح:

```text
https://github.com/Mtzallqmy/ash-line-prologue/settings/actions/runners/new
```

اختر **Windows** ثم **x64**. سيعرض GitHub Registration Token مؤقتًا. لا تضعه في المستودع أو ملف نصي أو المحادثة.

## 6. تشغيل سكربت التسجيل

من Terminal داخل مجلد المشروع نفذ:

```powershell
Unblock-File .\Scripts\Runner\SetupWindowsRunner.ps1

.\Scripts\Runner\SetupWindowsRunner.ps1 `
  -RunnerRoot 'C:\actions-runner' `
  -UnrealRoot $UE_ROOT `
  -AndroidHome $ANDROID_HOME `
  -AndroidNdkHome $ANDROID_NDK_HOME `
  -JavaHome $JAVA_HOME `
  -RunnerName "ashline-unreal-$env:COMPUTERNAME"
```

سيطلب السكربت Registration Token داخل Terminal. الصق التوكن عند الطلب واضغط Enter. سيقوم السكربت بتنزيل Runner الرسمي، وفحص Unreal وAndroid وJava، ثم تسجيل Runner بهذه Labels:

```text
self-hosted, Windows, X64, unreal-5.4, android
```

## 7. تشغيل Runner

عند التشغيل العادي سيطلب السكربت إبقاء النافذة مفتوحة. لا تغلق Terminal أثناء انتظار Workflow. إذا ظهرت الرسالة التي تطلب تشغيل `run.cmd`، نفذ:

```powershell
Set-Location C:\actions-runner
.\run.cmd
```

يمكن بدلًا من ذلك تسجيل Runner كخدمة Windows حتى يعمل بعد إعادة التشغيل:

```powershell
Set-Location C:\Projects\ash-line-prologue

.\Scripts\Runner\SetupWindowsRunner.ps1 `
  -RunnerRoot 'C:\actions-runner' `
  -UnrealRoot $UE_ROOT `
  -AndroidHome $ANDROID_HOME `
  -AndroidNdkHome $ANDROID_NDK_HOME `
  -JavaHome $JAVA_HOME `
  -RunnerName "ashline-unreal-$env:COMPUTERNAME" `
  -PersistEnvironment `
  -InstallAsService
```

تحقق من الخدمة:

```powershell
Get-Service | Where-Object { $_.Name -like 'actions.runner*' } | Format-Table Name,Status,StartType
```

يجب أن تكون الحالة `Running`.

## 8. التحقق من ظهور Runner في GitHub

من PowerShell الذي يحتوي GitHub CLI:

```powershell
gh api repos/Mtzallqmy/ash-line-prologue/actions/runners | ConvertFrom-Json | Select-Object total_count

gh api repos/Mtzallqmy/ash-line-prologue/actions/runners | ConvertFrom-Json | Select-Object -ExpandProperty runners | Format-List name,os,status,busy,labels
```

النتيجة المطلوبة هي Runner واحد على الأقل، بنظام `Windows` وحالة `online`، وبLabels تشمل:

```text
self-hosted
Windows
X64
unreal-5.4
android
```

## 9. متغيرات GitHub Actions

من GitHub افتح **Settings → Secrets and variables → Actions → Variables** وأضف القيم التالية:

| Variable | القيمة |
|---|---|
| `UE_ROOT` | نفس قيمة `$UE_ROOT` على Windows |
| `ANDROID_HOME` | نفس قيمة `$ANDROID_HOME` |
| `ANDROID_NDK_HOME` | نفس قيمة `$ANDROID_NDK_HOME` |
| `JAVA_HOME` | نفس قيمة `$JAVA_HOME` |

لا تضف كلمات مرور أو Private Keys إلى Variables. أسرار Shipping تضاف فقط في قسم **Secrets**:

```text
ANDROID_KEY_ALIAS
ANDROID_KEYSTORE_PASSWORD
ANDROID_KEY_PASSWORD
```

ويجب أن يكون ملف Keystore موجودًا على Runner في:

```text
$env:RUNNER_TEMP\ashline-release.keystore
```

## 10. متابعة Workflow وإنتاج APK

Run الموجود هو:

```text
https://github.com/Mtzallqmy/ash-line-prologue/actions/runs/32292788709
```

بعد ظهور Runner بحالة `online` سيبدأ الـRun الموجود في حالة `queued` تلقائيًا. لمراقبته من VS Code:

```powershell
gh run watch 32292788709 --repo Mtzallqmy/ash-line-prologue --interval 15
```

أو من GitHub CLI شغّل Run جديدًا من آخر master:

```powershell
gh workflow run build-android-apk.yml `
  --repo Mtzallqmy/ash-line-prologue `
  --ref master `
  -f configuration=Development
```

لمشاهدة آخر Runs:

```powershell
gh run list `
  --repo Mtzallqmy/ash-line-prologue `
  --workflow build-android-apk.yml `
  --limit 5
```

عند النجاح سيظهر Artifact باسم `ash-line-android-apk`، واسم APK الخاص بـDevelopment هو:

```text
AshLine_CombatPrototype_v0.0.1_android_arm64_development.apk
```

لن يعتبر البناء ناجحًا إلا بعد نجاح Compile وAsset Generation وCook وPackage ووجود APK حقيقي.

## أكثر الأخطاء شيوعًا

| الرسالة | الحل |
|---|---|
| `running scripts is disabled` | نفذ `Set-ExecutionPolicy -Scope Process Bypass -Force` |
| `UE_ROOT path not found` | مرر مجلد Unreal الذي يحتوي `Engine` مباشرة |
| `UnrealEditor-Cmd.exe not found` | صحح مسار UE 5.4.4 أو ثبّت مكونات Editor |
| `android.jar not found` | ثبت Android API 34 من SDK Manager |
| `aapt2.exe not found` | ثبت Build Tools 34.0.0 |
| `source.properties not found` | صحح مسار NDK إلى مجلد الإصدار نفسه |
| `Runner is offline` | شغّل `C:\actions-runner\run.cmd` أو شغّل خدمة Runner |
| `queued` مع Runner غير ظاهر | Runner لم يسجل بنجاح أو Labels غير مطابقة |
| `No runner matching labels` | راجع Labels حرفيًا: `self-hosted,Windows,X64,unreal-5.4,android` |

لا تغلق VS Code أو Windows أثناء تنفيذ BuildCookRun، ولا تحذف مجلد `C:\actions-runner` بعد التسجيل.
