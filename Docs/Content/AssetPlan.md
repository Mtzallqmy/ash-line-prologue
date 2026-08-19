# Asset Plan — Namar Prologue

يجب إنشاء Modular Kit صغير ومشترك بدل Material مستقل لكل مبنى. استخدم دقة 512–1024 افتراضيًا، مع 2K فقط لحالات بصرية مبررة، واعتمد Trim Sheets وTexture Atlases وMaterial Instances وChannel Packing وDecals قابلة لإعادة الاستخدام.

| المجموعة | أمثلة | Chunk | ملاحظات |
|---|---|---:|---|
| City Kit | SM_Wall_A, SM_Window_A, SM_Door_A, SM_Roof_A | 1 | بناء شارع وأزقة وسوق |
| Characters | SK_Player, SK_Infantry | 2 | رسوم متحركة مشتركة قدر الإمكان |
| Weapons | DA_AssaultRifle, DA_SMG, DA_Pistol | 2 | WeaponBase واحد وبيانات مختلفة |
| Drone | BP_ScoutDrone, MI_Drone | 1 | بطارية وحساس وكاميرا |
| Audio | S_Fire_*, S_Reload_* | 3 | ملفات مضغوطة للموبايل |
| UI | WBP_MainMenu, WBP_MissionHUD, WBP_DroneHUD | 0 | لا تستخدم خلفيات فيديو كبيرة |
