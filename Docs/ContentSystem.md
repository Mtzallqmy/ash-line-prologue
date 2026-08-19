# ASH LINE Modular Content System

## الهدف

يعزل هذا النظام Gameplay عن مصدر المحتوى. لا يعرف Gameplay إن كان المحتوى Core أو Local ALPACK أو CDN أو Google Play Asset Delivery؛ بل يستعمل `UALContentManagerSubsystem` عبر `IsPackageInstalled` و`RequestPackage` و`IsContentAvailable`، بينما تتولى طبقة Content Delivery القراءة والتحقق والتثبيت والـMount.

## Architecture

```text
Gameplay
   ↓
UALContentManagerSubsystem
   ├── UALPackageRegistry
   ├── UALPackageValidator
   ├── UALPackageMountManager
   ├── UALLocalContentDeliveryService
   ├── UALHttpContentService
   └── UALPlayAssetDeliveryService
           ↓
     UALAssetManager
```

يعيش Content Manager كـ `UGameInstanceSubsystem` طوال جلسة اللعبة، ولا يستخدم Tick دائمًا. عمليات Registry وManifest وDependency Resolution وValidation مشتركة بين المنصات، بينما يظل File Picker وPlatform Storage وDelivery Backend ضمن طبقة المنصة.

## Package Model

لكل حزمة `PackageID` ثابت، و`EALPackageType`، وVersion مستقل، و`minGameVersion` و`maxGameVersion`، وحجم تنزيل وحجم تثبيت، وDependencies وOptional Dependencies، و`contentRoot`، و`mountPriority`، وSHA-256 وتوقيع ومفتاح تحقق عام. لا يعتمد النظام على اسم الملف كهوية.

الحالات الرسمية هي `Unknown` و`NotInstalled` و`Queued` و`Downloading` و`Downloaded` و`Verifying` و`Installing` و`Installed` و`Mounting` و`Mounted` و`Updating` و`Corrupted` و`Incompatible` و`Failed`.

## Global Manifest

المصدر الحالي هو `Content/AshLine/Data/content_manifest.json`، ويحتوي `schemaVersion` حتى يمكن ترقية الصيغة مستقبلًا. تم الحفاظ على IDs السابقة `core` و`city_namar_prologue` و`weapons_pack_01` و`audio_core` و`localization_core`، مع جعل المدينة والحزم الاختيارية غير مطلوبة من Core.

## ALPACK Format

`.alpack` هو ملف توزيع ZIP-compatible منطقيًا يحتوي:

```text
manifest.json
signature.dat                 # اختياري Development، مطلوب رسميًا في Shipping
content/
  <content-only files>
optional_metadata/
  build.json
```

الحزم لا تسمح بملفات `.so` أو `.dll` أو `.exe` أو Scripts تنفيذية، ولا تضيف Native Code إلى APK. أي Feature Gameplay جديدة تحتاج تحديث APK/AAB، أما الحزم فتقدم Content/Data فقط.

## Import Flow

التدفق المسموح هو File Picker أو Development directory ثم Temporary Copy ثم Manifest Parse ثم Schema وPackage ID وPath Safety وGame Compatibility وSize وSHA-256 وSignature وDependencies وFree Storage ثم Atomic Install ثم Registry Update ثم Mount وAsset Manager Scan. يمنع Mount قبل نجاح Verify.

التنفيذ المشترك موجود في `UALContentManagerSubsystem` و`UALPackageValidator`. Android File Picker نفسه نقطة Bridge في `UALContentPlatformBridge`، ويحتاج JNI/Kotlin SAF داخل Unreal Android Target قبل إعلان Runtime Pass.

## Registry and Recovery

يحفظ `UALPackageRegistry` سجلات الحزم في `ProjectPersistentDownloadDir()/AshLine/Manifests/registry.json`، وهي منفصلة عن Player Save. يحتوي السجل Package ID وVersion وPath وState وHash وMounted وInstall Date وDependencies. عند تلف Registry يعاد بناؤه من `Content/Installed/**/manifest.json`.

ملفات التشغيل تقسم إلى `Content/Installed` و`Content/Downloads` و`Content/Temp` و`Content/Patches` و`Manifests`، ولا توجد مسارات Android hardcoded داخل Gameplay.

## Dependencies and Remove

