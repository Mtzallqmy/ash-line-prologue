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

player_h = read('Source/AshLineCharacters/Public/ALPlayerCharacter.h')
player_cpp = read('Source/AshLineCharacters/Private/ALPlayerCharacter.cpp')
controller_h = read('Source/AshLineCharacters/Public/ALPlayerController.h')
controller_cpp = read('Source/AshLineCharacters/Private/ALPlayerController.cpp')
interaction_h = read('Source/AshLineCharacters/Public/ALInteractionComponent.h')
state_h = read('Source/AshLineCharacters/Public/ALPlayerStateComponent.h')
engine_ini = read('Config/DefaultEngine.ini')
manifest_path = ROOT / 'Content/AshLine/Input/input_asset_manifest.json'

for token in ['ACharacter', 'CameraRoot', 'FirstPersonCamera', 'UALInteractionComponent', 'UALPlayerStateComponent', 'EALMovementState']:
    if token not in player_h: errors.append(f'player character missing {token}')
for token in ['Move(const FInputActionValue&', 'Look(const FInputActionValue&', 'StartSprint', 'StopSprint', 'ToggleCrouch', 'StartJump', 'CanJumpInternal_Implementation']:
    if token not in player_h or token not in player_cpp: errors.append(f'player input/state API missing {token}')
for forbidden in ['BindAxis', 'BindAction', 'AALPlayerCharacter::Fire', 'AALPlayerCharacter::Reload']:
    if forbidden in player_cpp: errors.append(f'legacy or forbidden player hook found: {forbidden}')
for token in ['UInputMappingContext', 'MoveAction', 'LookAction', 'JumpAction', 'CrouchAction', 'SprintAction', 'InteractAction', 'PauseAction']:
    if token not in controller_h: errors.append(f'controller declaration missing {token}')
for token in ['AddMappingContext', 'BindAction', 'HandleMove', 'HandleLook', 'HandlePauseStarted']:
    if token not in controller_cpp: errors.append(f'controller implementation missing {token}')
for token in ['OnInteractionAvailabilityChanged', 'TryInteract', 'IALInteractable', 'TraceInterval']:
    if token not in interaction_h and token != 'IALInteractable': errors.append(f'interaction component missing {token}')
interactable_h = read('Source/AshLineCharacters/Public/ALInteractable.h')
for token in ['UALInteractable', 'IALInteractable', 'CanInteract', 'Interact', 'GetInteractionText']:
    if token not in interactable_h: errors.append(f'interactable interface missing {token}')
for token in ['SetMovementEnabled', 'SetLookEnabled', 'SetInteractionEnabled', 'SetInputLocked']:
    if token not in state_h: errors.append(f'player state API missing {token}')
if '/Script/AshLineCharacters.ALPrologueGameMode' not in engine_ini: errors.append('Prologue GameMode not wired')

if manifest_path.exists():
    manifest = json.loads(manifest_path.read_text())
    actions = {a['id']: a for a in manifest['actions']}
    for action in ['IA_Move','IA_Look','IA_Jump','IA_Crouch','IA_Sprint','IA_Interact','IA_Pause']:
        if action not in actions: errors.append(f'implemented input action missing: {action}')
    for action in ['IA_Fire','IA_Aim','IA_Reload','IA_Drone']:
        if actions.get(action, {}).get('status') != 'reserved': errors.append(f'future action is not reserved: {action}')

print('Prompt 02 validation')
print('====================')
print('Status:', 'PASS' if not errors else 'FAIL')
for error in errors: print('ERROR:', error)
sys.exit(1 if errors else 0)
