from pathlib import Path
import ast,json
p=Path('work/stat_affect_scc/closed_leaves')
for f in p.glob('*.py'):ast.parse(f.read_text())
x=json.loads((p/'integration/plan.json').read_text());assert len(x['files'])==8
assert '\t$(CXX)' in (p/'Makefile.addition').read_text()
print('Staging Python AST, 8file guards and recipe tabs checked; no C++ build/apply')
