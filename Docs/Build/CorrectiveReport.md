# ASH LINE CORRECTIVE REPORT

## الحالة العامة

تم تنفيذ مهمة تثبيت قاعدة ASH LINE على الفرع `fix/playable-v0.0.1` مع الحفاظ على Modules العشرة والأنظمة القائمة، ودون إضافة مدينة كاملة أو Drone جديد أو Multiplayer أو أصول Marketplace ضخمة.

> **Ready for Combat Demo: NO** — المصدر وفحوصات السطح جاهزة للتجربة داخل Unreal، لكن لا يمكن إعلان جاهزية Combat Demo قبل إنشاء الأصول الفعلية، وفتح المشروع داخل Unreal، ونجاح UnrealHeaderTool وEditor Compile وRuntime Smoke Test.

## 1. Initial blockers

كان ملف `ASH_LINE.uproject` يعامل Engine Modules مثل `AIModule` و`GameplayTasks` و`UMG` كأنها Plugins. تم حذف هذه الإدخالات والإبقاء على `EnhancedInput` كـPlugin فعلي، مع ترك Engine Dependencies في ملفات `Build.cs`.

كان `DefaultEngine.ini` يشير إلى خريطة غير موجودة هي `L_Namar_Prologue`. تم استبدالها مؤقتًا بـ`/Engine/Maps/Templates/Minimal_Default` الآمنة لفتح المشروع والتجميع، مع توثيق أن الخريطة النهائية `L_CombatPrototype.umap` يجب إنشاؤها داخل Unreal Editor.

لم تكن بيئة Unreal Engine أو Android SDK/NDK متاحة في Sandbox، ولذلك لم يتم الادعاء بنجاح Compile أو Cook أو Package.

## 2. Files modified

تم تعديل ملفات UProject وConfig وManifest، وملفات Player وGameMode، وWeapon Base وWeapon Component، وAI Controller وAI Combat Component، وContent Manager وPackage Validator وPackage Mount Manager وLocal Content Delivery Service، إضافة إلى `validate_prompt03.py` و`BuildAndroidRelease.sh`.

## 3. Files added

| المسار | الغرض |
|---|---|
| `Docs/Build/PreFixAudit.md` | نتائج التدقيق قبل الإصلاح |
| `Docs/Build/CorrectiveReport.md` | هذا التقرير |
| `Source/AshLineContent/Public/ALSemanticVersion.h` | Semantic Version helper |
| `Source/AshLineContent/Private/ALSemanticVersion.cpp` | المقارنة الرقمية Major/Minor/Patch |
| `Source/AshLineCombat/Private/Tests/ALFoundationTests.cpp` | اختبارات Health/Damage المصدرية |
| `Source/AshLineContent/Private/Tests/ALContentTests.cpp` | اختبار Semantic Version |
| `Scripts/Editor/CreatePrototypeAssets.py` | أداة Unreal Editor Python لإنشاء Map وData Assets لاحقًا |
| `Scripts/Validation/validate_build_references.py` | فحص Maps وModules وDependencies وIDs وConfig |
| `Scripts/Build/ValidateBeforeBuild.sh` | بوابة التحقق قبل البناء على Linux |
| `Scripts/Build/BuildEditor.sh` | Editor target wrapper |
| `Scripts/Build/BuildDevelopment.sh` | Linux Development wrapper |
| `Scripts/Build/BuildAndroidDevelopment.sh` | Android Development wrapper |
| `Scripts/Build/BuildAndroidShipping.sh` | Android Shipping wrapper |
| `Scripts/Build/ValidateBeforeBuild.ps1` | Windows validation wrapper |
| `Scripts/Build/BuildAndroidDevelopment.ps1` | Windows Android Development wrapper |
| `Scripts/Build/BuildAndroidShipping.ps1` | Windows Android Shipping wrapper |

## 4. Build.cs fixes

تمت مراجعة اتجاه الاعتماد بين الوحدات. بقي `AshLineCombat` عامًا ولا يعتمد على `AshLineWeapons` أو `AshLineCharacters`. تعتمد Characters على Combat وWeapons، وتعتمد AI على Combat وCharacters وWeapons، بينما تبقى Content معزولة عن Gameplay implementation. لم يتم إدخال اعتماد دائري في ملفات `Build.cs`.

