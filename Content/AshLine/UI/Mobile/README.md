# Mobile Touch Layer — Prompt 02 and Prompt 04

أنشئ Widget Blueprint باسم `WBP_MobileTouchLayer` داخل هذا المجلد، واجعله طبقة إدخال مستقلة عن `AALPlayerCharacter` و`UALWeaponComponent`. تستخدم عناصر الواجهة نفس Input Actions الموجودة في `IMC_Player`، ولا تستدعي منطق الرماية مباشرة.

| المنطقة أو الزر | الوظيفة |
|---|---|
| يسار الشاشة | Virtual Movement Stick مرتبط بـ `IA_Move` |
| يمين الشاشة | Camera Drag Region مرتبط بـ `IA_Look` |
| Jump | `IA_Jump` |
| Crouch | `IA_Crouch` |
| Sprint | `IA_Sprint` مع Hold أو Toggle من إعدادات اللاعب |
| Interact | `IA_Interact` |
| Pause | `IA_Pause` |
| Fire | `IA_Fire` مع Press/Hold/Release لدعم Full Auto |
| Aim | `IA_Aim` مع Hold-to-Aim |
| Reload | `IA_Reload` |
| Weapon Switch | `IA_NextWeapon` |

يجب أن يكون التصميم Landscape وSafe-Zone aware وDPI-scaled وNotch-aware، وألا يعتمد على Pixel Positions ثابتة. استخدم Anchors وSize Boxes وSafe Zone Widget، واسمح بتزامن إصبع الحركة مع إصبع النظر ومع الضغط على Fire أو Aim أو زر آخر في الوقت نفسه.

يجب ألا تتداخل منطقة النظر مع أزرار Jump وCrouch وSprint وInteract وPause وFire وAim وReload وWeapon Switch. تستمر عصا الحركة أثناء استخدام النظر أو الضغط على زر آخر. تظل تفاصيل السلاح داخل `AALPlayerCharacter` و`UALWeaponComponent` و`AALWeaponBase`، بينما يقتصر Widget على إرسال Input Actions.
