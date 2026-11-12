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
import json,struct,hashlib
from pathlib import Path
from elftools.elf.elffile import ELFFile
p=Path('work/stat_primitives');d=json.loads((p/'old-types-signatures.json').read_text())
def offset(row):return row['location'][0]['args'][0]
cp=next(r for r in d['records'] if r['unit']=='char.cpp' and r['name']=='character_point')
ip=next(r for r in d['records'] if r['unit']=='char.cpp' and r['name']=='character_point_instant')
base={r['name']:offset(r) for r in d['character_members'] if r['unit']=='char.cpp'}
cpfields={r['name']:offset(r) for r in cp['members']}
ipfields={r['name']:offset(r) for r in ip['members']}
proof={'m_points.job':base['m_points']+cpfields['job'],'m_pointsInstant.position':base['m_pointsInstant']+ipfields['position'],'m_pointsInstant.instant_flag':base['m_pointsInstant']+ipfields['instant_flag']}
assert proof=={'m_points.job':0x500,'m_pointsInstant.position':0x92c,'m_pointsInstant.instant_flag':0x930}
raw={}
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 elf=ELFFile(f)
 def read(addr,n):
  for s in elf.iter_sections():
   if s['sh_addr']<=addr and addr+n<=s['sh_addr']+s['sh_size'] and s['sh_type']!='SHT_NOBITS':
    return s.data()[addr-s['sh_addr']:addr-s['sh_addr']+n]
  raise ValueError(hex(addr))
 raw['race_switch_table']={'address':'0821da30','hex':read(0x821da30,32).hex(),'targets':[hex(v) for v in struct.unpack('<8I',read(0x821da30,32))]}
 raw['GetJob_format']={'address':'08225000','text':read(0x8225000,90).split(b'\0')[0].decode()}
 raw['GetJob_function_label']={'address':'08224494','text':read(0x8224494,20).split(b'\0')[0].decode()}
assert raw['GetJob_format']['text']=='CHARACTER::GetJob(name=%s, race=%d).OUT_OF_RACE_RANGE'
assert raw['GetJob_function_label']['text']=='GetJob'
exports=[];calls=[]
for f in sorted(p.glob('*-decompile.json')):
 x=json.loads(f.read_text());assert x.get('truncated') is False
 exports.append({'address':x['function']['address'],'name':x['function']['name'],'file':f.name,'sha256':hashlib.sha256(f.read_bytes()).hexdigest()})
for f in sorted(p.glob('*-calls.json')):
 pages=json.loads(f.read_text())
 for q in pages:
  assert not q['callers']['has_more'] and not q['callees']['has_more']
  calls.append({'function':q['function'],'callers':q['callers']['items'],'callees':q['callees']['items'],'evidence':f.name})
out={'target_sha256':d['target_sha256'],'absolute_target_member_offsets':proof,'original_bytes_and_strings':raw,'functions':exports,'callers_callees':calls,'expected_static_layout_vectors':{'skill_bMasterType3_bLevel40_tNextRead_minus2':struct.pack('<BBi',3,40,-2).hex(),'packet_header17_vid89abcdef_type13_amount_minus01020304_value12345678':struct.pack('<iIBii',17,0x89abcdef,13,-0x01020304,0x12345678).hex()},'qualification':'Four complete bounded candidate helpers; no production changes, no builds/tests or target execution. NewPlayerTable2 and GetName exports are caller/dependency evidence, not promoted implementations.'}
(p/'evidence-summary.json').write_text(json.dumps(out,indent=2))
print(json.dumps({'exports':len(exports),'offsets':proof,'raw_strings':raw,'vectors':out['expected_static_layout_vectors']}))
