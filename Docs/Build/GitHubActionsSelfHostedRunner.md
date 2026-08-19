# GitHub Actions Android APK Build

أُضيف Workflow البناء في:

```text
.github/workflows/build-android-apk.yml
```

ويعمل يدويًا عبر `workflow_dispatch` على Runner Windows self-hosted مجهز بـ Unreal Engine 5.4.4 وAndroid SDK/NDK وJDK. يوقف التنفيذ عند فشل التحقق من Unreal أو Android أو Compile أو توليد الخريطة أو BuildCookRun أو غياب APK حقيقي.

## Runner labels

يجب أن يحمل الـRunner جميع Labels التالية حرفيًا:

| Label | الاستخدام |
|---|---|
| `self-hosted` | اختيار Runner مستضاف ذاتيًا |
| `Windows` | نظام التشغيل المطلوب |
| `X64` | بنية الجهاز |
| `unreal-5.4` | وجود Unreal Engine 5.4.x |
| `android` | توفر Android toolchain |

صيغة التسجيل المتوقعة للـRunner هي:

```text
self-hosted, Windows, X64, unreal-5.4, android
```

## Repository Variables

أضف المتغيرات التالية من **Settings → Secrets and variables → Actions → Variables**:

| Variable | القيمة المطلوبة |
|---|---|
| `UE_ROOT` | جذر Unreal Engine 5.4.4، مثل `C:\Unreal\UE_5.4.4` |
| `ANDROID_HOME` | جذر Android SDK |
| `ANDROID_NDK_HOME` | مجلد NDK المتوافق مع UE 5.4، مثل `C:\Android\Sdk\ndk\25.1.8937393` |
| `JAVA_HOME` | JDK/JBR الذي يقبله تثبيت Unreal/Android على الـRunner |

يجب أن توجد داخل `UE_ROOT` الملفات التالية:

```text
Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe
Engine\Binaries\Win64\UnrealEditor.exe
Engine\Binaries\Win64\UnrealEditor-Cmd.exe
Engine\Build\BatchFiles\RunUAT.bat
```

ويجب أن يحتوي Android SDK على `platforms\android-34` وBuild Tools 34، بينما يجب أن يحتوي NDK على `source.properties`.

## Shipping secrets

أضف Secrets التالية من **Settings → Secrets and variables → Actions → Secrets**:

| Secret | الاستخدام |
|---|---|
| `ANDROID_KEY_ALIAS` | اسم المفتاح داخل Keystore |
| `ANDROID_KEYSTORE_PASSWORD` | كلمة مرور Keystore |
| `ANDROID_KEY_PASSWORD` | كلمة مرور المفتاح |

لا تُحفظ كلمات المرور أو Keystore في المستودع. يجب حفظ Keystore بأمان على الـRunner في المسار:

```text
$env:RUNNER_TEMP\ashline-release.keystore
```

يستخدم Workflow هذه الأسرار فقط في خطوة Shipping. Development لا يقرأ أسرار التوقيع.

## What the workflow does

ينفذ Workflow Checkout مع Git LFS، ثم يتحقق من UnrealBuildTool وRunUAT وUnrealEditor-Cmd وUnrealEditor، ويتحقق من Android SDK وNDK وJava. بعد ذلك يشغّل `ValidateBeforeBuild.ps1 -RequireUnreal`، ويبني `ASH_LINEEditor Win64 Development`، ويشغل `CreatePrototypeAssets.py` عبر `UnrealEditor-Cmd`، ويتأكد من وجود `L_CombatPrototype.umap`.

بعد نجاح هذه المراحل يشغل `BuildAndroidPrototype.ps1` بالـConfiguration المختارة. ثم يبحث عن APK حقيقي، ويتحقق من الحجم، ويحسب SHA-256، ويشغّل `verify_android_release.py` للتحقق من Package وABI وMin SDK، ثم يرفع APK في Artifact باسم `ash-line-android-apk` والتقارير في Artifact مستقل باسم `ash-line-android-reports`.

## تشغيل Workflow من GitHub

من صفحة المستودع افتح **Actions**، ثم اختر **ASH LINE Android ARM64 APK**، واضغط **Run workflow**، واختر الفرع `release/combat-prototype-v0.0.1`، ثم اختر `Development` أو `Shipping` واضغط **Run workflow**. مدة التنفيذ القصوى مضبوطة على 180 دقيقة.

يمكن تشغيله أيضًا من GitHub CLI بعد التأكد من إعداد الـRunner والمتغيرات:

