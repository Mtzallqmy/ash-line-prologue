#!/usr/bin/env python3
from __future__ import annotations

import hashlib
import json
import re
import shutil
import subprocess
import sys
import zipfile
from pathlib import Path

apk = Path(sys.argv[1]) if len(sys.argv) > 1 else None
report_path = Path(sys.argv[2]) if len(sys.argv) > 2 else (apk.with_suffix('.json') if apk else Path('android_release_report.json'))
if not apk or not apk.is_file():
    print('APK verification blocked: APK file does not exist', file=sys.stderr)
    raise SystemExit(2)

errors: list[str] = []
warnings: list[str] = []
with zipfile.ZipFile(apk) as archive:
    names = archive.namelist()
    abi_dirs = sorted({name.split('/')[1] for name in names if name.startswith('lib/') and name.count('/') >= 2})
    if 'arm64-v8a' not in abi_dirs:
        errors.append('arm64-v8a native libraries are missing')
    for forbidden in {'armeabi-v7a', 'x86', 'x86_64'}:
        if forbidden in abi_dirs:
            errors.append(f'forbidden ABI present: {forbidden}')
    native_entries = [name for name in names if name.startswith('lib/') and name.endswith('.so')]
    if 'AndroidManifest.xml' not in names:
        errors.append('AndroidManifest.xml is missing')

apksigner = shutil.which('apksigner')
aapt = shutil.which('aapt') or shutil.which('aapt2')
signature = 'NOT_CHECKED'
manifest_text = ''
if apksigner:
    result = subprocess.run([apksigner, 'verify', '--verbose', str(apk)], capture_output=True, text=True)
    signature = 'PASS' if result.returncode == 0 else f'FAIL: {result.stderr.strip()[:500]}'
    if result.returncode != 0:
        errors.append('apksigner verification failed')
else:
    warnings.append('apksigner unavailable; APK signature was not verified')

metadata = {
    'package': None,
    'versionCode': None,
    'versionName': None,
    'minSdk': None,
    'targetSdk': None,
}
if aapt:
    result = subprocess.run([aapt, 'dump', 'badging', str(apk)], capture_output=True, text=True)
    manifest_text = result.stdout
    if result.returncode != 0:
        warnings.append('aapt could not dump Android manifest')
    else:
        package_match = re.search(r"package: name='([^']+)' versionCode='([^']+)' versionName='([^']+)'", manifest_text)
        sdk_match = re.search(r"sdkVersion:'([^']+)'", manifest_text)
        target_match = re.search(r"targetSdkVersion:'([^']+)'", manifest_text)
        if package_match:
            metadata['package'], metadata['versionCode'], metadata['versionName'] = package_match.groups()
        if sdk_match:
            metadata['minSdk'] = sdk_match.group(1)
        if target_match:
            metadata['targetSdk'] = target_match.group(1)
else:
    warnings.append('aapt/aapt2 unavailable; package/minSdk/targetSdk were not decoded')

if metadata['package'] is not None and metadata['package'] != 'com.ashline.game':
    errors.append(f"unexpected package name: {metadata['package']}")
if metadata['versionCode'] is not None and metadata['versionCode'] != '1':
    errors.append(f"unexpected versionCode: {metadata['versionCode']}")
if metadata['versionName'] is not None and metadata['versionName'] != '0.0.1':
    errors.append(f"unexpected versionName: {metadata['versionName']}")
if metadata['minSdk'] is not None and metadata['minSdk'] != '26':
    errors.append(f"unexpected minSdk: {metadata['minSdk']}")
if metadata['targetSdk'] is not None and int(metadata['targetSdk']) < 34:
    errors.append(f"targetSdk is below 34: {metadata['targetSdk']}")

hasher = hashlib.sha256()
with apk.open('rb') as handle:
    for chunk in iter(lambda: handle.read(1024 * 1024), b''):
        hasher.update(chunk)
sha = hasher.hexdigest()
size = apk.stat().st_size
if size > 500 * 1024 * 1024:
    errors.append(f'APK exceeds 500 MiB hard limit: {size / (1024 * 1024):.2f} MiB')

report = {
    'apk': str(apk),
    'filename': apk.name,
    'apkBytes': size,
    'apkMiB': round(size / (1024 * 1024), 3),
    'sha256': sha,
    'abiDirs': abi_dirs,
    'nativeEntries': native_entries,
    'signature': signature,
    'manifestDecoded': bool(manifest_text),
    'metadata': metadata,
    'expected': {
        'package': 'com.ashline.game',
        'versionName': '0.0.1',
        'versionCode': 1,
        'minSdk': 26,
        'targetSdkMin': 34,
        'abi': 'arm64-v8a',
    },
    'errors': errors,
    'warnings': warnings,
}
report_path.parent.mkdir(parents=True, exist_ok=True)
report_path.write_text(json.dumps(report, indent=2, ensure_ascii=False) + '\n', encoding='utf-8')
print(json.dumps({k: report[k] for k in ['filename', 'apkBytes', 'sha256', 'abiDirs', 'signature', 'metadata', 'errors', 'warnings']}, indent=2, ensure_ascii=False))
raise SystemExit(1 if errors else 0)
