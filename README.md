# ASH LINE — Prologue

هذا المستودع هو أساس مرحلة Phase 1 للعبة تكتيكية ثلاثية الأبعاد مبنية على Unreal Engine 5.4، مع أولوية Android ودعم قابل للتوسعة إلى Windows. تم فصل الأنظمة إلى Modules مستقلة، واعتماد Data-Driven Gameplay، وتصميم Content Delivery يعمل محليًا في هذه المرحلة ويمكن تبديل تنفيذه لاحقًا إلى CDN أو Play Asset Delivery.

## حدود التنفيذ الحالية

بيئة التنفيذ الحالية لا تحتوي على Unreal Editor أو Unreal Build Tool، لذلك تم إنشاء **مشروع مصدر Unreal قابل للفتح والتجميع** مع كل الهيكل البرمجي وملفات الإعداد والـ manifests وأدوات التدقيق. لا يمكن من داخل هذه البيئة تنفيذ Cook أو Packaging أو تشغيل Android فعليًا دون تثبيت Unreal Engine وAndroid SDK/NDK وربط مجلد عمل محلي.

## فتح المشروع

افتح `ASH_LINE.uproject` في Unreal Engine 5.4 أو أحدث، ثم أنشئ المحتوى المرئي داخل مجلدات `Content/AshLine/` وفق `Docs/Content/AssetPlan.md`. بعد توليد ملفات المشروع، نفّذ البناء من محرر Unreal أو من `Scripts/Build/BuildPrologue.sh` على جهاز يحتوي على Unreal Engine.

## التحقق المحلي المتاح

```bash
python3 Scripts/Validation/validate_project.py .
python3 Scripts/Validation/size_report.py .
```

الأدوات لا تعتبر المشروع ناجحًا لمجرد وجود الملفات؛ فهي تتحقق من أسماء الـ modules، ووجود الـ manifests، وميزانية الحجم، والمجلدات المطلوبة، وتنتج تقارير قابلة للمراجعة.
