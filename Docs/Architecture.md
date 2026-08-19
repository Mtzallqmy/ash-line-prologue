# ASH LINE Architecture

## Runtime modules

| Module | Responsibility | Intended extension |
|---|---|---|
| AshLineCore | Save, settings, asset manager, damage contracts | Shared gameplay foundation |
| AshLineCharacters | Player and reusable actor components | Vehicles and civilians |
| AshLineCombat | Central damage subsystem | Vehicle and environmental damage |
| AshLineWeapons | Data-driven WeaponBase | 20–50 weapons without new classes |
| AshLineAI | Lightweight infantry state machine | Additional enemy archetypes |
| AshLineDrones | Drone base and sensor/battery components | Combat and support drones |
| AshLineMissions | Mission data and lifecycle | City_02 and optional missions |
| AshLineContent | Manifest, local delivery, chunks | CDN/PAD implementations |
| AshLineUI | HUD extension points | Menu, HUD, drone, settings widgets |
| AshLinePlatform | Android/Windows abstraction boundary | Storage, notifications, PAD |

القلب البرمجي مكتوب في C++، بينما المحتوى المرتفع المستوى يمكن ربطه عبر Blueprint subclasses وData Assets. لا يعتمد Gameplay على مسارات فعلية للملفات أو على ZIP extraction.
