# Enhanced Input Assets — Prompt 02 and Prompt 04

أنشئ هذه الأصول من داخل Unreal Editor باستخدام Enhanced Input، ثم اربطها بخصائص `AALPlayerController` داخل Blueprint Controller عند الحاجة:

| الأصل | النوع | الحالة |
|---|---|---|
| `IMC_Player` | Input Mapping Context | مطلوب |
| `IA_Move` | Input Action / Axis2D | منفذ |
| `IA_Look` | Input Action / Axis2D | منفذ |
| `IA_Jump` | Input Action / Boolean | منفذ |
| `IA_Crouch` | Input Action / Boolean | منفذ |
| `IA_Sprint` | Input Action / Boolean | منفذ |
| `IA_Interact` | Input Action / Boolean | منفذ |
| `IA_Pause` | Input Action / Boolean | منفذ |
| `IA_Fire` | Input Action / Boolean | منفذ في Prompt 04 ويدعم Hold |
| `IA_Aim` | Input Action / Boolean | منفذ في Prompt 04 ويدعم Hold |
| `IA_Reload` | Input Action / Boolean | منفذ في Prompt 04 |
| `IA_NextWeapon` | Input Action / Boolean | منفذ في Prompt 04 |
| `IA_Drone` | Input Action / Boolean | محجوز للمرحلة التالية |

على PC استخدم WASD للحركة، Mouse X/Y للنظر، Space للقفز، Shift للركض، C أو Ctrl للانحناء، E للتفاعل، Esc للإيقاف المؤقت، Left Mouse Button للإطلاق، Right Mouse Button للتصويب، R لإعادة التعبئة، وMouse Wheel أو مفتاحي 1/2 لتبديل السلاح. على Android اربط `IA_Move` بعصا الحركة اليسرى و`IA_Look` بمنطقة السحب اليمنى، وأضف أزرار Fire وAim وReload وNextWeapon إلى Touch Layer.

يجب أن تمر كل الأفعال عبر نقاط الدخول نفسها في C++؛ لا تنشئ منطق حركة أو رماية منفصلًا للهاتف أو للكمبيوتر. لا تضع تفاصيل السلاح داخل Input Action أو Widget.
