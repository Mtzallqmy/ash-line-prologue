#!/usr/bin/env python3
from pathlib import Path
import json
import sys

ROOT = Path(sys.argv[1] if len(sys.argv) > 1 else '.')
errors = []

def read(rel):
    path = ROOT / rel
    if not path.exists():
        errors.append(f'missing: {rel}')
        return ''
    return path.read_text(encoding='utf-8', errors='ignore')

weapon_h = read('Source/AshLineWeapons/Public/ALWeaponBase.h')
weapon_cpp = read('Source/AshLineWeapons/Private/ALWeaponBase.cpp')
data_h = read('Source/AshLineWeapons/Public/Data/ALWeaponData.h')
types_h = read('Source/AshLineWeapons/Public/Types/ALWeaponTypes.h')
component_h = read('Source/AshLineWeapons/Public/Components/ALWeaponComponent.h')
component_cpp = read('Source/AshLineWeapons/Private/Components/ALWeaponComponent.cpp')
recoil_h = read('Source/AshLineWeapons/Public/Components/ALRecoilComponent.h')
recoil_cpp = read('Source/AshLineWeapons/Private/Components/ALRecoilComponent.cpp')
player_h = read('Source/AshLineCharacters/Public/ALPlayerCharacter.h')
player_cpp = read('Source/AshLineCharacters/Private/ALPlayerCharacter.cpp')
controller_h = read('Source/AshLineCharacters/Public/ALPlayerController.h')
controller_cpp = read('Source/AshLineCharacters/Private/ALPlayerController.cpp')
weapon_build = read('Source/AshLineWeapons/AshLineWeapons.Build.cs')
characters_build = read('Source/AshLineCharacters/AshLineCharacters.Build.cs')

for token in ['WeaponMesh', 'MuzzlePoint', 'Data', 'StartFire', 'StopFire', 'StartReload', 'CompleteReload', 'CancelReload', 'StartAim', 'StopAim', 'SetWeaponInputEnabled', 'FireAtTarget']:
    if token not in weapon_h or token not in weapon_cpp: errors.append(f'WeaponBase missing {token}')
for token in ['WeaponId', 'DisplayName', 'WeaponType', 'FireMode', 'Damage', 'RoundsPerMinute', 'MagazineSize', 'ReserveAmmo', 'ReloadTime', 'Range', 'HipFireSpread', 'ADSSpread', 'RecoilSettings', 'HitZones']:
    if token not in data_h: errors.append(f'weapon data missing {token}')
for token in ['SemiAuto', 'FullAuto', 'AssaultRifle', 'SMG', 'Pistol', 'Reloading']:
    if token not in types_h: errors.append(f'weapon type/state missing {token}')
for token in ['EquipWeapon', 'UnequipCurrentWeapon', 'SwitchToNextWeapon', 'StartFire', 'StopFire', 'StartReload', 'StartAim', 'StopAim', 'OnAmmoChanged']:
    if token not in component_h or token not in component_cpp: errors.append(f'weapon component missing {token}')
for token in ['ApplyRecoil', 'RecoverRecoil', 'MobileRecoilScale', 'PrimaryComponentTick.bCanEverTick = false']:
    if token not in recoil_h and token not in recoil_cpp: errors.append(f'recoil component missing {token}')
for token in ['UALWeaponComponent', 'StartFire', 'StopFire', 'StartAim', 'StopAim', 'ReloadWeapon', 'SwitchWeapon']:
    if token not in player_h or token not in player_cpp: errors.append(f'player weapon integration missing {token}')
for token in ['FireAction', 'AimAction', 'ReloadAction', 'NextWeaponAction', 'HandleFireStarted', 'HandleAimStarted', 'HandleReloadStarted']:
    if token not in controller_h or token not in controller_cpp: errors.append(f'controller combat input missing {token}')
for token in ['FALDamageData', 'EALDamageType::Bullet', 'ApplyDamage', 'HitLocation', 'BoneName']:
    if token not in weapon_cpp: errors.append(f'weapon damage integration missing {token}')
if 'PrimaryComponentTick.bCanEverTick = true' in weapon_cpp or 'TickComponent' in weapon_cpp: errors.append('WeaponBase should not use a permanent Tick')
if 'AshLineCharacters' in weapon_build: errors.append('Weapons must not depend on Characters or create a circular dependency')
if 'AshLineWeapons' not in characters_build: errors.append('Characters must depend on Weapons for Player WeaponComponent')

catalog = ROOT / 'Content/AshLine/Data/weapon_catalog.json'
if catalog.exists():
    data = json.loads(catalog.read_text())
    weapons = data.get('weapons', [])
    ids = {w.get('id') for w in weapons}
    for expected in ['WPN_AR_001', 'WPN_SMG_001', 'WPN_PST_001']:
        if expected not in ids: errors.append(f'catalog missing {expected}')
    if len(ids) != len(weapons): errors.append('duplicate weapon IDs in catalog')
else: errors.append('missing weapon catalog')

manifest_path = ROOT / 'Content/AshLine/Input/input_asset_manifest.json'
if manifest_path.exists():
    manifest = json.loads(manifest_path.read_text())
    implemented = set(manifest.get('mappingContext', {}).get('implementedActions', []))
    for action in ['IA_Fire', 'IA_Aim', 'IA_Reload', 'IA_NextWeapon']:
        if action not in implemented: errors.append(f'input manifest missing active action {action}')
else: errors.append('missing input manifest')

for path in ROOT.rglob('*'):
    if path.is_file() and path.suffix in {'.h', '.cpp', '.cs', '.ini'}:
        text = path.read_text(encoding='utf-8', errors='ignore')
        if any(token in text for token in ['AALAssaultRifle', 'AALSMG', 'AALPistol']): errors.append(f'per-weapon class detected in {path}')
        if 'FALDamageEvent' in text: errors.append(f'legacy damage event remains in {path}')

print('Prompt 04 validation')
print('====================')
print('Status:', 'PASS' if not errors else 'FAIL')
for error in errors: print('ERROR:', error)
sys.exit(1 if errors else 0)
