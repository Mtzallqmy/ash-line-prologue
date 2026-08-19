# ASH LINE — Pre-Fix Audit

## Scope

تم تنفيذ هذا التدقيق على الفرع `fix/playable-v0.0.1` قبل تعديل المصدر، مع الحفاظ على Modules الحالية وعدم إعادة إنشاء المشروع.

## Compile Blockers

المشروع يعرّف عشرة Runtime Modules صحيحة، لكن `ASH_LINE.uproject` يحتوي Entries باسم `AIModule` و`GameplayTasks` و`UMG` داخل قسم Plugins. هذه أسماء Engine Modules وليست Plugins فعلية، ولذلك يجب حذفها من `.uproject` والإبقاء على Dependencies داخل `Build.cs` فقط. إضافة إلى ذلك، لا يمكن إجراء UBT/UnrealHeaderTool في بيئة التدقيق لأن Unreal Engine 5.4 غير مثبت ولا يوجد `UE_ROOT`.

يجب مراجعة الـBuild.cs مع الـincludes الفعلية، خصوصًا في Characters وWeapons وAI وContent. لا تظهر دورة اعتماد مباشرة في التصور الحالي، لكن بعض الـPublic Dependencies أوسع من اللازم وقد تجعل اتجاه الاعتماد هشًا. يجب ألا يعتمد Combat على Weapon implementation، ويجب أن يبقى Health/Damage عامًا.

## Missing Assets

مجلد `Content/` يحتوي ملفات توثيق وJSON فقط، ولا يحتوي `.uasset` أو `.umap` أو Blueprint assets. لذلك لا توجد أصول فعلية لـInput Mapping أو Weapon Data Assets أو Mission Data Assets أو Player/GameMode Blueprints.

## Invalid References

`DefaultEngine.ini` و`content_manifest.json` يشيران إلى `/Game/AshLine/Maps/Namar/L_Namar_Prologue`، لكن الملف غير موجود كـ`.umap`. كما تشير Primary Asset scans إلى مسارات Data غير محتوية على أصول Unreal فعلية. هذه الإشارات يجب ألا تبقى كمرجع Default Map مفقود.

## Missing Maps

الخريطة الوحيدة الموصوفة هي design/readme content تحت `Content/AshLine/Maps/Namar/`; لا توجد خريطة Unreal قابلة للفتح. يجب إنشاء `L_CombatPrototype.umap` لاحقًا داخل Unreal Editor أو استخدام Map فعلية فقط أثناء تصحيح Compile، مع عدم الادعاء أن الخريطة أُنشئت في Sandbox.

## Missing Scripts

يوجد Build Android وLinux وبعض Python validators، لكن Prompt الإصلاح يتطلب scripts مستقلة باسم `BuildEditor` و`BuildDevelopment` و`BuildAndroidDevelopment` و`BuildAndroidShipping` و`ValidateBeforeBuild`، إضافة إلى Windows PowerShell equivalents و`Scripts/Editor/CreatePrototypeAssets.py` و`validate_build_references.py`.

## Unsafe Runtime Assumptions

`UALContentManagerSubsystem::LoadGlobalManifest` يقرأ ملف JSON الخام من `ProjectContentDir()`، وهو افتراض غير آمن بعد Packaging Android ما لم يتم Staging للملف كـNon-UFS أو تحويله إلى Config/Data Asset. `SetPackageState` يفشل إذا لم يوجد Registry record، بينما `InstallPackage` يستدعيه على Package مستورد حديثًا قبل إنشاء Record. `VerifyPackage` يتحقق حاليًا من Metadata فقط ولا يحسب SHA-256 للملفات. كما أن import الحالي يقبل Directory لكنه يسمي الواجهة العامة `ImportPackage` وكأنه يقبل `.alpack`.

نظام التوقيع الحالي Development stub؛ لا يجوز اعتباره تحققًا تشفيريًا لأن فحص Signature غير الفارغة لا يثبت صحة التوقيع. Directory mounting يجب أن يبقى Development-only مع Backend interface واضح، وألا يُقدّم كـPAK/IoStore mount في Shipping.

`AALPlayerCharacter::RequestRestartFromCheckpoint` يعيد Health ويفك القفل قبل أن يقرر GameMode/Checkpoint manager موضع اللاعب. يجب نقل ملكية إعادة التشغيل إلى GameMode/Prototype manager، مع إبقاء PlayerCharacter مسؤولًا عن تعطيل اللاعب وطلب restart فقط.

يجب تدقيق `StartCombat` و`ConfigurePerception` لمنع إعادة تشغيل timers أو تكرار delegate binding عند كل Decision Interval. كما يجب تدقيق Weapon firing hot path لمنع `LoadSynchronous()` المتكرر أثناء إطلاق النار.

## Android Blockers

إعدادات ARM64 وPackage Name وSDK values موجودة، لكن Unreal Engine 5.4 وAndroid SDK/NDK/Build Tools غير متاحة. لا يوجد Compile أو Cook أو Package أو APK أو Runtime device test. لا يجوز إعلان `APK READY` قبل تنفيذ هذه الخطوات فعليًا.

## Required Verification After Fixes

بعد الإصلاح يجب تشغيل Python validators و`git diff --check` و`validate_build_references.py`. إذا توفرت Unreal، يجب توليد project files وبناء Editor target وتشغيل command-line project validation. إذا لم تتوفر، يجب تسجيل العبارة الصريحة `UNREAL BUILD ENVIRONMENT NOT AVAILABLE` مع الأوامر المطلوبة على جهاز البناء.
