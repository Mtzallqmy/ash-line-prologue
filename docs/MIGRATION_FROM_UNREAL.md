# Migration from Unreal prototype to Expo

تم استبدال تطبيق Unreal السابق بنسخة Expo / React Native مستقلة لأن Expo لا يستطيع تشغيل `ASH_LINE.uproject` أو C++ الخاص بـUnreal مباشرة.

## ما تم نقله

- اسم اللعبة والهوية العامة: ASH LINE — Prologue.
- منطقة Namar Training District كمسرح للنسخة التجريبية.
- ثلاث مراحل: Arrival، First Contact، Eye Above.
- Health/Damage/Death/Restart.
- ثلاثة أسلحة ببيانات قريبة من النموذج الأصلي: AR وSMG وPistol.
- Enemy AI خفيف: اقتراب، مسافة اشتباك، إطلاق نار، إصابة وموت.
- Cover يوقف المقذوفات.
- Scout Drone scan في المهمة الثالثة.
- Extraction وPrologue Complete.

## ما تغير

النسخة الجديدة لعبة تكتيكية ثنائية الأبعاد Top-down مبنية بعناصر React Native، وليست منفذًا ثلاثي الأبعاد لمحرك Unreal. لا يمكن نقل ملفات `.uasset/.umap` أو Modules C++ وتشغيلها داخل Expo.

## Android

- Package: `com.ashline.game`
- Version: `0.0.1`
- Min SDK: 26 (Android 8.0)
- Target/Compile SDK: 36
- ABI: `arm64-v8a` فقط
- Orientation: Landscape

## Builds

- `preview`: APK قابل للتثبيت مباشرة.
- `production`: AAB للنشر عبر Google Play.
