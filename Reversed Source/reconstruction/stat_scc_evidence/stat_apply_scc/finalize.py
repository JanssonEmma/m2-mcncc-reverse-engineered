from pathlib import Path
import json,hashlib
p=Path('work/stat_apply_scc');edges=[]
for addr in ['0812f7d0','08167420']:
 x=json.loads(Path('work/stat_dependency/'+addr+'-calls.json').read_text());x=x.get('structuredContent') or json.loads(x['content'][0]['text']);edges.append(x)
p.joinpath('original-direct-callgraphs.json').write_text(json.dumps(edges,indent=2)+'\n')
files=[f for f in p.iterdir() if f.is_file()];p.joinpath('MANIFEST.json').write_text(json.dumps({'scope':'Complete staged source, static audit only; no compile/runtime/coverage promotion','sources':[{'path':str(f).replace('\\','/'),'sha256':hashlib.sha256(f.read_bytes()).hexdigest()} for f in files]},indent=2)+'\n')
