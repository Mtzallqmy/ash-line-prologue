# ASH LINE — Prologue (Expo)

نسخة Expo / React Native قابلة للبناء من **ASH LINE — Prologue**. تم تحويل المشروع من أساس Unreal غير قابل للبناء داخل البيئات السحابية العادية إلى تطبيق Expo مستقل يمكن بناؤه باستخدام **EAS Build** من دون تثبيت Unreal Engine.

## النسخة الحالية

تقدم النسخة `0.0.1` نموذجًا تكتيكيًا علويًا قابلًا للعب في وضع Landscape:

1. **Arrival** — التحرك إلى نقطة التجمع.
2. **First Contact** — الاشتباك مع أربعة أعداء.
3. **Eye Above** — استخدام مسح الدرون لكشف الأهداف ثم التوجه إلى الاستخراج.

توجد ثلاثة أسلحة Data-driven داخل `src/data/gameData.js`: Assault Rifle وSMG وPistol، مع Health وAmmo وReload وWeapon Switching وEnemy AI وCover وDeath/Restart.

## المتطلبات

- Node.js 22.13+
- npm
- حساب Expo لإنشاء EAS builds

## التشغيل محليًا

```bash
npm install
npx expo start
```

لأن SDK 57 أحدث من نسخة Expo Go العامة في بعض الفترات، استخدم Development/Preview build عند الحاجة.

## إعداد EAS لأول مرة

```bash
npm install -g eas-cli
eas login
eas init
```

خذ Project ID الناتج وضعه كمتغير بيئة عند البناء أو كـGitHub Actions variable باسم `EXPO_PROJECT_ID`.

## إنشاء APK تجريبي

```bash
eas build --platform android --profile preview
```

`preview` مضبوط على `android.buildType = apk`، لذلك ينتج ملف APK يمكن تثبيته مباشرة على جهاز Android.

## إنتاج AAB للنشر على Google Play

```bash
eas build --platform android --profile production
```

ملف `production` ينتج Android App Bundle (`.aab`).

## Android target

| الإعداد | القيمة |
|---|---|
| Package | `com.ashline.game` |
| Version | `0.0.1` |
| Min SDK | 26 — Android 8.0 |
| Compile SDK | 36 |
| Target SDK | 36 |
| ABI | `arm64-v8a` فقط |
| Orientation | Landscape |

يتم فرض هذه القيم بواسطة `expo-build-properties` داخل `app.config.js`.

## GitHub Actions

يوجد Workflow:

```text
.github/workflows/eas-android.yml
```

قبل تشغيله أضف إلى المستودع:

- Secret: `EXPO_TOKEN`
- Variable: `EXPO_PROJECT_ID`

ثم:

`Actions → Expo Android Build → Run workflow → preview`

بعد نجاح Preview build سيظهر APK كـArtifact باسم `ash-line-preview-apk`.

## بنية المشروع

```text
App.js
src/
  components/
  data/
  game/
  screens/
app.config.js
eas.json
.github/workflows/eas-android.yml
```

راجع `docs/MIGRATION_FROM_UNREAL.md` لمعرفة ما تم تحويله من نموذج Unreal السابق.
