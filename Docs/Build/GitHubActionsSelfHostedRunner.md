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