```bash
gh workflow run build-android-apk.yml \
  --repo Mtzallqmy/ash-line-prologue \
  --ref release/combat-prototype-v0.0.1 \
  -f configuration=Development
```

ولـShipping:

```bash
gh workflow run build-android-apk.yml \
  --repo Mtzallqmy/ash-line-prologue \
  --ref release/combat-prototype-v0.0.1 \
  -f configuration=Shipping
```

بعد نجاح Development سيظهر APK ضمن Artifact باسم `ash-line-android-apk` بالاسم:

```text
AshLine_CombatPrototype_v0.0.1_android_arm64_development.apk
```

وبعد نجاح Shipping:

```text
AshLine_CombatPrototype_v0.0.1_android_arm64.apk
```

لا يعني إنشاء Workflow أن APK أصبح موجودًا فورًا؛ يتطلب النجاح Runner متصلًا بهذه Labels، وUnreal Engine، وAndroid toolchain، وKeystore/Secrets عند اختيار Shipping.

## References

[1]: https://docs.github.com/en/actions/using-workflows/events-that-trigger-workflows#workflow_dispatch "GitHub Docs — workflow_dispatch"
[2]: https://docs.github.com/en/actions/hosting-your-own-runners/managing-self-hosted-runners/about-self-hosted-runners "GitHub Docs — Self-hosted runners"
[3]: https://docs.github.com/en/actions/using-workflows/storing-workflow-data-as-artifacts "GitHub Docs — Storing workflow data as artifacts"

## إنشاء Runner على جهاز Windows

تمت إضافة سكربت الإعداد التالي:

```text
Scripts/Runner/SetupWindowsRunner.ps1
```

يجب تشغيله على جهاز Windows فعلي مجهز مسبقًا بـ Unreal Engine 5.4.4 وAndroid SDK/NDK وJDK. افتح **Windows PowerShell بصلاحية Administrator**، ثم نفذ:

```powershell
Set-ExecutionPolicy -Scope Process Bypass -Force
Set-Location C:\
$env:GITHUB_RUNNER_REGISTRATION_TOKEN = '<REGISTRATION_TOKEN_FROM_GITHUB>'
& 'C:\path\to\ash-line-prologue\Scripts\Runner\SetupWindowsRunner.ps1' `
  -RunnerRoot 'C:\actions-runner' `
  -UnrealRoot 'C:\Unreal\UE_5.4.4' `
  -AndroidHome 'C:\Android\Sdk' `
  -AndroidNdkHome 'C:\Android\Sdk\ndk\25.2.9519653' `
  -JavaHome 'C:\Program Files\Eclipse Adoptium\jdk-21' `
  -RunnerName "ashline-unreal-$env:COMPUTERNAME"
```

السكربت ينزل أحدث Windows x64 Runner الرسمي، يفحص الملفات الأساسية، يسجل الـRunner بالـLabels المطلوبة، ثم يشغله عبر `run.cmd`. يمكن تشغيله كخدمة Windows بدل النافذة التفاعلية باستخدام `-InstallAsService`، ويمكن حفظ مسارات الأدوات كمتغيرات Machine باستخدام `-PersistEnvironment`.

يصدر GitHub Registration Token مؤقتًا من صفحة **Settings → Actions → Runners → New self-hosted runner**، أو عبر GitHub CLI بصلاحية مناسبة:

```bash
gh api -X POST repos/Mtzallqmy/ash-line-prologue/actions/runners/registration-token
```

لا تضع قيمة التوكن داخل Git أو داخل أي ملف في المستودع. التوكن مؤقت ويُمرر إلى PowerShell من خلال متغير البيئة أو يُدخل عند طلب السكربت. بعد التسجيل، تحقق من ظهور Runner في:

```text
https://github.com/Mtzallqmy/ash-line-prologue/settings/actions/runners
```

ويجب أن يظهر بحالة `Idle` وبالـLabels التالية:

```text
self-hosted, Windows, X64, unreal-5.4, android
```

بعد اتصاله سيستأنف GitHub Run الموجود في حالة `queued` تلقائيًا. إذا انتهت صلاحية التوكن، أصدر توكنًا جديدًا بدل إعادة استخدام القديم. لا يمكن تنفيذ التسجيل الفعلي من بيئة Linux الحالية لأن المطلوب Runner Windows يحمل Unreal Engine 5.4.4؛ هذه البيئة تستطيع إصدار التوكن وتجهيز السكربت فقط.
