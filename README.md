# ASH LINE — Prologue

**ASH LINE** هي لعبة حرب تكتيكية ثلاثية الأبعاد، ويُعد هذا المستودع أساس الإصدار الأول **Prologue**. يستهدف المشروع أجهزة Android المتوسطة والقوية مع دعم اللمس، مع الحفاظ على فصل طبقة المنصة بما يسمح بإضافة Windows لاحقًا دون إعادة كتابة أنظمة Gameplay الأساسية.

> هذا المستودع يقدّم الهيكل البرمجي والمحتوى الوصفي وملفات الإعداد وأدوات التدقيق الخاصة بـ Phase 1. يحتاج إنشاء نسخة Android قابلة للتثبيت إلى جهاز يحتوي على Unreal Engine وAndroid SDK/NDK وبيئة بناء مناسبة.

## نطاق الإصدار الأول

تدور تجربة Prologue في منطقة تدريب صغيرة داخل مدينة **Namar — Training District**، ولا تهدف إلى بناء مدينة كاملة. تتكون المنطقة من شارع رئيسي، وأربعة أزقة، ومبانٍ عربية Modular، وسوق صغير، ومبنى قابل للدخول، وسطحين قابلين للوصول، وساحة، ونقطة عسكرية، ومنطقة استخراج.

تتكون التجربة من ثلاث مهمات بمدة مستهدفة تتراوح بين **15 و25 دقيقة**. تعلّم المهمة الأولى الحركة والكاميرا والركض والانحناء والتفاعل. تضيف المهمة الثانية إطلاق النار وإعادة التعبئة واستخدام الغطاء والضرر وذكاء الأعداء. أما المهمة الثالثة فتقدم Scout Drone للاستطلاع وتحديد الأهداف والاشتباك القصير ثم الانتقال إلى الاستخراج، وبعد النجاح تظهر رسالة **Prologue Complete**.

يحتوي الإصدار الأول على ثلاثة أسلحة فقط: Assault Rifle وSMG وPistol. جميعها تعتمد على `WeaponBase` واحد وبيانات `WeaponDataAsset`، ولذلك يمكن إضافة أسلحة مستقبلية من خلال Data Assets بدل إنشاء Class منفصل لكل سلاح.

## المتطلبات التقنية

| العنصر | الاختيار |
|---|---|
| المحرك | Unreal Engine 5.4 أو أحدث |
| المنصة الأساسية | Android |
| المنصة المستقبلية | Windows |
| Gameplay والأنظمة عالية الأداء | C++ |
| ربط المحتوى والأحداث وواجهة المستخدم | Blueprint فوق أنظمة C++ |
| تكامل Android المستقبلي | Kotlin عند الحاجة فقط |
| المواد والمؤثرات | Unreal Material System وHLSL عند الحاجة |
| بيانات المحتوى | JSON وData Assets |
| الأتمتة والتدقيق | Python |
| التحكم | Touch وKeyboard/Gamepad عبر نقاط إدخال مشتركة |

لا يعتمد قلب اللعبة على Blueprint-only architecture، ولا يحتوي Gameplay على مسارات ملفات مادية أو مفاتيح حساسة. كما لا يستخدم التصميم فك ZIP التقليدي كأساس لتوزيع الحزم؛ بل يعتمد Manifest وحزمًا معروفة للمحرك وطبقة Delivery قابلة للاستبدال.

## بنية Modules

| Module | المسؤولية |
|---|---|
| `AshLineCore` | Damage contracts، Save Game، Settings، Asset Manager والأنظمة المشتركة |
| `AshLineCharacters` | Player Character والمكونات القابلة لإعادة الاستخدام |
| `AshLineCombat` | Damage System مركزي قابل للتوسع إلى المركبات لاحقًا |
| `AshLineWeapons` | `WeaponBase` و`WeaponDataAsset` والـ Ballistics |
| `AshLineAI` | Infantry AI وحالات Idle وPatrol وSuspicious وAlert وCombat وSearch |
| `AshLineDrones` | Drone Base والحركة والكاميرا والبطارية والحساس |
| `AshLineMissions` | Mission Data وبدء المهمة وإنهائها وتقدم الأهداف |
| `AshLineContent` | Manifest وLocal Delivery وChunk Definitions وواجهات التوزيع |
| `AshLineUI` | HUD ونقاط التكامل مع Main Menu وMission HUD وDrone HUD وSettings |
| `AshLinePlatform` | حدود التكامل مع Storage وPermissions وNotifications وPlay Asset Delivery |

