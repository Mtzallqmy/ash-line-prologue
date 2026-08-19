# L_CombatTest — Prompt 05

أنشئ خريطة Unreal باسم `L_CombatTest` لاختبار أول Combat Prototype. يجب أن تحتوي الخريطة على Player Spawn، وثلاثة أعداء من `BP_ALInfantryCharacter`، وجدران، ونقاط `BP_ALCoverPoint`، ومسار `BP_ALPatrolRoute`، وSpawner اختياري للتفعيل من Trigger أو Mission.

| الاختبار | النتيجة المطلوبة |
|---|---|
| Patrol | Spawn ثم Patrol ثم Wait ثم Continue دون رؤية اللاعب |
| Detection | Patrol ثم Suspicious ثم Alert ثم Combat عند استمرار الرؤية |
| Lost Sight | Combat ثم Last Known Location ثم Search ثم Returning ثم Patrol |
| Gunshot Hearing | Gunshot خارج Sight وداخل Hearing يحول العدو إلى Suspicious أو Alert |
| Damage Awareness | ضربة من الخلف تحدد مصدر الضرر وتبدأ Alert ثم Combat/Search |
| Enemy Fire | Aim ثم Burst ثم انخفاض صحة اللاعب عبر Weapon وHealth الحاليين |
| Cover | دخول Combat ثم Find/Reserve ثم Move ثم Fire |
| Death | موت العدو يوقف السلاح والحركة والقرار ويحرر Cover ويرسل OnEnemyKilled |
| Six AI | ستة أعداء نشطين في Combat مع تحديثات Timed مستقرة |
| Maximum Load | عشرة أعداء مع Full Auto وNavigation وCover وDamage وVFX للمراقبة فقط |

اختبارات Android وقياس Game Thread وRender Thread وGPU وFPS وMemory وAI Time وNavigation Time يجب تنفيذها على جهاز فعلي بعد Compile/Cook/Package.
