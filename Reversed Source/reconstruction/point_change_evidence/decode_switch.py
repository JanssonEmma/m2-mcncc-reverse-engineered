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
import json,pathlib,struct,re,ast
from elftools.elf.elffile import ELFFile
p=pathlib.Path('work/point_change');out=[]
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 e=ELFFile(f)
 for seg in e.iter_segments():
  a=seg['p_vaddr']
  if a<=0x8224524 and 0x8224524+127*4<=a+seg['p_filesz']:
   vals=struct.unpack('<127I',seg.data()[0x8224524-a:0x8224524-a+508]);out=[{'type':i,'target':hex(v)} for i,v in enumerate(vals)];break
p.joinpath('switch-targets.json').write_text(json.dumps(out,indent=2));groups={}
for x in out:groups.setdefault(x['target'],[]).append(x['type'])
p.joinpath('switch-groups.json').write_text(json.dumps(groups,indent=2));print(groups)
calls=[]
for l in p.joinpath('assembly.txt').read_text().splitlines():
 m=re.search(r'^\s*([0-9a-f]+):.*\bcall\s+([0-9a-f]+)\s+<([^>]+)>',l)
 if m:calls.append({'site':'0x'+m[1],'address':'0x'+m[2],'symbol':m[3]})
p.joinpath('direct-callgraph.json').write_text(json.dumps(calls,indent=2))
