# ASH LINE COMBAT PROTOTYPE v0.0.1

## الحالة

> **CODE READY / BUILD BLOCKED**
>
> لم يتم إنشاء APK أو `.umap` أو `.uasset` فعلي داخل Sandbox لأن Unreal Engine 5.4 وAndroid SDK/NDK غير متاحة. تم تجهيز C++ وConfig وEditor Python وBuild Scripts، مع منع أي ادعاء بأن Demo أو Release اكتمل.

## 1. Commit SHA

Implementation commit: `0691ba5` — `feat: prepare combat prototype runtime and asset pipeline`.

## 2. Unreal version

المشروع مضبوط على Unreal Engine `5.4`. لم يتم تشغيل UnrealHeaderTool أو UBT أو Editor Compile لأن `UE_ROOT` غير مضبوط ولا يوجد Unreal installation في البيئة الحالية.

## 3. Files changed

تم تعديل `DefaultEngine.ini` لاستخدام `AALCombatPrototypeGameMode` مع إبقاء Engine Minimal map مؤقتًا، وتحديث input manifest ليشمل `IA_SwitchWeapon` مع alias هو `IA_NextWeapon`، وتحديث Android release script وvalidation scripts.

تمت إضافة `AALCombatPrototypeGameMode` في `AshLineMissions` لتسجيل الأعداء الموجودين في الخريطة، عدّ القتلى، وبث `OnEnemyCountChanged` و`OnPrototypeCompleted`. وتمت إضافة `AALCombatPrototypeHUD` في `AshLineUI` لربط Health وAmmo وEnemy Count وPrototype Complete وBuild Label عبر Delegates دون Tick polling.

تمت إضافة `Scripts/Editor/CreatePrototypeAssets.py` و`BuildAndroidPrototype.ps1` و`BuildAndroidPrototype.sh` وتقارير Release وSize وTest وSHA256 placeholders.

## 4. Assets created

لا توجد أصول Unreal فعلية حاليًا. يقوم `Scripts/Editor/CreatePrototypeAssets.py` بإنشاء الأصول التالية عند تشغيله من Unreal Editor Python:

| الفئة | الأصول |
|---|---|
| Map | `/Game/AshLine/Maps/Namar/L_CombatPrototype.umap` |
| Input | `IMC_Player` و`IA_Move` و`IA_Look` و`IA_Jump` و`IA_Crouch` و`IA_Sprint` و`IA_Interact` و`IA_Fire` و`IA_Aim` و`IA_Reload` و`IA_SwitchWeapon` و`IA_NextWeapon` و`IA_Pause` |
| Weapons | `DA_WPN_AR_001` و`DA_WPN_SMG_001` و`DA_WPN_PST_001` |
| AI | `DA_AI_Soldier_Basic` و`DA_AI_Soldier_Trained` |
| Blueprints | `BP_ALPlayerCharacter` و`BP_ALInfantry_Test` و`BP_ALCombatPrototypeGameMode` و`BP_ALCombatPrototypeHUD` و`BP_ALCoverPoint` و`BP_ALPatrolRoute` |
| UI | `WBP_MainMenu` و`WBP_MobileTouchLayer` و`WBP_CombatPrototypeHUD` |
| Support | `DA_Health_100` |

## 5. Map

ينشئ السكربت Arena خفيفة من Engine Basic Shapes تشمل PlayerStart، ساحة مركزية، شارعًا قصيرًا، مبنيين Blockout، أزقة، حواجز، جدارًا مرتفعًا، NavMeshBoundsVolume، مسار Patrol، ثماني نقاط Cover، وستة مواضع Enemy.

لا يستخدم السكربت Marketplace assets أو City packs أو Native Code. بعد الإنشاء يجب مراجعة Navigation ومواضع Cover داخل Unreal Editor، ثم تغيير `GameDefaultMap` و`EditorStartupMap` إلى `/Game/AshLine/Maps/Namar/L_CombatPrototype`.

## 6. Player

يبقى التنفيذ فوق `AALPlayerCharacter` و`AALPlayerController` الحاليين، ويدعم Move وLook وJump وCrouch وSprint وAim وFire وReload وSwitch Weapon. PC bindings هي WASD وMouse، بينما تستخدم Mobile widgets نفس Enhanced Input Actions.

## 7. Weapons

أنشأ السكربت Data Assets بالقيم المطلوبة:

| Weapon | Damage | Magazine | Reserve | RPM | Mode |
|---|---:|---:|---:|---:|---|
| AR | 28 | 30 | 90 | 650 | FullAuto |
| SMG | 21 | 32 | 96 | 850 | FullAuto |
| Pistol | 32 | 15 | 45 | 350 placeholder | SemiAuto |

الـPlayer loadout الافتراضي يبقى Primary AR وSidearm Pistol عبر Blueprint defaults، مع إمكانية اختيار SMG في Development من Weapon Data Asset أو Blueprint.

## 8. AI

يستخدم السكربت `BP_ALInfantry_Test` المشتق من `AALInfantryCharacter` مع ستة مواضع اختبار. يعتمد Basic وTrained على Data Assets، ويستمر AI في Patrol وDetect وAlert وCover وShoot وSearch وDie عبر النظام الموجود دون Tick دائم.

يراقب `AALCombatPrototypeGameMode` delegate `OnEnemyKilled` ويعلن Prototype Complete عند وصول عدد القتلى إلى ستة.

## 9. HUD

