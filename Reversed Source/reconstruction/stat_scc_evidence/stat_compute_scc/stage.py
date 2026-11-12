# Resolve portable paths from the repository root, regardless of current working directory.
import sys as _portable_sys
from pathlib import Path as _portable_Path
for _portable_parent in (_portable_Path(__file__).resolve().parent, *_portable_Path(__file__).resolve().parents):
    if (_portable_parent / 'project_paths.py').is_file():
        _portable_sys.path.insert(0, str(_portable_parent))
        break
else:
    raise RuntimeError('project_paths.py not found in repository ancestors')
from project_paths import resolve_path as _resolve_portable_path
from pathlib import Path
import json,re,hashlib
p=Path('work/stat_compute_scc');root=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'))
def readtool(path):
 x=json.loads(Path(path).read_text(encoding='utf-8-sig'))
 if isinstance(x,list): return x
 return x.get('structuredContent') or (json.loads(x['content'][0]['text']) if 'content' in x else x)
def section(text,signature):
 at=text.index(signature);start=text.index('{',at);level=0
 for i in range(start,len(text)):
  if text[i]=='{':level+=1
  elif text[i]=='}':
   level-=1
   if not level:return text[at:i+1]+'\n'
origins={}
f=Path('work/battle_points/compute-method.cpp.candidate');p.joinpath('char.cpp.ComputeBattlePoints.candidate').write_bytes(f.read_bytes());origins['ComputeBattlePoints']={'source':str(f),'sha256':hashlib.sha256(f.read_bytes()).hexdigest(),'address':'0812bda0'}
f=Path('work/mount_polymorph/candidate-mount-polymorph.cpp');text=f.read_text();p.joinpath('char.cpp.ComputePoints.candidate').write_text('// Full original0812c520 body, extracted without narrowing from saved mount_polymorph candidate.\n'+section(text,'void CHARACTER::ComputePoints()'));origins['ComputePoints']={'source':str(f),'sha256':hashlib.sha256(f.read_bytes()).hexdigest(),'address':'0812c520'}
helpers=Path('work/battle_points/char-helper-methods.cpp.candidate').read_text();p.joinpath('battle-arithmetic.inc.candidate').write_text(helpers[helpers.index('namespace battle_points_detail'):helpers.index('//0812bb80')]);p.joinpath('compute-arithmetic.inc.candidate').write_text('#include <cstring>\n'+text[text.index('namespace mount_polymorph_detail'):text.index('//08135b10')])
p.joinpath('char.h.additions.candidate').write_text('// Original nonconst public API, char.cpp DWARF: exact void() member methods.\nvoid ComputeBattlePoints();\nvoid ComputePoints();\n')
edges=[]
for addr in ('0812bda0','0812c520'):
 path='work/stat_dependency/'+addr+'-calls.json';x=readtool(path);edges.append(x)
p.joinpath('original-callgraphs.json').write_text(json.dumps(edges,indent=2)+'\n');print([(x['function']['name'],sorted(set(y['name'] for y in x['callees']['items']))) for x in edges])
p.joinpath('candidate-origins.json').write_text(json.dumps(origins,indent=2)+'\n')