يحل Validator الرسم البياني ويكتشف Missing وCircular Dependencies. يمنع Remove للحزم المطلوبة أو للحزم التي تعتمد عليها حزم مثبتة. قبل حذف حزمة اختيارية ينفذ Safe Unmount ثم حذف الملفات ثم Registry Update، ولا يلمس Player Save أو Mission Progress.

## Mount and Asset Manager

`IALPackageMountBackend` abstraction تسمح لاحقًا بدعم Directory وPak وIoStore/UTOC/UCAS. التنفيذ الحالي يثبت Directory packages في Development ويستدعي `UALAssetManager::RegisterMountedPackageRoot` لمسح Asset Registry للمسارات المعنية. Pak/IoStore الفعلي يحتاج Unreal Packaging Target وRuntime Platform File المتوافق مع إصدار المحرك.

## Delivery Services

`UALLocalContentDeliveryService` هو التنفيذ الأول للحزم المحلية. `UALHttpContentService` يوفر عقد CDN وManifestURL وContentBaseURL وEnvironment من دون hardcoded production domain، لكنه لا ينفذ شبكة قبل وجود Backend. `UALPlayAssetDeliveryService` نقطة مستقبلية لـ Google Play Asset Delivery ولا تعد متاحة قبل ربط Android Play Core.

## BuildALPack

الأداة التطويرية هي `Scripts/Content/BuildALPack.py`. تستخدم Copy/Hash على شكل Streaming blocks، تمنع الملفات التنفيذية، تنشئ Manifest وSize Report وTop Files، ويمكنها توقيع Manifest باستخدام `openssl` ومفتاح خارج Repository عبر `--signing-key`. في Shipping يجب استعمال Public Verification Key فقط داخل التطبيق، وعدم حفظ Private Key أو Password داخل Git أو APK.

مثال:

```bash
python3 Scripts/Content/BuildALPack.py \
  Tests/ContentFixtures/test.content01/content \
  Releases/Content/Development/AshLine_TestContent_v1.alpack \
  --package-id test.content01 \
  --package-type Mission \
  --dependency core \
  --content-root /Game/AshLine/TestContent/ \
  --entry-asset /Game/AshLine/TestContent/test_asset
```

## Update and Patch Readiness

يدعم Manifest `patchFromVersion` و`patchSize` وMount Priority وUpdate metadata. التحديث الكامل أو Patch مستقبلي يمر بنفس Verify ثم Atomic Install، ولا تحذف النسخة القديمة قبل نجاح النسخة الجديدة إذا سمحت المساحة. لا يوجد Binary Diff Engine في هذه المرحلة.

## Storage and Security

يتحقق النظام من Path Traversal وAbsolute Paths وPackage IDs غير الآمنة وSize Limits وManifest Limits. لا يقرأ الحزمة كاملة إلى RAM في BuildALPack؛ يستخدم Streaming Hash وCopy. Runtime Android يحتاج File Picker SAF بدل الوصول الحر إلى مساحة الهاتف. لا يطلب النظام Permissions إضافية من Gameplay لمجرد Save أو Content.

## Tests

تم توفير Fixture صغيرة `AshLine_TestContent_v1.alpack` لاختبار ALPACK. فحوصات Python تتحقق من Manifest Schema وDuplicate IDs وMissing/Circular Dependencies وForbidden Native Files وNo Secret Markers وProject Regressions. اختبار Hash Mismatch وInvalid Signature وAndroid SAF وPak/IoStore الحقيقي يحتاج Unreal Toolchain وجهاز Android.

## Developer Workflow

لإضافة مدينة جديدة: أنشئ Assets وCity Definition ثم Package Definition ثم ID وDependencies، شغّل BuildALPack، راجع Size Report، وقّع الحزمة في بيئة آمنة، ثم اختبر Import/Validate/Install/Mount/Use/Unmount/Remove/Reinstall. لا يحتاج Content Manager إلى تعديل C++ لكل مدينة جديدة.

## Limits Before APK

لا يمكن إعلان Android Content Runtime Pass في البيئة الحالية قبل توفر Unreal Engine 5.4 الفعلي، Android SDK/NDK/JDK المتوافق، Android File Picker JNI، وPak/IoStore Cook. ما تم تنفيذه الآن هو Core Content Infrastructure وALPACK Builder وValidation، مع stubs صريحة لا تدّعي وجود CDN أو Play Asset Delivery.
