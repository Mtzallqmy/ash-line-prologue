# ASH LINE Combat Prototype v0.0.1 — Test Report

> **CODE READY / BUILD BLOCKED** — Unreal Editor and Android runtime tests were not available in the execution environment.

| Test | Result |
|---|---|
| C++ source validation | PASS |
| Project/module/config validation | PASS |
| Build-reference validation | PASS with Engine-map existence deferred to installed UE |
| UnrealHeaderTool | NOT RUN — Unreal unavailable |
| Editor Compile | NOT RUN — Unreal unavailable |
| Real `.umap` exists | NO — generated only by Unreal Editor Python script |
| Real `.uasset` input/data assets exist | NO — generated only by Unreal Editor Python script |
| Player spawn | NOT RUN |
| PC movement/look/jump/crouch/sprint | NOT RUN |
| Android touch move + look + fire | NOT RUN |
| Aim/fire/reload/switch weapon | NOT RUN |
| Damage/health/death/restart | NOT RUN in runtime |
| AI patrol/detect/alert/cover/shoot/search/death | NOT RUN in runtime |
| Prototype completion after enemy count reaches zero | Source hook added; runtime NOT RUN |
| Pause | NOT RUN |
| 3-AI FPS/RAM | NOT MEASURED |
| 6-AI FPS/RAM | NOT MEASURED |
| Android install/launch | NOT RUN |

Run the real runtime matrix only after creating assets with `Scripts/Editor/CreatePrototypeAssets.py`, compiling the Editor target, cooking the arena, and producing an APK.