تم الحفاظ على Dependencies المطلوبة مثل `AIModule` و`GameplayTasks` و`NavigationSystem` و`EnhancedInput` و`UMG` داخل ملفات Modules الصحيحة بدل وضعها في `.uproject` كPlugins.

## 5. UProject fixes

أصبح قسم Plugins يحتوي فقط على `EnhancedInput`. بقيت Modules العشرة كما هي، وبقيت Target Platforms هي Android وWindows، مع إعدادات Target وEditor Target الحالية المتوافقة مع Unreal Engine 5.4.

## 6. Player fixes

تم تعديل Death Flow بحيث لا يعيد `AALPlayerCharacter` الصحة ويفتح التحكم مباشرة بعد الموت. أصبح التسلسل:

```text
Player dies
→ Player disabled
→ OnRestartCheckpointRequested
→ AALPrologueGameMode chooses checkpoint/current prototype transform
→ Player moves and stops
→ Health resets
→ Controls and weapon input enable
```

أضيفت `ApplyCheckpointRestart`، وأصبح GameMode يملك اختيار Checkpoint اختياريًا عبر `bUseConfiguredCheckpoint` و`CheckpointTransform`. عند عدم وجود Checkpoint حقيقي، يستخدم Prototype-safe transform الحالي بدل اختراع مدير Checkpoint كامل.

تم أيضًا جعل ربط Death delegate فريدًا باستخدام `AddUniqueDynamic`، مع الحفاظ على Movement وLook وSprint وCrouch وJump وInteraction وWeapon input hooks.

## 7. Weapon fixes

تم الإبقاء على التصميم Data-Driven عبر `AALWeaponBase` و`UALWeaponComponent` و`UALWeaponDataAsset`. تمت إضافة Cache transient للصوت والمؤثر، وأصبح `LoadSynchronous()` للأصول السمعية والبصرية يحدث أثناء التهيئة أو التجهيز لا أثناء كل Shot.

تم منع تكرار Weapon delegates باستخدام `AddUniqueDynamic`، وإلغاء ربط السلاح السابق قبل التبديل، مع استمرار StopFire وCancelReload وStopAim عند تعطيل إدخال السلاح أو موت المالك.

## 8. AI fixes

أصبح `UALAICombatComponent::StartCombat` idempotent. إذا كان القتال نشطًا على نفس الهدف، فلا يعيد Reaction Timer أو Decision Timer أو تهيئة Engagement.

تم جعل `ConfigurePerception` يعيد ضبط قيم Sense عند الحاجة دون تسجيل `OnTargetPerceptionUpdated` أكثر من مرة، باستخدام `bPerceptionBound` و`AddUniqueDynamic`. بقيت بنية Timed Decision Loop دون Actor Tick دائم، مع استمرار Patrol وSuspicious وAlert وCombat وSearch وReturning وDead.

## 9. Content fixes

تمت إضافة `FALSemanticVersion` لمنع المقارنة النصية الخاطئة مثل اعتبار `0.10.0` أقل من `0.9.0`.

تم إصلاح انتقال Package State ليقبل transient records عند `Installing` و`Verifying` وDownload states قبل وجود Record نهائي في Registry.

أضيف حساب SHA-256 فعلي لمجلد Development package بطريقة deterministic تعتمد على المسارات النسبية ومحتوى الملفات، ثم تتم مقارنة digest الفعلي مع Hash الموجود في Manifest عند توفره.

تم توضيح أن التوقيع الحالي ليس تحققًا تشفيريًا. في Shipping يتم منع unsigned external directory packages، كما يتم حظر المسار الخارجي إلى أن تتم إضافة Public-Key signature verification حقيقية.

تم فصل أسماء الاستيراد إلى `ImportDevelopmentPackageDirectory` و`ImportPackageFile`. الأخير Stub صريح يعيد الفشل بدل الادعاء بأن Directory هو `.alpack` container.

تمت تسمية Backend الحالي `UALDevelopmentDirectoryPackageMountBackend`، مع الإبقاء على `IALPackageMountBackend` كواجهة مستقبلية لـPak وIoStore. رسائل Runtime توضح أن Directory backend لا ينفذ Pak أو IoStore mount فعليًا.