## هيكل الملفات

```text
ASH_LINE/
├── ASH_LINE.uproject
├── Config/
├── Content/AshLine/
│   ├── Core/
│   ├── Characters/
│   ├── Weapons/Data/
│   ├── Drones/
│   ├── AI/
│   ├── Missions/Data/
│   ├── Maps/Namar/
│   ├── UI/
│   ├── Audio/
│   ├── Materials/
│   ├── VFX/
│   ├── Data/
│   └── Localization/
├── Source/
│   ├── AshLineCore/
│   ├── AshLineCharacters/
│   ├── AshLineCombat/
│   ├── AshLineWeapons/
│   ├── AshLineAI/
│   ├── AshLineDrones/
│   ├── AshLineMissions/
│   ├── AshLineContent/
│   ├── AshLineUI/
│   └── AshLinePlatform/
├── Scripts/Validation/
├── Scripts/Build/
└── Docs/
```

يجب الالتزام بتسمية الأصول باستخدام بادئات واضحة مثل `BP_` و`WBP_` و`DA_` و`SM_` و`SK_` و`T_` و`M_` و`MI_` و`S_` و`A_`. أما الخريطة الرئيسية فتُسمى `L_Namar_Prologue`، وتُمنع الأسماء العامة مثل `NewBlueprint` و`Test2` و`Final_Final`.

## Content Delivery والـ Chunks

يستخدم المشروع ملف `Content/AshLine/Data/content_manifest.json` لتعريف الحزم، وملف `Content/AshLine/Data/chunk_manifest.json` لتعريف توزيعها. الحزم الأساسية هي `core` و`city_namar_prologue` و`weapons_pack_01` و`audio_core` و`localization_core`.

| Chunk ID | الحزمة | المحتوى |
|---:|---|---|
| 0 | `core` | Runtime وCore UI وبيانات اللعبة الأساسية |
| 1 | `city_namar_prologue` | خريطة Namar ومحتوى المهمات |
| 2 | `weapons_pack_01` | الأسلحة والشخصيات المرتبطة بها |
| 3 | `audio_core` | أصوات الإطلاق وإعادة التعبئة والصوتيات الأساسية |
| 4 | `localization_core` | ملفات الترجمة والتوطين |

تعرّف الواجهة `IALContentDeliveryService` وظائف فحص Manifest، استعراض الحزم المثبتة، طلب الحزمة، تتبع التقدم، Mount وUnmount، والتحقق من سلامة الحزمة. يعمل التنفيذ الحالي محليًا، ويمكن استبداله لاحقًا بتنفيذ CDN أو Google Play Asset Delivery دون تعديل أنظمة Gameplay.

## الحفظ

يحفظ الإصدار الأول تقدم المهمات، والإعدادات، والمعدات المفتوحة، وتقدم اللاعب. يستخدم Save Game مع IDs مستقرة بدل تخزين بيانات المدينة نفسها، بحيث يمكن تحديث الأصول أو حذف حزمة اختيارية دون فقدان Save Game.

## ميزانية الحجم والأداء

| الفئة | الهدف |
|---|---:|
| Core Runtime + Code | ≤150 MB |
| Namar Prologue City | ≤170 MB |
| Characters + Animations | ≤60 MB |
| Weapons | ≤30 MB |
| Audio | ≤40 MB |
| UI + Fonts | ≤15 MB |
| Drone + VFX | ≤15 MB |
| Reserved overhead | ≤20 MB |
| **الحد الصارم للإصدار** | **≤500 MB** |

