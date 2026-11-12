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
import json,re,hashlib,shutil,subprocess
p=Path('work/stat_compute_scc');root=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'));files={str(f.relative_to(root)).replace('\\','/'):f for f in (root/'game/src').glob('*.cpp')};edges=json.loads((p/'original-callgraphs.json').read_text());matrix=[]
for graph in edges:
 rows=[]
 for name in sorted(set(x['name'] for x in graph['callees']['items'])):
  found=[]
  for rel,f in files.items():
   text=f.read_text(errors='replace')
   for m in re.finditer(re.escape(name)+r'\s*\(',text):
    after=text[m.end():];end=after.find(')');suffix=after[end+1:].lstrip() if end>=0 else ''
    if suffix.startswith('{'):found.append({'file':rel,'line':text.count('\n',0,m.start())+1,'sha256':hashlib.sha256(f.read_bytes()).hexdigest()})
  state='actual-current-source' if found else 'missing-current-game-source'
  if name in ('_sys_err','sys_log'):state='actual-libthecore-logging'
  if name=='<EXTERNAL>::__assert':state='external-C-assert-runtime'
  if name=='LC_IsYMIR':state='actual-locale-service-helper'
  rows.append({'name':name,'status':state,'definitions':found,'call_sites':[e['call_site'] for e in graph['callees']['items'] if e['name']==name]})
 matrix.append({'caller':graph['function'],'callees':rows})
p.joinpath('dependency-matrix.json').write_text(json.dumps(matrix,indent=2)+'\n')
text=Path('work/mount_polymorph/candidate-mount-polymorph.cpp').read_text()
exec(Path('work/stat_compute_scc/stage.py').read_text().split('origins={}')[0])
for name in ('RefreshAffect','ComputeAffect'):
 signature='void CHARACTER::'+name+'('
 p.joinpath('char_affect.cpp.'+name+'.candidate').write_text('// Full saved auxiliary body, STILL SCC-dependent; no app substitute.\n'+section(text,signature))
for src in ['work/battle_points/required-records-dwarf.json','work/battle_points/dwarf.json','work/battle_points/compute-assembly.txt','work/battle_points/direct-callgraph.json','work/mount_polymorph/full-records-api-virtuals.json','work/mount_polymorph/0812c520-assembly.json']:
 dest=p/'evidence'/Path(src).name;dest.parent.mkdir(exist_ok=True);shutil.copyfile(src,dest)
meta=json.loads(Path('work/stat_dependency/old-data-metadata.json').read_text());p.joinpath('original-compute-api.json').write_text(json.dumps([m for m in meta['methods'] if m['name'] in ('ComputeBattlePoints','ComputePoints')],indent=2)+'\n')
for addr in ['0812bda0','0812c520']:
 shutil.copyfile('work/stat_dependency/'+addr+'-decompile.c',p/'evidence'/str(addr+'-decompile.c'))
 r=subprocess.run([r'C:\Strawberry\c\bin\objdump.exe','-d','--start-address='+('0x'+addr),'--stop-address='+hex(0x812c520 if addr=='0812bda0' else 0x812cc10),_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game')],capture_output=True,text=True,check=True);p.joinpath('evidence/'+addr+'-assembly.txt').write_text(r.stdout)
print([(m['caller']['name'],[(e['name'],e['status']) for e in m['callees'] if 'missing' in e['status']]) for m in matrix])
