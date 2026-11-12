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
import json,hashlib,struct,re
from pathlib import Path
from elftools.elf.elffile import ELFFile
p=Path('work/guild_level');meta=json.loads((p/'old-types-signatures.json').read_text());program=json.loads((p/'program-info.json').read_text())
assert meta['target_sha256']==program['sha256']=='61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c'
functions=[];graphs=[];instructions=[]
for f in sorted(p.glob('*-decompile.json')):
 d=json.loads(f.read_text());assert not d['truncated']
 assert d['function']['address']==f.name[:8],f
 (p/(f.stem+'.c')).write_text(d['code'])
 functions.append({'address':d['function']['address'],'name':d['function']['name'],'file':f.name,'complete':True})
for f in sorted(p.glob('*-calls.json')):
 d=json.loads(f.read_text());pages=d if isinstance(d,list) else [d]
 for page in pages:
  for kind in ('callers','callees'):
   if page.get(kind):assert not page[kind]['has_more'],f
  graphs.append({'function':page['function'],'callers':page.get('callers'),'callees':page.get('callees'),'file':f.name})
for f in sorted(p.glob('*-instructions.json')):
 d=json.loads(f.read_text());assert len(d['items'])==d['total'] and not d['has_more']
 instructions.append({'address':f.name[:8],'count':d['total'],'file':f.name})
variants=[]
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 elf=ELFFile(f)
 for sec in elf.iter_sections():
  if sec['sh_type'] not in ('SHT_SYMTAB','SHT_DYNSYM'):continue
  for s in sec.iter_symbols():
   if s['st_value'] in (0x8089f80,0x8089ff0,0x808a060,0x808a0b0) and s.name:
    variants.append({'address':hex(s['st_value']),'linkage':s.name,'size':s['st_size']})
packet=struct.pack('<IIIBB',0x89abcdef,0x12345678,0xfedcba98,255,0xe1)
dbframe=struct.pack('<BII',24,0,len(packet))+packet
member=struct.pack('<IBBBBIB',0x87654321,0xe1,0xa7,0xf2,255,0xfedcba98,0)
client=struct.pack('<BHB',75,17,2)+member
assert len(packet)==14 and len(dbframe)==23 and len(member)==13 and len(client)==17
constants={'DB_opcode':24,'DB_handle':0,'DB_payload_size':14,'client_header':75,'client_size':17,'client_subheader':2,'client_header_size':4,'member_prefix_size':13,'evidence':'0808f5d0-instructions.json'}
(p/'wire-constants-and-vectors.json').write_text(json.dumps({'target_sha256':meta['target_sha256'],'constants':constants,'sample':{'guild_id':'89abcdef','map_key_pid':'12345678','stored_member_pid':'87654321','offer':'fedcba98','grade':225,'is_general':167,'job':242,'level':255},'DB_payload14':packet.hex(),'DB_frame23':dbframe.hex(),'member_prefix13':member.hex(),'client_packet17':client.hex()},indent=2))
manifest={'schema_version':1,'target_sha256':meta['target_sha256'],'metadata':'old-types-signatures.json','constants':'wire-constants-and-vectors.json','functions':functions,'graphs':graphs,'instructions':instructions,'constructor_variants':variants,'status':'Complete bounded candidates and static evidence. No production edits, build, test or original execution. CGuild full lifecycle remains unrecovered, with its existing explicit test-only fixture kept separate.','artifacts':[{'path':f.name,'bytes':f.stat().st_size,'sha256':hashlib.sha256(f.read_bytes()).hexdigest()} for f in sorted(p.iterdir()) if f.is_file() and f.name!='manifest.json']}
(p/'manifest.json').write_text(json.dumps(manifest,indent=2))
print(json.dumps({'functions':len(functions),'graphs':len(graphs),'instructions':instructions,'variants':variants,'DB23':dbframe.hex(),'client17':client.hex()}))
