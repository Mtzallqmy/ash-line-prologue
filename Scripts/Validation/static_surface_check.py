#!/usr/bin/env python3
from pathlib import Path
import json
import sys

ROOT = Path(sys.argv[1] if len(sys.argv) > 1 else '.')
ERRORS = []
MODULES = ['AshLineCore', 'AshLineCharacters', 'AshLineCombat', 'AshLineWeapons', 'AshLineAI', 'AshLineDrones', 'AshLineMissions', 'AshLineContent', 'AshLineUI', 'AshLinePlatform']

for module in MODULES:
    if not (ROOT / f'Source/{module}/{module}.Build.cs').exists():
        ERRORS.append(f'missing Build.cs: {module}')
    if not (ROOT / f'Source/{module}/Private/{module}.cpp').exists():
        ERRORS.append(f'missing module cpp: {module}')

for path in ROOT.rglob('*'):
    if path.is_file() and path.name != 'static_surface_check.py' and path.suffix in {'.h', '.cpp', '.cs', '.ini', '.json', '.py', '.sh'}:
        text = path.read_text(encoding='utf-8', errors='ignore')
        if 'ASHLINE DRONES' in text:
            ERRORS.append(f'malformed export macro in {path}')
        if any(token in text for token in ['NewBlueprint', 'Final_Final', 'WeaponNew', 'Test2']):
            ERRORS.append(f'forbidden placeholder name in {path}')

manifest = json.loads((ROOT / 'Content/AshLine/Data/content_manifest.json').read_text())
chunks = json.loads((ROOT / 'Content/AshLine/Data/chunk_manifest.json').read_text())
if {p['id'] for p in manifest['packages']} != {c['package'] for c in chunks['chunks']}:
    ERRORS.append('package/chunk sets differ')

print(f'C++ implementation files: {len(list((ROOT / "Source").glob("**/*.cpp")))}')
print(f'Headers: {len(list((ROOT / "Source").glob("**/*.h")))}')
print(f'Modules: {len(list((ROOT / "Source").glob("*/*.Build.cs")))}')
print('Static checks:', 'PASS' if not ERRORS else 'FAIL')
for error in ERRORS:
    print('ERROR:', error)
sys.exit(1 if ERRORS else 0)