أضيف C++ HUD bridge delegate-driven. عند ربط `WBP_CombatPrototypeHUD` بالأحداث، يعرض Health bar وAmmo وCrosshair وEnemy count وBuild label `v0.0.1`، وتعرض `WBP_MainMenu` عنوان ASH LINE وCOMBAT PROTOTYPE وأزرار PLAY وSETTINGS وEXIT على PC.

## 10. Mobile controls

يتضمن التصميم Mobile movement virtual stick في الجهة اليسرى، Look region في الجهة اليمنى، وأزرار Fire وAim وReload وJump وCrouch وSprint وWeapon Switch وPause. تم تعريف Multi-touch في input manifest، لكن التحقق البصري واللمسي الفعلي يحتاج Unreal Editor وجهاز Android.

## 11. Build result

تم تشغيل فحوصات المصدر والـBuild References بنجاح. ثم أُعيد تنفيذ بوابات الإصدار الفعلية في آخر محاولة:

| Gate | Exit | النتيجة |
|---|---:|---|
| `BuildEditor.sh` | `2` | `UNREAL BUILD ENVIRONMENT NOT AVAILABLE: set UE_ROOT to a UE 5.4 installation.` |
| `BuildAndroidPrototype.sh Development` | `2` | `ANDROID BUILD BLOCKED: Set UE_ROOT to the installed Unreal Engine 5.4 directory.` |
| `BuildAndroidPrototype.sh Shipping` | `2` | `ANDROID BUILD BLOCKED: Set UE_ROOT to the installed Unreal Engine 5.4 directory.` |

وبالتالي لم يتم تنفيذ Generate Project Files أو Compile أو Cook أو Stage أو Package أو Install، ولم يظهر أي APK في `Releases/Android/0.0.1`.

## 12. APK path

المسار المتوقع بعد نجاح Shipping Build هو:

```text
Releases/Android/0.0.1/APK/AshLine_CombatPrototype_v0.0.1_android_arm64.apk
```

الحالة الحالية: **NOT PRODUCED**.

## 13. APK size

غير متاح. سيتم توليده فقط بعد Build فعلي في `ReleaseReport.md` و`SizeReport.md`. الهدف هو <=500 MB، والمفضل لهذا الـPrototype <=300 MB حيثما أمكن.

## 14. SHA-256

غير متاح قبل APK فعلي. الملف `Releases/Android/0.0.1/Checksums/SHA256.txt` يوضح ذلك، وسيتم استبداله تلقائيًا بواسطة Build script بعد نجاح Packaging.

## 15. Android device tested

لا يوجد جهاز Android متصل عبر ADB في Sandbox. لم يتم تنفيذ Install أو Launch أو Input أو Combat أو Death/Restart.

## 16. FPS

غير مقاس. يجب قياس 3 AI و6 AI، مع تسجيل FPS وCombat frame time وStartup time على جهاز Android ARM64، والحد الأدنى المستهدف 30 FPS.

## 17. RAM

غير مقاس. يجب تسجيل Runtime RAM وTexture memory وCooked content size بعد Build فعلي.

## 18. Known bugs and blockers

الموانع المعروفة هي غياب Unreal Engine وAndroid SDK/NDK، وعدم وجود `.uasset` أو `.umap` في المستودع حتى الآن، وعدم تشغيل Unreal Automation tests أو Runtime smoke tests. كذلك تحتاج Widgets التي أنشأها السكربت إلى تصميم عناصرها وربطها بصريًا داخل Unreal Editor، وتحتاج الخريطة إلى مراجعة NavMesh وSpawn وCover داخل Editor.

## 19. Next recommended work

شغّل السكربت من Unreal Editor Python:

```bash
UnrealEditor-Cmd ASH_LINE.uproject -run=pythonscript -script=Scripts/Editor/CreatePrototypeAssets.py -unattended -nop4
```

ثم حدّث `DefaultEngine.ini` إلى الخريطة الحقيقية، ولّد ملفات المشروع، وابنِ Editor target:

```bash
Scripts/Build/ValidateBeforeBuild.sh --require-ue
Scripts/Build/BuildEditor.sh
```

ولـAndroid:

```bash
export UE_ROOT=/path/to/UnrealEngine
export ANDROID_HOME=/path/to/Android/Sdk
export ANDROID_NDK_HOME=/path/to/Android/Sdk/ndk/<version>
Scripts/Build/BuildAndroidPrototype.sh Shipping
```

بعدها يجب تشغيل Automation tests، مراجعة Main Menu وMobile HUD، تنفيذ Runtime smoke test، تثبيت APK عبر ADB، وقياس FPS/RAM قبل اعتبار الإصدار مكتملًا.

## Release gate

| Gate | Status |
|---|---|
| C++ Compile | BLOCKED — Unreal unavailable |
| Map exists | BLOCKED — Editor script ready, asset not created |
| Input assets | BLOCKED — Editor script ready, assets not created |
| Player spawn | BLOCKED — Editor runtime not executed |
| Weapons | Source/Data pipeline ready; runtime not executed |
| Damage | Existing source validation PASS; runtime not executed |
| AI | Existing source validation PASS; runtime not executed |
| HUD | C++ bridge ready; widgets not visually authored/tested |
| Restart | Source hook ready; runtime not executed |
| Android config | PASS |
| APK build | ENVIRONMENT BLOCKED |

**Final status: CODE READY / BUILD BLOCKED.**
