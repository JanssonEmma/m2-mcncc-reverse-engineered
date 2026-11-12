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
import json,hashlib,re
p=Path(__file__).resolve().parent
rows=json.loads((p/'pc-full-record-api-dwarf.json').read_text())
pcs=[r for r in rows if r['record']['name']=='PC']
assert len(pcs)==4
keys=lambda r:{(c['name'],c['linkage']) for c in r['children'] if c['tag']=='DW_TAG_subprogram' and not c['artificial']}
assert all(keys(r)==keys(pcs[0]) for r in pcs) and len(keys(pcs[0]))==61
old=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source\\game\\src\\questpc.h')).read_text()
new=(p/'questpc.h.candidate').read_text()
fields=lambda s:re.findall(r'\b(m_[A-Za-z0-9_]+)\s*;',s[s.index('std::vector<TQuestStateChangeInfo>'):])
assert fields(old)==fields(new)
data={'PC_method_declarations':61,'cross_CU_records':4,'private_field_names_order_preserved':fields(new),'source_only':True,'integration_ready':False,'builds':False,'F_writes':False}
(p/'api-data-audit.json').write_text(json.dumps(data,indent=2))
files=[f for f in p.iterdir() if f.is_file() and f.name!='manifest.json']
(p/'manifest.json').write_text(json.dumps({'status':'bounded_source_API_evidence_saved_NOT_APPLY_READY','files':{f.name:{'sha256':hashlib.sha256(f.read_bytes()).hexdigest(),'bytes':f.stat().st_size} for f in files}},indent=2))
print('Saved',len(files),'files;61 APIs across4 CUs; private fields preserved; no builds/Fwrites.')
