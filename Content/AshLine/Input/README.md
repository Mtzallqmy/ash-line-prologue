# Prompt 02 — Enhanced Input Assets

أنشئ هذه الأصول من داخل Unreal Editor باستخدام Enhanced Input:

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
| `IA_Fire` | Input Action / Boolean | محجوز للمرحلة التالية |
| `IA_Aim` | Input Action / Boolean | محجوز للمرحلة التالية |
| `IA_Reload` | Input Action / Boolean | محجوز للمرحلة التالية |
| `IA_Drone` | Input Action / Boolean | محجوز للمرحلة التالية |

ضع الـ assets النهائية داخل هذا المجلد، واربطها بخصائص `AALPlayerController` في Blueprint مشتق. القيم المطلوبة للمحاكاة على PC هي WASD للحركة، Mouse X/Y للنظر، Space للقفز، Shift للركض، C أو Ctrl للانحناء، E للتفاعل، وEsc للإيقاف المؤقت. في Android اربط IA_Move بعصا الحركة اليسرى وIA_Look بمنطقة السحب اليمنى.

يجب أن تكون كل الأفعال مرتبطة بنقاط الدخول نفسها في C++؛ لا تنشئ منطق حركة منفصلًا للهاتف أو للكمبيوتر.