يجب قياس Shipping Build الحقيقي بدل الاعتماد على تقدير المصدر. لا تستخدم المرحلة الأولى Textures بدقة 4K؛ الافتراضي هو 512–1024، وتُستخدم 2K فقط عند وجود مبرر بصري واضح. يُفضّل استخدام Texture Atlases وTrim Sheets وShared Materials وMaterial Instances وChannel Packing وDecals قابلة لإعادة الاستخدام.

يجب توفير Profiles باسم Low وMedium وHigh، وقياس FPS وRAM وVRAM وDraw Calls وعدد المضلعات وذاكرة Textures وتعقيد Shaders وزمن CPU الخاص بالـ AI. الهدف الأدنى هو Gameplay مستقر على Medium Profile مع تشغيل AI غير الضروري خارج المنطقة النشطة.

## تشغيل المشروع في Unreal Engine

افتح `ASH_LINE.uproject` باستخدام Unreal Engine 5.4 أو أحدث، ثم ولّد ملفات المشروع من قائمة Unreal أو من نظام التشغيل. بعد ذلك أنشئ Blueprint subclasses وData Assets والمحتوى المرئي داخل `Content/AshLine/` وفق الخطة الموجودة في `Docs/Content/AssetPlan.md`، وأنشئ الخريطة `L_Namar_Prologue` طبقًا للتصميم في `Content/AshLine/Maps/Namar/`.

لتجميع المصدر من سطر الأوامر، اضبط متغير البيئة `UE_ROOT` على مسار تثبيت Unreal Engine ثم شغّل:

```bash
Scripts/Build/BuildPrologue.sh
```

السكربت يتحقق أولًا من المشروع والـ Manifests، ثم يستدعي Unreal Build Tool. أما Cook وPackage وقياس حجم Android Shipping Build فتُنفذ من جهاز البناء الذي يحتوي على Unreal Engine وAndroid SDK/NDK.

## التحقق المحلي

يمكن تشغيل فحوصات المصدر والبيانات من جذر المشروع:

```bash
python3 Scripts/Validation/validate_project.py .
python3 Scripts/Validation/static_surface_check.py .
python3 Scripts/Validation/size_report.py .
```

تتحقق الأدوات من وجود Modules العشرة، وتوافق الحزم مع الـ Chunks، وعدم وجود IDs مكررة للأسلحة، وعدم وجود أسماء Placeholder ممنوعة، وتنتج تقريرًا أوليًا للحجم. لا يُعد تقرير المصدر بديلًا عن قياس Shipping Build النهائي.

## Git وGit LFS

يستبعد `.gitignore` مجلدات `Binaries` و`Intermediate` و`Saved` و`DerivedDataCache` وملفات البناء المؤقتة. يحدد `.gitattributes` أصول Unreal الثنائية المناسبة لـ Git LFS، مثل `.uasset` و`.umap` وملفات الصوت والنماذج. إذا لم يكن Git LFS مثبتًا على جهاز التطوير، يجب تثبيته قبل رفع أصول ثنائية كبيرة.

## ما هو خارج Phase 1

لا يتضمن هذا الإصدار Multiplayer أو مدينة ثانية أو دبابات أو مروحيات قتالية أو متجرًا أو Battle Pass أو Accounts Backend أو Destruction متقدمًا أو نظام Factions كاملًا أو طقسًا معقدًا أو Cinematics ضخمة. صُممت الواجهات الحالية بحيث لا تمنع إضافة هذه الأنظمة في إصدارات لاحقة.

## الحالة الحالية والقيود المعروفة

تم تنفيذ هيكل المشروع البرمجي، والـ Modules، وPlayer Components، وWeapon System، وDamage System، وAI State Machine، وDrone Components، وMission Framework، وSave Game، وAsset Manager، وContent Delivery Interfaces، وJSON Manifests، وChunk Definitions، وملفات الإعداد، وأدوات التدقيق.

إنشاء الأصول المرئية الفعلية، وتجميع C++ داخل Unreal، وبناء Namar في Editor، واختبار Touch على جهاز Android، وتشغيل Navigation، وCook وPackaging، وقياس Shipping Build النهائي تحتاج إلى بيئة Unreal Engine وAndroid SDK/NDK. لذلك لا يُعلن المستودع نجاح Definition of Done النهائي قبل تنفيذ هذه الاختبارات على جهاز بناء فعلي.

