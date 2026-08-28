# سلسلة أدوات Godot وAndroid الخفيفة

## ما يُعاد استخدامه وما لا يُثبت

يستخدم هذا المسار محرر Godot القياسي المحمول 4.7.2 على Windows مع GDScript، وليس إصدار .NET؛ لذلك لا يحتاج Visual Studio أو .NET SDK. المحرر ملف مضغوط ذاتي الاحتواء، ويمكن تشغيله بعد الاستخراج. [1]

لا يعتمد المشروع على Unreal Engine أو Unreal Editor أو RunUAT أو BuildCookRun. تُضبط قوالب التصدير باسم preset محفوظ في `export_presets.cfg`، ويمكن لتشغيل الطرفية أن يستخدم `--path` مع `--export-debug` أو `--export-release` لإنتاج APK. [2]

| المكوّن | القرار | المبرر |
|---|---|---|
| Godot 4.7.2 القياسي المحمول | مطلوب | محرر وتشغيل وتصدير GDScript؛ لا مثبّت ولا .NET. |
| `android_debug.apk` و`android_release.apk` | مطلوبان | قالب الاختبار وقالب الإصدار الأساسيان لـAndroid. |
| `android_source.zip` | مستبعد في المرحلة الأولى | مطلوب فقط لمسار Gradle المخصص؛ لا يوجد مكوّن Android أصلي أو إضافة تحتاجه حالياً. |
| Android Studio | غير مطلوب | تستخدم الحزم النصية فقط عبر Android SDK. |
| Visual Studio / Compiler | غير مطلوب لمسار Godot GDScript | لا يوجد C# أو GDExtension أو بناء محرك مخصص. |

## أحجام القوالب الفعلية

تعرّف حزمة قوالب Godot الرسمية ملفات Android الثلاثة `android_debug.apk` و`android_release.apk` و`android_source.zip`. تم تحليل دليل ZIP المركزي للحزمة الرسمية لاختيار القالبين الأولين فقط. [3]

| الملف | الحجم المضغوط داخل الحزمة | القرار |
|---|---:|---|
| `android_debug.apk` | 125,834,876 بايت، 120.0 MiB | يُنزّل |
| `android_release.apk` | 103,949,600 بايت، 99.1 MiB | يُنزّل |
| `android_source.zip` | 214,421,853 بايت، 204.5 MiB | لا يُنزّل حالياً |
| قوالب Android المطلوبة فقط | 229,784,476 بايت، 219.1 MiB | الحد الأدنى المختار |

## Android SDK

توصي وثائق Godot الحالية بـOpenJDK 17 وتسمح باستخدام `sdkmanager` بدلاً من Android Studio. وتشير إلى Platform Tools وBuild Tools وAndroid Platform وCommand-line Tools وCMake وNDK المطلوبة لمسار Android الرسمي. [4] سيُنفذ المشروع مبدأ **الكشف ثم إعادة الاستخدام ثم تثبيت الناقص فقط**، مع اختبار التصدير الحقيقي قبل تنزيل نسخة إضافية من أي مكوّن.

## المراجع

[1] [تنزيل Godot 4 لنظام Windows](https://godotengine.org/download/windows/)

[2] [تصدير مشاريع Godot من سطر الأوامر](https://docs.godotengine.org/en/latest/tutorials/export/exporting_projects.html)

[3] [مصدر مدير قوالب التصدير في Godot — قائمة قوالب Android](https://github.com/godotengine/godot/blob/4.7/editor/export/export_template_manager.cpp)

[4] [تصدير Godot إلى Android](https://docs.godotengine.org/en/stable/tutorials/export/exporting_for_android.html)
