#!/usr/bin/env python3
from pathlib import Path
import json, sys

root = Path(sys.argv[1] if len(sys.argv) > 1 else '.')
errors, warnings = [], []
project = root / 'ASH_LINE.uproject'
if not project.exists(): errors.append('ASH_LINE.uproject is missing')
else:
    data = json.loads(project.read_text())
    names = {m['Name'] for m in data.get('Modules', [])}
    required = {'AshLineCore','AshLineCharacters','AshLineCombat','AshLineWeapons','AshLineAI','AshLineDrones','AshLineMissions','AshLineContent','AshLineUI','AshLinePlatform'}
    missing = required - names
    if missing: errors.append('Missing modules: ' + ', '.join(sorted(missing)))
for rel in ['Content/AshLine/Data/content_manifest.json','Content/AshLine/Data/chunk_manifest.json','Content/AshLine/Data/weapon_catalog.json','Config/DefaultEngine.ini','Config/DefaultGame.ini']:
    if not (root / rel).exists(): errors.append('Missing required file: ' + rel)
manifest = json.loads((root/'Content/AshLine/Data/content_manifest.json').read_text())
chunks = json.loads((root/'Content/AshLine/Data/chunk_manifest.json').read_text())
packages = {p['id'] for p in manifest['packages']}
chunk_packages = {c['package'] for c in chunks['chunks']}
if not packages <= chunk_packages: errors.append('Manifest package without chunk: ' + ', '.join(sorted(packages-chunk_packages)))
if any(p['required'] and p['id'] != 'core' and 'core' not in p['dependencies'] for p in manifest['packages']): warnings.append('A required package is not explicitly dependent on core')
weapon_ids = [w['id'] for w in json.loads((root/'Content/AshLine/Data/weapon_catalog.json').read_text())['weapons']]
if len(weapon_ids) != len(set(weapon_ids)): errors.append('Duplicate weapon IDs found')
print('ASH LINE validation')
print('==================')
print('Errors:', len(errors))
for e in errors: print('ERROR:', e)
print('Warnings:', len(warnings))
for w in warnings: print('WARNING:', w)
if not errors: print('PASS: project structure, manifests, chunks, and weapon catalog are coherent')
sys.exit(1 if errors else 0)