## الترخيص والمساهمة

هذا المشروع نموذج تطويري خاص بمرحلة ASH LINE Prologue. قبل نشر أصول فنية أو صوتية أو حزم خارجية، يجب التأكد من حقوق استخدامها وتوثيق تراخيصها. تُقبل التعديلات التي تحافظ على فصل Modules، وData-Driven Content، وميزانيات الحجم، وقابلية إضافة City_02 كحزمة مستقلة.

## الأمان

لا تضع Tokens أو مفاتيح API أو بيانات اعتماد داخل المستودع أو ملفات Gameplay أو README. إذا ظهر Token في محادثة أو سجل أو ملف بالخطأ، يجب إلغاؤه فورًا من مزود الخدمة وإنشاء Token بديل بصلاحيات أقل ما يمكن.

## Prompt 02 — Player and Movement

تمت إضافة بنية اللاعب الإنتاجية في `AshLineCharacters` عبر `AALPlayerCharacter` و`AALPlayerController` و`UALPlayerStateComponent` و`UALInteractionComponent` و`IALInteractable`. يستخدم اللاعب `UCharacterMovementComponent` وEnhanced Input، مع First-Person camera hierarchy، حالات حركة واضحة، سرعات Data-Driven، Sprint Hold/Toggle، Crouch، Jump، Pitch Clamp، Input Locks، وتفاعل عام قابل للتوسع.

تم توثيق إعداد `IMC_Player` وInput Actions و`BP_ALPlayerCharacter` و`DA_PlayerMovement` و`WBP_MobileTouchLayer` داخل مجلدات `Content/AshLine/Input/` و`Content/AshLine/Characters/Player/` و`Content/AshLine/UI/Mobile/`. الأفعال الخاصة بالأسلحة والـ Aim والـ Reload والـ Drone محجوزة للمراحل التالية ولا تُنفذ في Prompt 02.

للتفاصيل الكاملة راجع [تقرير Prompt 02](Docs/Prompt02ImplementationReport.md).

## Prompt 03 — Health, Damage and Death

تمت إضافة Health/Damage Framework خفيف داخل `AshLineCombat` عبر `UALHealthComponent` و`UALDamageSystemSubsystem` و`FALDamageData` و`IALDamageReceiver`. يدعم الإصدار الحالي اللاعب وشخصيات Infantry، مع Health وHealing وReset وInvulnerability وDamage Events وDeath Event وFall Damage وEnvironmental Damage Volume، بينما تبقى Armor وVehicle Damage وAdvanced Ragdoll وMultiplayer خارج النطاق.

يرتبط موت اللاعب بـ `UALPlayerStateComponent` لتعطيل الحركة والنظر والتفاعل والإدخال، ثم يرسل Hook لإعادة التشغيل من Checkpoint. كما توجد أدوات Development للضرر والشفاء والقتل وGod Mode، وهي غير فعالة في Shipping. تفاصيل التنفيذ والاختبارات موجودة في [تقرير Prompt 03](Docs/Prompt03ImplementationReport.md).

## Prompt 04 — Weapon System and Combat Prototype

تم تنفيذ Weapon Framework Data-Driven داخل `AshLineWeapons` عبر `AALWeaponBase` و`UALWeaponComponent` و`UALRecoilComponent` و`UALWeaponDataAsset`. يدعم الإصدار الحالي Assault Rifle وSMG وPistol من خلال Data Assets وIDs ثابتة، مع Full Auto وSemi Auto وAmmo وReserve Ammo وReload وAim وADS FOV وSpread وRecoil وHitscan وHit Zones و`FALDamageData`.

