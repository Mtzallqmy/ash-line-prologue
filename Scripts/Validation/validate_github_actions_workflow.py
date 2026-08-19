from pathlib import Path

try:
    import yaml
except ImportError as exc:
    raise SystemExit(f"PyYAML is required for local workflow validation: {exc}")

root = Path(__file__).resolve().parents[2]
workflow_path = root / ".github/workflows/build-android-apk.yml"
text = workflow_path.read_text(encoding="utf-8")
data = yaml.safe_load(text)

required_strings = [
    "workflow_dispatch",
    "Development",
    "Shipping",
    "self-hosted",
    "Windows",
    "X64",
    "unreal-5.4",
    "android",
    "UE_ROOT",
    "ANDROID_HOME",
    "ANDROID_NDK_HOME",
    "JAVA_HOME",
    "UnrealBuildTool.exe",
    "RunUAT.bat",
    "UnrealEditor-Cmd.exe",
    "ValidateBeforeBuild.ps1",
    "CreatePrototypeAssets.py",
    "L_CombatPrototype.umap",
    "BuildAndroidPrototype.ps1",
    "ash-line-android-apk",
    "ash-line-android-reports",
    "timeout-minutes",
]
missing = [token for token in required_strings if token not in text]
if missing:
    raise SystemExit("Missing workflow requirements: " + ", ".join(missing))

jobs = data.get("jobs", {})
if len(jobs) != 1:
    raise SystemExit("Expected exactly one build job")
job = next(iter(jobs.values()))
if job.get("timeout-minutes") != 180:
    raise SystemExit("Build job timeout must be 180 minutes")
labels = job.get("runs-on")
expected_labels = {"self-hosted", "Windows", "X64", "unreal-5.4", "android"}
if set(labels or []) != expected_labels:
    raise SystemExit(f"Runner labels mismatch: {labels}")
workflow_dispatch = data.get(True, {}).get("workflow_dispatch") or data.get("on", {}).get("workflow_dispatch")
if not workflow_dispatch:
    raise SystemExit("workflow_dispatch trigger is missing")
options = workflow_dispatch.get("inputs", {}).get("configuration", {}).get("options", [])
if set(options) != {"Development", "Shipping"}:
    raise SystemExit(f"Configuration options mismatch: {options}")

print("GitHub Actions workflow validation: PASS")
