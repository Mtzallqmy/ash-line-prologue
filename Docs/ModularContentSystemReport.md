# ASH LINE MODULAR CONTENT SYSTEM REPORT

## 1. Files Created

تم إنشاء `ALPackageRegistry` و`ALPackageValidator` و`ALPackageMountManager` و`ALContentManagerSubsystem` و`ALRemoteContentServices` و`ALContentPlatformBridge` داخل Module `AshLineContent`. تمت إضافة `ALNoise` غير مرتبط بهذا التحديث. كما تم إنشاء `BuildALPack.py` و`validate_content_system.py` و`ContentSystem.md` وFixture اختبار صغيرة باسم `AshLine_TestContent_v1.alpack`.

## 2. Files Modified

تم تحديث `ALContentManifest.h` و`ALContentDeliveryService.h` و`ALLocalContentDeliveryService.h/.cpp` و`AshLineContent.Build.cs`، إضافة إلى `UALAssetManager` في `AshLineCore`، و`content_manifest.json` ليتحول إلى Schema Version 1 قابل للترقية.

## 3. Modules Used

تم استخدام Module `AshLineContent` الموجود مسبقًا دون إنشاء Module مكرر. يعتمد على Core وEngine وAssetRegistry وPakFile وJson وJsonUtilities وAshLineCore.

## 4. Content Manager Architecture

`UALContentManagerSubsystem` هو `UGameInstanceSubsystem` طويل العمر. يملك Registry وValidator وMountManager وLocalService ويعرض API مستقلة لـ Gameplay، لذلك لا يعرف Gameplay هل الحزمة محلية أو CDN أو Play Asset Delivery.

## 5. Delivery Service Interface

`IALContentDeliveryService` يدعم FetchManifest وRequestDownload/RequestPackage وPause وResume وCancel وProgress وImport وVerify وMount وUnmount وGetPackageFilePath. تم تجهيز `UALHttpContentService` و`UALPlayAssetDeliveryService` كـ stubs صريحة خلف نفس العقد.

## 6. Local Import Service

`UALLocalContentDeliveryService` موجود كأول Local backend، بينما التدفق الإنتاجي المشترك للحزم يدخل من `UALContentManagerSubsystem::ImportPackage`. Development يدعم Directory ALPACK، ويحتاج Android File Picker SAF Bridge لإكمال اختيار الملف على الهاتف.

## 7. Android File Picker Integration

تم إنشاء `UALContentPlatformBridge` ليحصر OpenFilePicker وApp-Specific Root. Android SAF/JNI/Kotlin غير مفعّل في البيئة الحالية ولا يوجد وصول حر لمساحة الهاتف. لا يُعلن Android Import Runtime PASS قبل إضافة Unreal Android JNI bridge واختباره على جهاز.

## 8. Storage Paths

المسارات مشتقة من `FPaths::ProjectPersistentDownloadDir()`:

```text
AshLine/
├── Content/Installed/
├── Content/Downloads/
├── Content/Temp/
├── Content/Patches/
└── Manifests/registry.json
```

لا يستخدم SaveGame لتخزين Registry ولا توجد مسارات Android hardcoded.

## 9. ALPACK Format

الـ ALPACK ZIP-compatible ويحتوي `manifest.json` و`content/` و`optional_metadata/build.json` و`signature.dat` عند التوقيع. يمنع BuildALPack Native libraries وExecutables وScripts التنفيذية من حزمة Content-only.

## 10. Manifest Schema

Schema Version 1 يحتوي Package ID وPackage Type وVersion وGame Compatibility وDownload/Installed Size وDependencies وOptional Dependencies وMount Priority وSHA-256 وSignature وPublic Key ID وContent Root وEntry Assets وPatch metadata.

## 11. Package Registry

Registry JSON دائم يحتوي Package ID وVersion وPath وState وHash وMounted وInstall Date وInstalled Size وDependencies. عند فشل القراءة يعيد النظام Scan لملفات `manifest.json` تحت Installed لإعادة البناء.

## 12. Package States

الحالات هي Unknown وNotInstalled وQueued وDownloading وDownloaded وVerifying وInstalling وInstalled وMounting وMounted وUpdating وCorrupted وIncompatible وFailed، مع Delegates لتغيير الحالة والتقدم والأحداث.

## 13. Dependency Resolver

`UALPackageValidator` يبني رسم Dependencies ويكشف Missing Dependency وCircular Dependency قبل Install أو Mount. Required وOptional Dependencies منفصلتان في Manifest.

## 14. Hash Verification

Manifest يحمل SHA-256، وValidator يقارن Expected Hash بـ Actual Hash مع Error Code `HashMismatch`. BuildALPack ينفذ SHA-256 Streaming على محتوى الحزمة ويكتب القيمة في Manifest. Runtime cryptographic file hashing الكامل يحتاج ربط Unreal SecureHash/Platform File المتوافق مع نسخة المحرك النهائية.

## 15. Signature Verification

BuildALPack يدعم توقيع Manifest عبر OpenSSL ومفتاح خارجي، ولا يحفظ Private Key في Repository أو APK. Shipping policy مهيأة لرفض Unsigned Packages، بينما Development يسمح مؤقتًا بالحزم غير الموقعة عند تفعيل ذلك صراحة.

## 16. Package Installation

التدفق هو Temp Copy ثم Manifest/Path/Compatibility/Dependency/Storage validation ثم Copy إلى Installed ثم Registry Update ثم Mount. لا يسجل النظام Installed قبل اكتمال النسخ، وMount يمر عبر Verify أولًا.

