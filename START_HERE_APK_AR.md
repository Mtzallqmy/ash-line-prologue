# ابدأ من هنا — إنتاج APK لـ ASH LINE

تمت معالجة المشروع لتقليل أسباب فشل إنتاج APK.

## أسرع طريقة

على جهاز Windows أو Windows Cloud VM يحتوي **Unreal Engine 5.4.4+**:

```text
BUILD_APK.cmd
```

هذا الأمر ينفذ تلقائيًا:

```text
اكتشاف Unreal/Android/JDK
→ فحص المشروع
→ Compile للـEditor
→ إنشاء L_CombatPrototype وData Assets
→ ضبط الخريطة وBlueprint GameMode
→ Build + Cook + Stage + PACKAGE
→ استخراج APK
→ فحص ARM64 / Package / SDK / الحجم / SHA-256
```

الناتج:

```text
Releases/Android/0.0.1/APK/
AshLine_CombatPrototype_v0.0.1_android_arm64_development.apk
```

## المتطلبات التي يرفض البناء بدونها

```text
Unreal Engine 5.4.4 أو أحدث من 5.4
Android SDK API 34
Build Tools 34.0.0
NDK r25b = 25.1.8937393
JDK 17
Python 3 (أو Python المدمج في Unreal إذا توفر)
```

Android المستهدف:

```text
ARM64-v8a فقط
Android 8.0+ / minSdk 26
Package: com.ashline.game
```

## GitHub Actions

تم تجهيز:

```text
.github/workflows/build-android-apk.yml
```

وهو يعمل على Windows **self-hosted runner** يحمل Labels:

```text
self-hosted, Windows, X64, unreal-5.4, android
```

من GitHub:

```text
Actions
→ ASH LINE Android ARM64 APK
→ Run workflow
→ Development
```

بعد النجاح حمّل Artifact:

```text
ash-line-android-apk-Development
```

## أهم إصلاح تقني

مسار UAT القديم كان يستخدم:

```text
-build -cook -stage -pak -archive
```

والآن يستخدم أيضًا:

```text
-package
```

ليطلب من `BuildCookRun` إنتاج حزمة Android/APK فعلية بدل الاكتفاء بالـStage/Archive.

## إذا ظهر UNREAL BUILD ENVIRONMENT NOT AVAILABLE

هذا يعني أن الجهاز/Runner الذي ينفذ البناء **لا يحتوي Unreal Engine**. لا يمكن معالجة هذه النقطة من Android SDK وحده. استخدم Windows PC أو Windows Cloud VM وثبّت UE 5.4.4 عليه مرة واحدة، ثم سجله كـGitHub self-hosted runner.

للتفاصيل:

```text
Docs/Build/APKBuildSolution.md
```
