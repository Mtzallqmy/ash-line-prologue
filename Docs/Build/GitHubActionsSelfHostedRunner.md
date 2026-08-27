# GitHub Actions Android APK Build — ASH LINE

Workflow:

```text
.github/workflows/build-android-apk.yml
```

يعمل يدويًا عبر `workflow_dispatch` على Windows self-hosted runner. الـRunner يجب أن يحتوي Unreal Engine 5.4.4+ وAndroid toolchain؛ السكربت `ResolveBuildEnvironment.ps1` يكتشف المسارات تلقائيًا أو يستخدم Repository Variables كـhints.

## Labels

```text
self-hosted
Windows
X64
unreal-5.4
android
```

## الأدوات المطلوبة

```text
UE 5.4.4+
Android SDK API 34
Build Tools 34.0.0
NDK 25.1.8937393 (r25b)
JDK 17
Git + Git LFS
```

UE 5.4 الأساسي لا يجب بناؤه باستخدام Java 21 أو NDK r25c في هذا pipeline؛ استخدم JDK 17 وr25b.

## Variables اختيارية

```text
UE_ROOT
ANDROID_HOME
ANDROID_NDK_HOME
JAVA_HOME
```

إذا كانت فارغة، يتم الاكتشاف من Environment Variables الخاصة بالجهاز، Epic Launcher manifest، والمسارات الشائعة.

## Development

من GitHub:

```text
Actions → ASH LINE Android ARM64 APK → Run workflow → Development
```

Development هو المسار الموصى به لأول APK تجريبي ولا يحتاج Release Keystore.

## Shipping Secrets

```text
ANDROID_KEYSTORE_B64
ANDROID_KEY_ALIAS
ANDROID_KEYSTORE_PASSWORD
ANDROID_KEY_PASSWORD
```

الـWorkflow يفك `ANDROID_KEYSTORE_B64` داخل `RUNNER_TEMP` وقت المهمة فقط، ثم Build script ينسخه مؤقتًا إلى `Build/Android` لأن Unreal يتوقع Keystore التوزيع في هذا المسار. `Build/` موجود في `.gitignore`.

## أمر البناء المباشر على جهاز الـRunner

```powershell
.\Scripts\Build\BuildFirstAPK.ps1 -Configuration Development
```

الناتج:

```text
Releases/Android/0.0.1/APK/AshLine_CombatPrototype_v0.0.1_android_arm64_development.apk
```

لـShipping بعد ضبط الأسرار/Environment Variables:

```powershell
.\Scripts\Build\BuildFirstAPK.ps1 -Configuration Shipping
```

الناتج:

```text
Releases/Android/0.0.1/APK/AshLine_CombatPrototype_v0.0.1_android_arm64.apk
```

## ملاحظة مهمة

الـWorkflow لا يستخدم GitHub-hosted Windows runner لأن Unreal Engine غير مثبت عليه افتراضيًا. يجب توصيل self-hosted runner فعلي يحتوي المحرك. المشكلة `UNREAL BUILD ENVIRONMENT NOT AVAILABLE` تعني أن Runner نفسه لا يحتوي Unreal أو أن نسخة المحرك أقدم من 5.4.4.
