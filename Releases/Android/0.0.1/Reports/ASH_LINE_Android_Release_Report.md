# ASH LINE ANDROID RELEASE REPORT

## الحالة الحالية

> **APK READY: NO** — لم يتم إنشاء APK؛ عملية البناء توقفت قبل Compile لأن Unreal Engine 5.4 غير مثبت أو غير محدد عبر `UE_ROOT`.

> **AAB BUILD READY: NO** — نفس مانع البناء ينطبق على AAB.

تم تنفيذ فحوصات المشروع وContent System وPrompt 02–05 بنجاح، وتم تجهيز إعدادات Android وسكربت Build رسمي، لكن لا يجوز إعلان APK قابل للتثبيت دون Unreal Cook/Package فعلي والتحقق على Android.

| الحقل | القيمة |
|---|---|
| Unreal Engine Version | مطلوب UE 5.4؛ غير متوفر في البيئة الحالية |
| Build Configuration | Development وShipping ARM64 مخططان |
| Package Name | `com.ashline.game` |
| Version Name | `0.0.1` |
| Version Code | `1` |
| ABI | `arm64-v8a` فقط |
| Minimum SDK | `26` |
| Target SDK | `34` مهيأ، ويجب مطابقته مع UE 5.4 Toolchain الفعلي |
| Compile SDK | غير متاح قبل Unreal Android Toolchain |
| NDK Version | غير متاح؛ `ANDROID_NDK_HOME`/`NDK_HOME` غير مضبوط |
| Build Tools | غير متاحة؛ Android SDK غير مضبوط |
| JDK | Java 21 موجود، لكن توافقه مع UE 5.4 لم يُتحقق بسبب غياب Unreal |
| Renderer | OpenGL ES compatibility path؛ Vulkan اختياري/معطل كمسار إلزامي |
| Texture Format | يحتاج اختيارًا نهائيًا داخل UE حسب الأجهزة؛ لا توجد أصول Cooked بعد |
| APK Filename | `AshLine_CombatPrototype_v0.0.1_android_arm64.apk` عند نجاح Shipping Build |
| APK Size | غير متاح |
| Installed Size | غير متاح |
| SHA-256 | غير متاح |
| Permissions | لا يمكن استخراجها قبل APK؛ لا توجد Permissions Gameplay إضافية مضافة عمدًا |
| Signing Status | غير موقّع؛ Shipping يتطلب Keystore خارجيًا عبر Environment Variables |
| Maps Included | لم يتم Cook؛ الخريطة الفعلية `.umap` غير موجودة في Sandbox |
| Plugins Included | EnhancedInput وAIModule وGameplayTasks وUMG مفعلة في `.uproject` |
| Top 30 Assets by Size | غير متاح قبل Cook |
| RAM Result | غير مقاس |
| FPS Results | غير مقاسة |
| AI Stress Test | Static validation فقط؛ لا يوجد Runtime |
| Android Versions Tested | لا شيء |
| Devices/GPUs Tested | لا شيء |
| Bugs | Build blocked by missing Unreal/Android Toolchain |
| Release Blockers | Unreal Engine 5.4، SDK، NDK، Android Build Tools، Android File Picker Runtime، `.uasset`/`.umap` Cook |

## ما تم تجهيزه

تم ضبط `PackageName=com.ashline.game` و`MinSDKVersion=26` و`TargetSDKVersion=34` و`ARM64=True` وARMv7/x86/x86_64=False، مع Landscape وFullscreen وOpenGL ES compatibility flags. تمت إضافة `ProjectVersion=0.0.1` و`VersionDisplayName=0.0.1` و`StoreVersion=1`.

تم إنشاء `Scripts/Build/BuildAndroidRelease.sh` ليوقف البناء برسالة واضحة إذا كان `UE_ROOT` أو Android SDK/NDK أو Keystore ناقصًا. عند توفر البيئة سيبني Development وShipping، ويسمي Shipping APK بالاسم المطلوب، ويضع APK وSymbols وReports وChecksums في Release Folder.

تم إنشاء `Scripts/Validation/verify_android_release.py` لفحص `arm64-v8a` وغياب ABI غير المطلوبة والتوقيع والحجم وSHA-256 وManifest عندما تتوفر `aapt` و`apksigner`.

## نتيجة Build Gate

تم تشغيل:

```bash
./Scripts/Build/BuildAndroidRelease.sh Development
```

والنتيجة:

```text
ANDROID BUILD BLOCKED: Set UE_ROOT to the installed Unreal Engine 5.4 directory.
```

هذه نتيجة مانع بيئي حقيقي وليست فشلًا في إعدادات المشروع. لا يمكن إصلاح غياب Unreal Engine بتوليد ملف APK وهمي أو بتجميع C++ خارج Unreal.

## طريقة الاستئناف على جهاز Build

بعد تثبيت Unreal Engine 5.4 وAndroid SDK/NDK/JDK المتوافق، يجب ضبط `UE_ROOT` و`ANDROID_HOME` و`ANDROID_NDK_HOME` ثم تشغيل:

```bash
export UE_ROOT=/path/to/UnrealEngine
export ANDROID_HOME=/path/to/Android/Sdk
export ANDROID_NDK_HOME=/path/to/Android/Sdk/ndk/<version>
./Scripts/Build/BuildAndroidRelease.sh Development
```

ولـ Shipping يجب توفير Keystore خارج Git:

```bash
export ANDROID_KEYSTORE=/secure/path/ashline-release.keystore
export ANDROID_KEY_ALIAS=ashline
export ANDROID_KEYSTORE_PASSWORD='provided-by-secure-secret-store'
export ANDROID_KEY_PASSWORD='provided-by-secure-secret-store'
./Scripts/Build/BuildAndroidRelease.sh Shipping
```

لا تضع كلمات المرور أو Keystore في Repository.
