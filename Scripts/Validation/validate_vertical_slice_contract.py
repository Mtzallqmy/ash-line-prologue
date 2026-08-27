from pathlib import Path
import sys


root = Path(sys.argv[1] if len(sys.argv) > 1 else ".")
required = {
    "Source/AshLineCharacters/Public/ALPlayerCharacter.h": ["MoveFromTouch", "LookFromTouch"],
    "Source/AshLineCharacters/Public/ALPlayerController.h": [
        "SubmitMobileMove", "SubmitMobileLook", "SetMobileFireHeld", "SetMobileAimHeld",
        "SetMobileSprintHeld", "TriggerMobileReload", "TriggerMobileSwitchWeapon",
        "TriggerMobileInteract", "TriggerMobileCrouch", "TriggerMobilePause",
    ],
    "Source/AshLineMissions/Public/ALMissionSubsystem.h": [
        "ActiveObjectiveIndex", "SetActiveObjectiveProgress", "CompleteActiveObjective",
        "OnObjectiveActivated", "OnObjectiveProgressChanged",
    ],
    "Source/AshLineMissions/Private/ALCombatPrototypeGameMode.cpp": [
        "StartMission", "ActivateObjective", "SetActiveObjectiveProgress", "CompleteActiveObjective",
    ],
    "Source/AshLineUI/Public/ALCombatPrototypeHUD.h": [
        "SetMissionDisplay", "SetObjectiveDisplay", "SetObjectiveProgressDisplay", "SetMissionCompleteDisplay",
    ],
    "Source/AshLineUI/Private/ALCombatPrototypeHUD.cpp": [
        "OnMissionStarted", "OnObjectiveActivated", "OnObjectiveProgressChanged", "HandleMissionCompleted",
    ],
    "Source/AshLineCore/Public/ALSaveGameSubsystem.h": [
        "LoadOrCreateSave", "RecordMissionProgress", "GetMissionProgress", "ASH_LINE_Profile",
    ],
    "Source/AshLineCore/Private/ALSaveGameSubsystem.cpp": [
        "DoesSaveGameExist", "SaveGameToSlot",
    ],
    "Source/AshLineMissions/Private/ALMissionSubsystem.cpp": [
        "PersistActiveMissionProgress", "RecordMissionProgress", "SaveCurrent",
    ],
    "Docs/Production/MobileTouchWidgetContract.md": ["WBP_MobileTouchLayer", "SubmitMobileMove"],
    "Docs/Production/ASH_LINE_3D_Production_Plan.md": ["Vertical Slice", "30 FPS"],
    "Docs/Build/WindowsUnrealAndroidBringup.md": ["Unreal Engine 5.4.4", "BuildFirstAPK.ps1", "30 FPS"],
}

errors = []
for relative_path, markers in required.items():
    path = root / relative_path
    if not path.is_file():
        errors.append(f"Missing required file: {relative_path}")
        continue
    text = path.read_text(encoding="utf-8")
    for marker in markers:
        if marker not in text:
            errors.append(f"Missing marker {marker!r} in {relative_path}")

print("ASH LINE vertical slice contract")
print("================================")
if errors:
    for error in errors:
        print("ERROR:", error)
    sys.exit(1)
print("PASS: mission progression, mobile input bridge, and production documentation are present")
