#!/usr/bin/env python3
from pathlib import Path
import json, sys
root = Path(sys.argv[1] if len(sys.argv) > 1 else '.')
by_root = {}
for p in root.rglob('*'):
    if p.is_file() and not any(x in p.parts for x in ['.git','Binaries','Intermediate','Saved','DerivedDataCache']):
        rel = p.relative_to(root)
        top = rel.parts[0] if rel.parts else '.'
        by_root[top] = by_root.get(top, 0) + p.stat().st_size
rows = sorted(by_root.items(), key=lambda x: x[1], reverse=True)
print('ASH LINE source/content size report')
print('===================================')
for name, size in rows[:20]: print(f'{size/1024/1024:8.3f} MB  {name}')
print(f'TOTAL (source snapshot): {sum(by_root.values())/1024/1024:.3f} MB')
print('NOTE: final Shipping size requires Unreal Cook/Package on a configured build machine.')