تمت إضافة `DirectoriesToAlwaysStageAsNonUFS` لملفات `Content/AshLine/Data` حتى لا يعتمد Runtime Android على وجود Source JSON الخام داخل Content بعد Packaging.

## 10. Security fixes

لا يوجد Private Key أو Credential أو Keystore داخل المستودع. Shipping scripts تتطلب `ANDROID_KEYSTORE` وAlias وكلمات المرور من Environment Variables آمنة. لا يدّعي النظام Signature validation حقيقية قبل توفر implementation بالمفتاح العام.

## 11. Android config

| الإعداد | القيمة |
|---|---|
| Package Name | `com.ashline.game` |
| Version | `0.0.1` |
| Version Code | `1` |
| ABI | `arm64-v8a` فقط |
| Minimum SDK | `26` |
| Target SDK | `34` مهيأ ويجب مطابقته مع UE Toolchain الفعلي |
| Orientation | Landscape |
| Renderer | OpenGL ES compatibility path |
| Vulkan | غير مطلوب كمسار إلزامي |

## 12. Tests

نجحت الفحوصات التالية دون أخطاء:

```text
validate_project.py              PASS
validate_content_system.py       PASS
validate_prompt02.py             PASS
validate_prompt03.py             PASS
validate_prompt04.py             PASS
validate_prompt05.py             PASS
validate_build_references.py      PASS
static_surface_check.py          PASS
Python syntax compilation        PASS
Bash syntax checks               PASS
git diff --check                 PASS
```

يعرض `validate_build_references.py` تحذيرين فقط لأن الخريطة المؤقتة `/Engine/Maps/Templates/Minimal_Default` يجب التحقق من وجودها داخل تثبيت Unreal الفعلي.

أضيفت Automation tests مصدرية لـHealth/Damage وSemantic Version، لكنها لم تُشغّل لأن Unreal Automation framework غير متوفر في البيئة الحالية.

## 13. Editor compile result

```text
UNREAL BUILD ENVIRONMENT NOT AVAILABLE
```

تم تشغيل بوابة التحقق قبل البناء بنجاح. أما `BuildEditor.sh` و`BuildDevelopment.sh` فتوقفا برسالة واضحة لأن `UE_ROOT` غير مضبوط ولا يوجد UnrealBuildTool. كما توقفت Android Development وShipping قبل UAT للسبب نفسه.

لم يتم تنفيذ Generate Project Files أو UnrealHeaderTool أو Editor Compile أو Cook أو Package أو Install على جهاز Android.

## 14. Remaining blockers

الموانع المتبقية هي تثبيت Unreal Engine 5.4+، وتحديد `UE_ROOT`، وتثبيت Android SDK وNDK وBuild Tools، وإنشاء الأصول الفعلية داخل Unreal Editor، وإنشاء `L_CombatPrototype.umap` عبر `Scripts/Editor/CreatePrototypeAssets.py`، ثم تشغيل Editor Compile وAutomation tests وRuntime smoke test.

تحتاج المرحلة اللاحقة إلى ضبط `GameDefaultMap` على `/Game/AshLine/Maps/Namar/L_CombatPrototype` بعد إنشاء الخريطة، ثم إنشاء Blueprint subclasses وInput Assets وWeapon Data Assets وAI Data Assets المطلوبة فعليًا. كما يجب تنفيذ Public-Key signature verification قبل السماح بحزم خارجية موقعة في Shipping.

## 15. Ready for Combat Demo

**NO.** السبب ليس فشلًا في Python validation أو خللًا معروفًا في Architecture الحالية، بل غياب Unreal/Android build environment والأصول Unreal الفعلية واختبارات Runtime. لا يجوز تغيير الحالة إلى YES قبل نجاح Compile وCook وفتح الخريطة وتشغيل حركة اللاعب وإطلاق النار والضرر والموت وإعادة التشغيل والـAI داخل Unreal.

## Commits

تم إنشاء commits صغيرة وواضحة على الفرع `fix/playable-v0.0.1`:

| Commit | الوصف |
|---|---|
| `d3bcf3c` | تصحيح UProject وتسجيل Baseline Audit |
| `d0bbfd3` | تثبيت Player وWeapon وAI وMap references |
| `f384ea4` | إضافة Content integrity وBuild tooling وTests |
| `2d22f3a` | تحديث Prompt 03 validator لـUnique death binding |