## 17. Package Mounting

`IALPackageMountBackend` abstraction جاهزة لـ Directory وPak وIoStore. التنفيذ الحالي Directory Development backend ويستدعي `UALAssetManager::RegisterMountedPackageRoot` لمسح Asset Registry لمسارات الحزمة. Pak/IoStore الفعلي يتطلب Unreal Cook/Runtime Platform File.

## 18. Asset Manager Integration

تمت إضافة `RegisterMountedPackageRoot` إلى `UALAssetManager` لتسجيل الجذور الجديدة دون `GetAllAssets` على كامل المشروع في كل مرة.

## 19. Remove/Uninstall Flow

Remove يتحقق من Required وDependent Packages ثم Safe Unmount ثم Delete Package Directory ثم Registry Update و`OnPackageRemoved`. لا يحذف SaveGame أو Mission Progress.

## 20. Update Architecture

يدعم النظام Update من Downloads staging، Version وPatch metadata وMount Priority. Full replacement وPatch foundation موجودان، أما Delta Binary Diff وRollback المتقدم فمؤجلان.

## 21. CDN Readiness

`UALHttpContentService` يحتوي ContentBaseURL وManifestURL وEnvironment ولا يثبت production domain. لا ينفذ Network Download قبل وجود Server، لكنه يمنع ربط Gameplay بـ HTTP مباشرة.

## 22. Google Play Asset Delivery Readiness

`UALPlayAssetDeliveryService` موجود كعقد مستقبلي. لا يوجد Play Core SDK أو Asset Pack build في هذه المرحلة، ولا يحتاج Content Manager إلى تعديل عند إضافته.

## 23. Async Operations

BuildALPack يستخدم Streaming Copy/Hash، بينما Runtime manager الحالي يمثل orchestration آمنًا لا يعتمد Tick. عمليات Android/HTTP الطويلة تحتاج استكمال Async worker/HTTP callbacks داخل Unreal Target قبل التوزيع.

## 24. Security Measures

هناك Path Traversal protection وAbsolute Path rejection وPackage ID validation وSize Limits وContent-only executable rejection وPrivate Key exclusion وVerify-before-Mount وDependency checks.

## 25. Recovery Mechanism

Registry recovery من Installed manifests موجود، وTemp/Downloads roots منفصلة. Startup cleanup يحذف Temp. Transaction Journal كامل وInterrupted Install Resume يحتاج إضافة سجل transaction صغير قبل Android Runtime release.

## 26. Debug Tools

توجد Events للحالة والتقدم والأخطاء، و`ContentSystem.md` يحدد Debug Screen المستقبلي. Console commands `content.list` و`content.scan` و`content.verify` لم تُربط إلى Unreal console handler بعد.

## 27. Package Builder Tool

`Scripts/Content/BuildALPack.py` ينشئ Manifest وALPACK وSize Report وTop 30 files، ويمنع Native/Executable files ويدعم Hard Size Limit.

## 28. Package Signing Tool

التوقيع اختياري Development ومطلوب عند `--require-signature`، باستخدام `openssl` ومفتاح خارجي. لا توجد مفاتيح خاصة في المشروع.

## 29. Size Audit

Report الحزمة يحتوي file count وcontent size وdownload size وTop Files. التقرير الحالي لحزمة Fixture الصغيرة، وليس تقرير Cooked Unreal Assets.

## 30. Android Tests

لم تُنفذ File Picker أو Import/Install/Mount على Android فعلي بسبب غياب Unreal وAndroid SDK/NDK. تمت تجربة Python ALPACK generation وفحص Manifest وArchive Contents وSHA-256 محليًا.

## 31. Import Test

تم إنشاء `Releases/Content/Development/AshLine_TestContent_v1.alpack` بمحتوى صغير، واحتوى `content/test_asset.txt` و`manifest.json` و`optional_metadata/build.json`، ونجح فحص Schema وPackage ID.

## 32. Invalid Package Tests

تم تجهيز Validator لفحص Invalid Manifest وUnsupported Schema وInvalid Signature وHashMismatch وIncompatible Version وMissing/Circular Dependency وInsufficient Storage وPath Traversal. اختبارات Runtime الفعلية تحتاج Unreal execution.

## 33. Restart/Recovery Tests

Recovery API وRegistry Rebuild موجودان، لكن Crash أثناء Finalization وAtomic Rename وAndroid process kill لم تُختبر فعليًا قبل توفر Unreal/Android.

## 34. Known Issues

Directory Mount هو Development backend وليس Pak/IoStore shipping mount. Android SAF bridge وRuntime cryptographic hash streaming وHTTP downloader وPlay Core غير مفعلة. كما يجب ربط `.uasset` وPrimary Asset Types داخل Unreal Editor قبل Cook.

## 35. Ready for real City Packages

**NO — ليس Runtime Production Ready بعد.** البنية الأساسية وALPACK Builder جاهزان للتطوير، لكن يلزم Unreal Cook/Mount وAndroid File Picker وRuntime SHA/Signature verification على Target.

## 36. Validation

نجحت الفحوصات المحلية:

```text
validate_content_system.py: PASS
validate_project.py: PASS
static_surface_check.py: PASS
git diff --check: PASS
```

## 37. APK Status

تجهيز APK متوقف حتى اعتماد هذا التحديث. البيئة الحالية لا تحتوي Unreal Engine أو Android SDK/NDK، ولذلك لا يمكن إنتاج APK صادق أو إعلان تثبيت/تشغيل فعلي قبل توفير Toolchain.
