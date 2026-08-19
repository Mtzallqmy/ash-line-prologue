# Prompt 02 — Mobile Touch Layer

أنشئ Widget Blueprint باسم `WBP_MobileTouchLayer` داخل هذا المجلد، واجعله طبقة إدخال مستقلة عن `AALPlayerCharacter`. يجب أن تستخدم عناصر الواجهة نفس Input Actions الموجودة في `IMC_Player` بدل استدعاء منطق Gameplay مخصص للهاتف.

| المنطقة | الوظيفة |
|---|---|
| يسار الشاشة | Virtual Movement Stick مرتبط بـ `IA_Move` |
| يمين الشاشة | Camera Drag Region مرتبط بـ `IA_Look` |
| زر علوي/مناسب | Jump عبر `IA_Jump` |
| زر مناسب | Crouch عبر `IA_Crouch` |
| زر مناسب | Sprint عبر `IA_Sprint` مع دعم Hold أو Toggle من `FALPlayerInputSettings` |
| زر مناسب | Interaction عبر `IA_Interact` |
| زر مناسب | Pause عبر `IA_Pause` |

يجب أن يكون التصميم Landscape وSafe-Zone aware وDPI-scaled وNotch-aware، وألا يعتمد على Pixel Positions ثابتة. استخدم Anchors وSize Boxes وSafe Zone Widget، واسمح بتزامن إصبع الحركة مع إصبع النظر ومع الضغط على زر في الوقت نفسه. لا تضف Fire أو Aim أو Reload أو Drone إلى Gameplay في Prompt 02؛ يمكن ترك أماكن محجوزة لها فقط.

من المهم ألا تتداخل منطقة النظر مع أزرار Jump وCrouch وSprint وInteract وPause، وأن يستمر عصا الحركة أثناء استخدام النظر أو الضغط على زر آخر.
