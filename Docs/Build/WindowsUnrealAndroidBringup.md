# دليل تهيئة وتشغيل ASH LINE على Windows وAndroid

## الهدف

هذا الدليل هو نقطة التحول من مراجعة مصدرية ساكنة إلى تشغيل لعبة Unreal ثلاثية الأبعاد فعلياً. لا يشغّل أي أمر تلقائياً؛ تُنفّذ الخطوات فقط على جهاز Windows متصل، بعد تثبيت المتطلبات والتحقق من مساراتها.

## المتطلبات

| الفئة | المطلب | التحقق |
|---|---|---|
| نظام التشغيل | Windows 10 أو 11، 64-bit | جلسة محلية أو runner متصل يبقى متاحاً طوال البناء |
| المحرك | Unreal Engine 5.4.4 أو إصدار لاحق من سلسلة 5.4 | وجود `UnrealEditor.exe` و`UnrealEditor-Cmd.exe` |
| بيئة C++ | Visual Studio 2022 مع Desktop development with C++ وWindows SDK | قدرة Unreal على إنشاء وتحديث ملفات المشروع وتجميع Editor |
| Android | Android SDK Platform 34 وBuild Tools 34.0.0 | تعرف Unreal على SDK من Project Settings أو متغيرات البيئة |
| NDK | Android NDK `25.1.8937393` | تعرف Unreal على NDK المحدد من Project Settings |
| Java | JDK 17 | تعرف Unreal على مسار `JAVA_HOME` |
| جهاز اختبار | هاتف Android ARM64 مع تفعيل Developer Options وUSB debugging | يظهر الجهاز في `adb devices` ويقبل تثبيت APK Development |

## إعداد البيئة

استنسخ الفرع `import/unreal-apk-ready-source` محلياً. اضبط مسارات Unreal وAndroid وNDK وJava عبر Project Settings في Unreal، أو مررها صراحةً إلى `Scripts/Build/BuildFirstAPK.ps1` باستخدام الوسائط `PreferredUnrealRoot` و`PreferredAndroidHome` و`PreferredAndroidNdkHome` و`PreferredJavaHome`. لا تضف مفاتيح توقيع أو مسارات شخصية إلى المستودع.

قبل البناء، افتح `ASH_LINE.uproject` مرة واحدة في Unreal وأنشئ ملفات المشروع عند الطلب. شغّل `Scripts/Build/ValidateBeforeBuild.ps1` للتحقق من المتطلبات، ثم شغّل `Scripts/Build/BuildFirstAPK.ps1 -Configuration Development`. ينشئ السكربت الأصول الكتلية داخل المحرر، يضبط خريطة القتال، ويجهز APK ARM64 Development. لا تشغّل Shipping قبل أن يجتاز إصدار Development اختبارات اللعب والأداء.

## اختبار الـVertical Slice

| اختبار | الإجراء | نتيجة القبول |
|---|---|---|
| فتح المستوى | افتح `L_CombatPrototype` من Unreal Editor | لا توجد أخطاء Blueprint أو مراجع أصول مفقودة |
| حركة اللمس | اربط `WBP_MobileTouchLayer` بعقد التحكم اللمسي | العصا اليسرى للحركة واليمنى للنظر، مع توقف صحيح عند رفع الإصبع |
| القتال | جرّب إطلاقاً وتصويباً وتعبئة وتبديل سلاح تحت ضغط | لا يستمر إطلاق أو تصويب بعد رفع الزر وتُحدّث الذخيرة في HUD |
| المهمة | اقتل الأعداء المسجلين | يرتفع تقدم الهدف إلى 100% وتعرض الواجهة حالة الاكتمال |
| الحفظ | أعد تشغيل التطبيق بعد إنهاء المهمة | تستعاد حالة المهمة من فتحة `ASH_LINE_Profile` |
| الأداء | قياس 10 دقائق على جهاز Android مستهدف | متوسط مستقر عند 30 FPS، ولا توجد زيادة ذاكرة متصاعدة |

## المخرجات المتوقعة

عند النجاح يجب أن يظهر APK Development باسم `AshLine_CombatPrototype_v0.0.1_android_arm64_development.apk` ضمن `Releases/Android/0.0.1/APK/`، مع تقرير تحقق في `Releases/Android/0.0.1/Reports/`. بعد اختبار الجهاز الحقيقي، توثّق النتائج والأعطال ومعدل الإطارات قبل فتح مرحلة الأصول النهائية أو بدء توقيع Shipping.

> لا يُعد اجتياز الفحوصات الساكنة دليلاً على نجاح UnrealHeaderTool أو Cook أو Package أو الأداء على هاتف Android. تتطلب هذه النقطة بيئة Windows حقيقية مزودة بالمحرك ومتطلبات Android.
