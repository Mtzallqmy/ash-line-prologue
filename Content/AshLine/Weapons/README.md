# Prompt 04 — Weapon Content

أنشئ ثلاث Primary Data Assets من نوع `UALWeaponDataAsset` داخل `Content/AshLine/Weapons/Data/`، ثم أنشئ Blueprint subclasses أو Weapon Actors خفيفة من `AALWeaponBase` عند الحاجة. لا تنشئ Class مستقلًا لكل سلاح؛ الاختلافات تأتي من البيانات.

| Data Asset | Weapon ID | النوع | Fire Mode | RPM | Magazine | Reserve | Damage | Range |
|---|---|---|---|---:|---:|---:|---:|---:|
| `DA_Weapon_AssaultRifle` | `WPN_AR_001` | AssaultRifle | FullAuto | 650 | 30 | 120 | 28 | 15000 |
| `DA_Weapon_SMG` | `WPN_SMG_001` | SMG | FullAuto | 850 | 32 | 128 | 21 | 9000 |
| `DA_Weapon_Pistol` | `WPN_PST_001` | Pistol | SemiAuto | 350 | 15 | 60 | 32 | 8000 |

تتضمن البيانات أيضًا Hip Fire Spread وADS Spread وMovement/Crouch Modifiers وFOV وAim Transition Time وRecoil Settings وHit Zone Profile وSoft References للصوت والـ Mesh والـ Muzzle FX. استخدم Textures بدقة 512–1024، ولا تضف حزمًا ضخمة أو 4K Assets في Prologue.

يبدأ اللاعب بسلاح `WPN_AR_001` عند ربطه في Blueprint، ويمكن استخدام `WPN_PST_001` كـ Sidearm و`WPN_SMG_001` كسلاح Primary بديل. يجب أن تكون `WeaponSocket` و`Muzzle` واضحة في الـ Mesh، وأن تبقى Input Actions وTouch Buttons خارج Weapon Actor.