تم تفعيل `IA_Fire` و`IA_Aim` و`IA_Reload` وإضافة `IA_NextWeapon`، مع تحديث Touch Layer لأزرار Fire وAim وReload وWeapon Switch. لا يحتوي Widget على منطق الرماية؛ تمر الأفعال عبر Player Controller ثم Player Character ثم Weapon Component ثم Weapon Base. يمكن لـ AI لاحقًا استخدام `SetAimDirection` و`FireAtTarget` دون الاعتماد على Mouse أو Touch.

لتفاصيل التنفيذ والاختبارات راجع [تقرير Prompt 04](Docs/Prompt04ImplementationReport.md) و[خطة محتوى الأسلحة](Content/AshLine/Weapons/README.md).

## Prompt 05 — Enemy AI and First Combat Prototype

تم تنفيذ Enemy AI فوق `AshLineAI` باستخدام `AALInfantryCharacter` و`AALAIController` وState/Sense/Combat/Cover/Significance Components. يدعم النظام Idle وPatrol وSuspicious وAlert وCombat وSearch وReturning وDead، مع Sight وHearing وAwareness وDamage Awareness وLast Known Location وPatrol Routes وCover Reservation وSpawner وAI LOD Foundation.

يعيد العدو استخدام `UALHealthComponent` و`UALWeaponComponent` و`AALWeaponBase` من المراحل السابقة. يطلق AI عبر Reaction Timer وAim Error وBurst/Pause، وتصل الطلقات إلى `FALDamageData` وHealth System نفسه. يرسل إطلاق اللاعب Gunshot Noise إلى Core Noise Subsystem لتستجيب AI القريبة بالسمع، ويرسل موت العدو `OnEnemyKilled` للمهمات اللاحقة.

تمت إضافة `DA_AI_Soldier_Basic` و`DA_AI_Soldier_Trained` كبيانات JSON، ودليل إعداد AI وPatrol وCover وSpawner، وخطة `L_CombatTest`. تفاصيل التنفيذ والاختبارات والقيود موجودة في [تقرير Prompt 05](Docs/Prompt05ImplementationReport.md).

## Prompt 06 — Modular Content Delivery وALPACK

تمت إضافة بنية Content Delivery مستقلة داخل `AshLineContent` باستخدام `UALContentManagerSubsystem` و`UALPackageRegistry` و`UALPackageValidator` و`UALPackageMountManager`، مع Local Delivery وCDN/PAD service stubs وAsset Manager scan hooks. يدعم النظام Manifest Schema Version 1 وPackage IDs ثابتة وVersioning وDependencies وOptional Dependencies وSHA-256 وSignature metadata وMount Priority وPatch readiness وRegistry recovery.

الحزمة المحلية تستخدم تنسيق `.alpack` يحتوي `manifest.json` و`content/` و`optional_metadata/` و`signature.dat` عند التوقيع. أداة البناء هي `Scripts/Content/BuildALPack.py`، ويوجد Fixture صغير باسم `Releases/Content/Development/AshLine_TestContent_v1.alpack`. الحزم Content-only ولا تسمح بـ Native Libraries أو Executables أو Scripts تنفيذية.

تم توثيق التدفق الكامل في [ContentSystem.md](Docs/ContentSystem.md)، والتقرير في [ModularContentSystemReport.md](Docs/ModularContentSystemReport.md). Android File Picker وPak/IoStore وCDN وGoogle Play Asset Delivery الفعلية ما زالت تحتاج Unreal/Android Toolchain قبل إعلان Runtime Release.

## Android v0.0.1 Release Preparation

تم تجهيز إعدادات Android للإصدار `ASH LINE — Combat Prototype v0.0.1`: Package ID هو `com.ashline.game`، وMin SDK هو 26، وABI المستهدف هو `arm64-v8a` فقط، مع Landscape وOpenGL ES compatibility path وVersion Code 1. تم إنشاء `Scripts/Build/BuildAndroidRelease.sh` و`Scripts/Validation/verify_android_release.py` وRelease Report داخل `Releases/Android/0.0.1/Reports/`.

تمت محاولة تشغيل Build Gate، لكنه توقف برسالة واضحة لأن Unreal Engine 5.4 غير مثبت أو غير محدد عبر `UE_ROOT`، كما أن Android SDK/NDK غير متاحين. لذلك لا يوجد APK وهمي أو غير قابل للتحقق؛ يجب تشغيل سكربت البناء على جهاز يحتوي Unreal وAndroid Toolchain فعليين.

