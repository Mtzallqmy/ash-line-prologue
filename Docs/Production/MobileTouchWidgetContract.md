# عقد واجهة اللمس — WBP_MobileTouchLayer

## الغرض

ينشئ `AALPlayerController` الواجهة `MobileTouchWidgetClass` عند بدء اللعب. توفر الدوال التالية عقداً مستقراً من UMG إلى C++ حتى تستخدم واجهة الهاتف منطق الشخصية نفسه ولا تعتمد على محاكاة لوحة مفاتيح أو ماوس.

| عنصر UMG | الحدث | دالة المتحكم | القيمة |
|---|---|---|---|
| عصا الحركة اليسرى | كل تحديث للسحب | `SubmitMobileMove` | `FVector2D` ضمن `[-1, 1]` |
| عصا النظر اليمنى | كل تحديث للسحب | `SubmitMobileLook` | `FVector2D` لمقدار السحب منذ التحديث السابق |
| زر إطلاق | Pressed / Released | `SetMobileFireHeld` | `true` ثم `false` |
| زر تصويب | Pressed / Released | `SetMobileAimHeld` | `true` ثم `false` |
| زر ركض | Pressed / Released | `SetMobileSprintHeld` | `true` ثم `false` |
| تعبئة | Pressed | `TriggerMobileReload` | لا قيمة |
| تبديل سلاح | Pressed | `TriggerMobileSwitchWeapon` | لا قيمة |
| تفاعل | Pressed | `TriggerMobileInteract` | لا قيمة |
| انحناء | Pressed | `TriggerMobileCrouch` | لا قيمة |
| إيقاف مؤقت | Pressed | `TriggerMobilePause` | لا قيمة |

## تنفيذ UMG المطلوب

تحتوي الواجهة على Canvas Panel كامل الشاشة في الوضع الأفقي. تكون عصا الحركة ضمن الربع السفلي الأيسر، وعصا النظر ومنطقة الإطلاق ضمن الربع السفلي الأيمن. يجب أن تعيد العصا قيمة صفرية عند `OnTouchEnded`، وأن تستدعي `SetMobileFireHeld(false)` و`SetMobileAimHeld(false)` عند إخفاء الواجهة أو فقدان التركيز حتى لا يستمر اللاعب في الحركة أو الإطلاق بلا قصد.

تظهر الصحة والذخيرة والهدف النشط في مناطق علوية آمنة لا تتداخل مع الكاميرا أو أزرار الإبهام. الحد الأدنى للمساحة النشطة لأي زر 48dp، ويجب ألا يعتمد فهم التهديدات على اللون وحده؛ تستخدم الرموز والنصوص والحالات المرئية معاً.
