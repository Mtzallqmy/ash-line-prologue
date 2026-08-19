#!/usr/bin/env python3
from pathlib import Path
import json, re, sys, zipfile

ROOT = Path(sys.argv[1] if len(sys.argv) > 1 else '.')
errors = []

def require(rel):
    path = ROOT / rel
    if not path.exists(): errors.append(f'missing: {rel}')
    return path

manifest_path = require('Content/AshLine/Data/content_manifest.json')
for rel in [
    'Source/AshLineContent/Public/ALContentManifest.h',
    'Source/AshLineContent/Public/ALContentDeliveryService.h',
    'Source/AshLineContent/Public/ALPackageRegistry.h',
    'Source/AshLineContent/Public/ALPackageValidator.h',
    'Source/AshLineContent/Public/ALPackageMountManager.h',
    'Source/AshLineContent/Public/ALContentManagerSubsystem.h',
    'Source/AshLineContent/Public/ALRemoteContentServices.h',
    'Source/AshLineContent/Public/ALContentPlatformBridge.h',
    'Scripts/Content/BuildALPack.py',
]: require(rel)

if manifest_path.exists():
    data = json.loads(manifest_path.read_text(encoding='utf-8'))
    if data.get('schemaVersion') != 1: errors.append('content manifest schemaVersion must be 1')
    packages = data.get('packages', [])
    ids = {p.get('id') for p in packages}
    if len(ids) != len(packages): errors.append('duplicate package IDs')
    graph = {p.get('id'): p.get('dependencies', []) for p in packages}
    for package_id, dependencies in graph.items():
        for dep in dependencies:
            if dep not in graph: errors.append(f'missing dependency {dep} for {package_id}')
    def visit(node, visiting, visited):
        if node in visiting: return False
        if node in visited: return True
        visiting.add(node)
        for dep in graph.get(node, []):
            if not visit(dep, visiting, visited): return False
        visiting.remove(node); visited.add(node); return True
    visited = set()
    for node in graph:
        if not visit(node, set(), visited): errors.append('circular dependency graph')
    for package in packages:
        if not package.get('id') or '..' in package.get('id', ''): errors.append('unsafe package ID')
        if '..' in package.get('contentRoot', ''): errors.append(f'unsafe content root for {package.get("id")}')
        if package.get('required') and package.get('id') == 'city_namar_prologue': errors.append('optional city must not be required by core manifest')

for path in (ROOT / 'Source/AshLineContent').rglob('*'):
    if path.is_file() and path.suffix in {'.h', '.cpp'}:
        text = path.read_text(encoding='utf-8', errors='ignore')
        if 'private key' in text.lower() or 'github_pat_' in text or 'OPENAI_API_KEY' in text: errors.append(f'secret marker in {path}')
        if 'MountPackage' in text and 'VerifyPackage' not in text and 'ALPackageMountManager' not in path.name: errors.append(f'possible mount-before-verify surface in {path}')

builder = ROOT / 'Scripts/Content/BuildALPack.py'
if builder.exists() and 'FORBIDDEN_SUFFIXES' not in builder.read_text(encoding='utf-8'): errors.append('BuildALPack missing content-only executable guard')

print('ASH LINE content-system validation')
print('==================================')
print('Status:', 'PASS' if not errors else 'FAIL')
for error in errors: print('ERROR:', error)
sys.exit(1 if errors else 0)
