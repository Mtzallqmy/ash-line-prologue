#!/usr/bin/env python3
from pathlib import Path
import sys

ROOT = Path(sys.argv[1] if len(sys.argv) > 1 else '.')
errors = []

def read(rel):
    path = ROOT / rel
    if not path.exists():
        errors.append(f'missing: {rel}')
        return ''
    return path.read_text(encoding='utf-8', errors='ignore')

health_h = read('Source/AshLineCombat/Public/Components/ALHealthComponent.h')
health_cpp = read('Source/AshLineCombat/Private/Components/ALHealthComponent.cpp')
damage_data = read('Source/AshLineCombat/Public/Damage/ALDamageData.h')
damage_types = read('Source/AshLineCombat/Public/Damage/ALDamageTypes.h')
damage_receiver = read('Source/AshLineCombat/Public/Interfaces/ALDamageReceiver.h')
subsystem_h = read('Source/AshLineCombat/Public/ALDamageSystemSubsystem.h')
subsystem_cpp = read('Source/AshLineCombat/Private/ALDamageSystemSubsystem.cpp')
player_h = read('Source/AshLineCharacters/Public/ALPlayerCharacter.h')
player_cpp = read('Source/AshLineCharacters/Private/ALPlayerCharacter.cpp')
infantry_cpp = read('Source/AshLineAI/Private/ALInfantryCharacter.cpp')
combat_build = read('Source/AshLineCombat/AshLineCombat.Build.cs')
characters_build = read('Source/AshLineCharacters/AshLineCharacters.Build.cs')
ai_build = read('Source/AshLineAI/AshLineAI.Build.cs')

for token in ['MaxHealth', 'CurrentHealth', 'bIsDead', 'ApplyDamage', 'ApplyHealing', 'ResetHealth', 'GetHealthPercentage', 'OnHealthChanged', 'OnDamageReceived', 'OnHealingReceived', 'OnDeath', 'SetInvulnerable']:
    if token not in health_h or token not in health_cpp: errors.append(f'health API missing {token}')
for token in ['BaseDamage', 'DamageType', 'Instigator', 'DamageCauser', 'HitLocation', 'HitNormal', 'BoneName', 'bHasHitResult']:
    if token not in damage_data: errors.append(f'damage data missing {token}')
for token in ['Generic', 'Bullet', 'Fall', 'Environmental']:
    if token not in damage_types: errors.append(f'damage type missing {token}')
for token in ['CanDamageTarget', 'ApplyDamage', 'ApplyPointDamage', 'FindComponentByClass']:
    if token not in subsystem_h and token not in subsystem_cpp: errors.append(f'damage routing missing {token}')
for token in ['UALDamageReceiver', 'CanReceiveALDamage', 'ReceiveALDamage']:
    if token not in damage_receiver: errors.append(f'damage receiver interface missing {token}')
for token in ['PrimaryComponentTick.bCanEverTick = false', 'bIsDead', 'OnDeath.Broadcast']:
    if token not in health_cpp: errors.append(f'health performance/death guarantee missing {token}')
for token in ['OnDeath.AddDynamic', 'HandleHealthDeath', 'RequestRestartFromCheckpoint', 'HandleFallDamage', 'EALDamageType::Fall']:
    if token not in player_cpp: errors.append(f'player death/fall integration missing {token}')
for token in ['Components/ALHealthComponent.h', 'OnDeath.AddDynamic', 'DisableMovement']:
    if token not in infantry_cpp: errors.append(f'infantry death integration missing {token}')
if 'AshLineCharacters' in combat_build or 'AshLineAI' in combat_build:
    errors.append('Combat module has an upward dependency that may create a circular architecture')
if 'AshLineCombat' not in characters_build: errors.append('Characters does not depend on Combat')
if 'AshLineCombat' not in ai_build: errors.append('AI does not depend on Combat')

for path in ROOT.rglob('*'):
    if path.is_file() and path.suffix in {'.h', '.cpp', '.cs', '.ini'}:
        text = path.read_text(encoding='utf-8', errors='ignore')
        if 'FALDamageEvent' in text or 'IALDamageableInterface' in text or 'ReceiveALDamage_Implementation' in text:
            errors.append(f'legacy damage symbol remains in {path}')
        if 'Source/AshLineCombat/Public/Components/ALHealthComponent.h' not in str(path) and 'ASHLINECHARACTERS_API UALHealthComponent' in text:
            errors.append(f'Health Component still exported from a non-Combat module: {path}')

print('Prompt 03 validation')
print('====================')
print('Status:', 'PASS' if not errors else 'FAIL')
for error in errors: print('ERROR:', error)
sys.exit(1 if errors else 0)