## Corrective Fix — Stable Playable Foundation

تم تنفيذ تصحيح استقرار شامل على الفرع `fix/playable-v0.0.1` دون إعادة إنشاء المشروع أو حذف Architecture الحالية. أزيلت إدخالات Engine Modules الخاطئة من `ASH_LINE.uproject`، وثُبّت Death Flow بحيث يقرر GameMode نقطة إعادة اللاعب قبل Reset Health وفتح التحكم، وتم جعل Weapon hot path يعتمد على Cached FireSound وMuzzleFX بدل التحميل المتكرر أثناء إطلاق النار.

تم جعل `StartCombat` في AI idempotent، ومنع تكرار Perception delegate binding، وإضافة Semantic Version parser ومقارنة رقمية، وحساب SHA-256 فعلي لمجلدات Development package، وتعطيل external unsigned packages في Shipping. أصبح Directory mount معلنًا بوضوح كـDevelopment backend، وأضيفت بوابة مستقبلية لـPak وIoStore عبر `IALPackageMountBackend`.

أضيفت أدوات البناء التالية:

```text
Scripts/Build/ValidateBeforeBuild.sh
Scripts/Build/BuildEditor.sh
Scripts/Build/BuildDevelopment.sh
Scripts/Build/BuildAndroidDevelopment.sh
Scripts/Build/BuildAndroidShipping.sh
Scripts/Build/ValidateBeforeBuild.ps1
Scripts/Build/BuildAndroidDevelopment.ps1
Scripts/Build/BuildAndroidShipping.ps1
Scripts/Editor/CreatePrototypeAssets.py
Scripts/Validation/validate_build_references.py
```

تشغّل الفحوصات السابقة عبر:

```bash
Scripts/Build/ValidateBeforeBuild.sh
```

وتنجح فحوصات Python وBash و`git diff --check`. أما Unreal Editor Compile وAutomation tests وCook/Package فلم تُنفذ لأن Unreal Engine 5.4 وAndroid SDK/NDK غير متاحة في بيئة التنفيذ. لذلك تبقى حالة المشروع **Ready for Combat Demo: NO** إلى أن يتم Compile وCook وفتح الخريطة وتشغيل Runtime smoke test على جهاز بناء فعلي. التفاصيل في [CorrectiveReport.md](Docs/Build/CorrectiveReport.md) و[PreFixAudit.md](Docs/Build/PreFixAudit.md).

## Combat Prototype v0.0.1

تم تجهيز أول Combat Prototype فوق النسخة المصححة. أضيف `AALCombatPrototypeGameMode` لعدّ الأعداء وإعلان `Prototype Complete` عبر `OnEnemyKilled`، وأضيف `AALCombatPrototypeHUD` لربط Health وAmmo وEnemy Count وBuild Label عبر Delegates دون Tick polling. كما تم توسيع `Scripts/Editor/CreatePrototypeAssets.py` لإنشاء الخريطة والأصول الفعلية من داخل Unreal Editor Python فقط، بما يشمل `L_CombatPrototype.umap` وInput Actions وWeapon Data Assets وAI Data Assets وBlueprint subclasses وMobile/Combat HUD assets وArena blockout خفيف.

تم إنشاء `BuildAndroidPrototype.ps1` و`BuildAndroidPrototype.sh` لإجراء Validate ثم UE/SDK/NDK gates ثم Build/Cook/Stage/Package وإخراج APK وSHA-256 وتقارير Release عند نجاح Unreal فعليًا. في البيئة الحالية لا توجد Unreal Engine أو Android SDK/NDK، لذلك الحالة الصادقة هي **CODE READY / BUILD BLOCKED**، ولا توجد `.umap` أو `.uasset` أو APK وهمية. راجع [CombatPrototype_v0.0.1.md](Docs/Build/CombatPrototype_v0.0.1.md) للتفاصيل والأوامر الدقيقة.
