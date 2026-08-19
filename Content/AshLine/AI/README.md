# Prompt 05 — Enemy AI Content

أنشئ `DA_AI_Soldier_Basic` و`DA_AI_Soldier_Trained` من نوع `UALAIArchetypeData`، واربط كل واحد بـ `UALHealthConfig` و`UALWeaponDataAsset` المناسب. لا تنشئ Classes منفصلة للجندي الأساسي والمدرّب؛ الاختلافات تأتي من Data Asset.

| الأصل | Archetype ID | Weapon ID | Reaction | Accuracy | Cover Preference |
|---|---|---|---:|---:|---:|
| `DA_AI_Soldier_Basic` | `AI_Soldier_Basic` | `WPN_AR_001` | 0.55s | 0.55 | 0.25 |
| `DA_AI_Soldier_Trained` | `AI_Soldier_Trained` | `WPN_AR_001` | 0.35s | 0.72 | 0.65 |

أنشئ Blueprint مشتقًا من `AALInfantryCharacter` باسم `BP_ALInfantryCharacter`، واربط `WeaponClass` بـ Blueprint مشتق من `AALWeaponBase` و`Archetype` بأحد Data Assets. يضيف العدو Health وWeapon وAI State وSense وCombat وCover وSignificance Components تلقائيًا.

أنشئ `BP_ALPatrolRoute` من `AALPatrolRoute` وضع Patrol Points وWait Times وLoop. أنشئ `BP_ALCoverPoint` من `AALCoverPoint` وحدد `StandingCover` أو `CrouchCover`. استخدم `AALEnemySpawner` للتفعيل من Mission أو Trigger مع Count وSpawn Points بدل ملء المستوى بالأعداء منذ البداية.

المسار التشغيلي هو Perception ثم Awareness ثم State ثم Decision Timer ثم Navigation/Cover ثم WeaponComponent ثم Damage System. الحالات المدعومة هي Idle وPatrol وSuspicious وAlert وCombat وSearch وReturning وDead. لا تضف Behavior Tree Services تعمل كل Frame، ولا تنشئ Squad AI أو Flanking أو Grenade AI في هذه المرحلة.

أنشئ خريطة أو مستوى اختبار باسم `L_CombatTest` داخل Unreal Editor، وضع Player Spawn وثلاثة أعداء وجدران وCover Points وPatrol Route. اختبر Patrol، Detection، Lost Sight، Gunshot Hearing، Damage Awareness، Enemy Fire، Cover، Death، وستة أعداء، ثم حمولة عشرة أعداء كاختبار ضغط.
