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

controller_h = read('Source/AshLineAI/Public/ALAIController.h')
controller_cpp = read('Source/AshLineAI/Private/ALAIController.cpp')
enemy_h = read('Source/AshLineAI/Public/ALInfantryCharacter.h')
enemy_cpp = read('Source/AshLineAI/Private/ALInfantryCharacter.cpp')
state_h = read('Source/AshLineAI/Public/AI/ALAIStateComponent.h')
sense_h = read('Source/AshLineAI/Public/AI/ALAISenseComponent.h')
sense_cpp = read('Source/AshLineAI/Private/AI/ALAISenseComponent.cpp')
combat_h = read('Source/AshLineAI/Public/AI/ALAICombatComponent.h')
combat_cpp = read('Source/AshLineAI/Private/AI/ALAICombatComponent.cpp')
cover_h = read('Source/AshLineAI/Public/AI/ALAICoverComponent.h')
cover_cpp = read('Source/AshLineAI/Private/AI/ALAICoverComponent.cpp')
route_h = read('Source/AshLineAI/Public/World/ALPatrolRoute.h')
coverpoint_h = read('Source/AshLineAI/Public/World/ALCoverPoint.h')
spawner_h = read('Source/AshLineAI/Public/World/ALEnemySpawner.h')
spawn_cpp = read('Source/AshLineAI/Private/World/ALEnemySpawner.cpp')
significance_h = read('Source/AshLineAI/Public/AI/ALAISignificanceComponent.h')
weapon_cpp = read('Source/AshLineWeapons/Private/ALWeaponBase.cpp')
weapon_component_cpp = read('Source/AshLineWeapons/Private/Components/ALWeaponComponent.cpp')
health_h = read('Source/AshLineCombat/Public/Components/ALHealthComponent.h')
core_noise_h = read('Source/AshLineCore/Public/ALNoiseSystemSubsystem.h')
core_noise_cpp = read('Source/AshLineCore/Private/ALNoiseSystemSubsystem.cpp')
ai_build = read('Source/AshLineAI/AshLineAI.Build.cs')

for token in ['Idle', 'Patrol', 'Suspicious', 'Alert', 'Combat', 'Search', 'Returning', 'Dead']:
    if token not in state_h: errors.append(f'AI state missing {token}')
for token in ['UAIPerceptionComponent', 'SightConfig', 'HearingConfig', 'OnPerceptionUpdated', 'DecisionInterval', 'OnMoveCompleted', 'MoveToLocation']:
    if token not in controller_h and token not in controller_cpp: errors.append(f'AI controller missing {token}')
for token in ['UALHealthComponent', 'UALWeaponComponent', 'UALAIStateComponent', 'UALAISenseComponent', 'UALAICombatComponent', 'UALAICoverComponent', 'OnEnemyKilled']:
    if token not in enemy_h or token not in enemy_cpp: errors.append(f'enemy character integration missing {token}')
for token in ['ProcessSightStimulus', 'ProcessHearingStimulus', 'ProcessDamageAwareness', 'LastKnownLocation', 'Awareness', 'DecayAwareness']:
    if token not in sense_h and token not in sense_cpp: errors.append(f'sense system missing {token}')
for token in ['StartCombat', 'StopCombat', 'ReactionTime', 'Accuracy', 'BeginBurst', 'EndBurst', 'BuildImperfectAim']:
    if token not in combat_h and token not in combat_cpp: errors.append(f'AI combat missing {token}')
for token in ['FindAndReserveCover', 'ReleaseCover', 'TryReserve', 'StandingCover', 'CrouchCover']:
    if token not in cover_h and token not in cover_cpp and token not in coverpoint_h: errors.append(f'cover foundation missing {token}')
for token in ['PatrolPoints', 'WaitTimes', 'bLoop']:
    if token not in route_h: errors.append(f'patrol route missing {token}')
for token in ['EnemyClass', 'Archetype', 'SpawnPoints', 'ActivateSpawner', 'DeactivateSpawner']:
    if token not in spawner_h and token not in spawn_cpp: errors.append(f'spawner foundation missing {token}')
for token in ['Near', 'Medium', 'Far', 'Inactive', 'UpdateSignificance']:
    if token not in significance_h: errors.append(f'AI LOD foundation missing {token}')
for token in ['OnDamageReceived', 'OnDeath']:
    if token not in health_h or token not in enemy_cpp: errors.append(f'health/death integration missing {token}')
for token in ['StartFire', 'SetAimDirection', 'FireAtTarget']:
    if token not in weapon_cpp and token not in weapon_component_cpp: errors.append(f'shared weapon integration missing {token}')
for token in ['ReportNoise', 'GetRecentEvents']:
    if token not in core_noise_h or token not in core_noise_cpp: errors.append(f'noise event system missing {token}')
if 'AshLineWeapons' not in ai_build: errors.append('AI module must depend on AshLineWeapons')
if 'PrimaryActorTick.bCanEverTick = true' in controller_cpp: errors.append('AIController must not use a permanent Tick')
if 'TickComponent' in ''.join([sense_cpp, combat_cpp, cover_cpp]): errors.append('AI components must not use TickComponent')

catalog = ROOT / 'Content/AshLine/Data/ai_archetype_catalog.json'
if catalog.exists():
    data = json.loads(catalog.read_text())
    ids = {item.get('id') for item in data.get('archetypes', [])}
    for expected in ['AI_Soldier_Basic', 'AI_Soldier_Trained']:
        if expected not in ids: errors.append(f'archetype catalog missing {expected}')
else: errors.append('missing ai archetype catalog')

for path in ROOT.rglob('*'):
    if path.is_file() and path.suffix in {'.h', '.cpp', '.cs', '.ini'}:
        text = path.read_text(encoding='utf-8', errors='ignore')
        if any(token in text for token in ['ABasicEnemy', 'ATrainedEnemy', 'AEliteEnemy', 'EnemyWeaponSystem', 'EnemyHealthSystem']): errors.append(f'duplicate enemy system/class found in {path}')
        if 'FALDamageEvent' in text: errors.append(f'legacy damage event remains in {path}')

print('Prompt 05 validation')
print('====================')
print('Status:', 'PASS' if not errors else 'FAIL')
for error in errors: print('ERROR:', error)
sys.exit(1 if errors else 0)
