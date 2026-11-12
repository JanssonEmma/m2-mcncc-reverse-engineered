from pathlib import Path
import json,hashlib,ast
p=Path('work/stat_compute_scc')
for f in p.glob('*.py'):ast.parse(f.read_text())
for name in ['char.cpp.ComputeBattlePoints.candidate','char.cpp.ComputePoints.candidate']:
 s=(p/name).read_text();assert s.count('void CHARACTER::')==1
p.joinpath('MANIFEST.json').write_text(json.dumps({'scope':'Full complete candidates + readonly static dependency audit. No compile/runtime/apply/coverage pass claimed.','files':[{'path':str(f).replace('\\','/'),'sha256':hashlib.sha256(f.read_bytes()).hexdigest()} for f in sorted(p.rglob('*')) if f.is_file() and f.name!='MANIFEST.json']},indent=2)+'\n')
print('Workspace scripts AST and single-method extraction checks passed only; no C++ build')
