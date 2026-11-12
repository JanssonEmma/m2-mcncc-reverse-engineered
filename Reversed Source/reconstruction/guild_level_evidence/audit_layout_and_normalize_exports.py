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
import json,hashlib,re
from pathlib import Path
p=Path('work/guild_level');meta=json.loads((p/'old-types-signatures.json').read_text());program=json.loads((p/'program-info.json').read_text())
root=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'))
old=next(r for r in meta['records'] if r['unit']=='guild.cpp' and r['name']=='CGuild')
header=(root/'game/src/guild.h').read_text();body=header.split('private:',1)[1].split('};',1)[0]
names=[]
for line in body.splitlines():
 line=line.strip()
 if not line or line.startswith('//'):continue
 match=re.search(r'\b(\w+)(?:\[\d+\])?;',line)
 assert match,line
 names.append(match.group(1))
assert names==[m['name'] for m in old['members']]
layouts={}
for profile,pointer,container,align in [('old_2009',4,12,4),('modern_GCC13_strict32',4,24,4),('modern_GCC13_Windows_native64',8,48,8)]:
 rows=[];end=0
 for field in old['members']:
  name=field['name'];iscontainer=name in ('m_member','m_memberOnline','m_memberP2POnline','m_EnemyGuild','m_mapGuildWarEndTime','m_GuildInviteEventMap')
  size=container if iscontainer else (220 if name=='m_data' else 12 if name=='abSkillUsable' else 4)
  alignment=align if iscontainer else (1 if name=='abSkillUsable' else 4)
  off=(end+alignment-1)//alignment*alignment
  if profile=='old_2009':assert off==field['location'][0]['args'][0]
  rows.append({'name':name,'offset':off,'size':size,'alignment':alignment,'old_container_size':12 if iscontainer else None})
  end=off+size
 total=(end+align-1)//align*align;layouts[profile]={'size':total,'members':rows}
assert layouts['old_2009']['size']==312 and layouts['modern_GCC13_strict32']['size']==384 and layouts['modern_GCC13_Windows_native64']['size']==536
headers=[]
for name in ('stl_tree.h','stl_map.h','stl_set.h'):
 f=Path(r'C:\Strawberry\c\include\c++\13.2.0\bits')/name
 headers.append({'path':str(f),'sha256':hashlib.sha256(f.read_bytes()).hexdigest()})
audit={'target_sha256':program['sha256'],'complete_logical_member_count':len(names),'source_logical_members_match_original_names_order':names,'layouts':layouts,'layout_provenance':'Old312 offsets/sizes verified from DWARF. Modern384/536 are exact header-derived calculations for normal GCC13.2 map/set (not debug mode, no short-enums/packing), not a new compiler measurement. No builds performed.','STL_headers':headers,'explanation':'Six old containers12 each become24 in modern32, adding72. New _Rb_tree_header embeds node-base16+count4; key-compare member plus alignment adds4, yielding24. Native64 node-base32+count8 plus compare alignment8 yields48, plus guild alignment gaps4+4 gives536. Old-string ABI selector affects string, not tree header layout.','ABI_strategy':'Keep concrete wire/prefix record assertions; validate all10 CGuild data member types and declaration order without a fixed whole-class size or invented padding.'}
(p/'logical-layout-audit.json').write_text(json.dumps(audit,indent=2))
exports=[]
for f in sorted(p.glob('*-decompile.json')):
 d=json.loads(f.read_text());assert not d['truncated'];a=d['function']['address'];assert a==f.name[:8]
 signature_file=p/(a+'-signature.json')
 signature=json.loads(signature_file.read_text()).get('signature') if signature_file.exists() else None
 exports.append({'requested_address':'0x'+a,'function':d['function'],'signature':signature,'code':d['code'],'truncated':d['truncated'],'error':'','provenance':{'raw_export':f.name,'raw_export_sha256':hashlib.sha256(f.read_bytes()).hexdigest(),'raw_schema':'Original saved Ghidra structured result: function/code/truncated; retained without rewriting','program_info':'program-info.json','target_sha256':program['sha256']}})
out={'target_sha256':program['sha256'],'program':'/game','program_function_count':program['function_count'],'exported_function_count':len(exports),'origin':'Read-only live Ghidra tool exports, normalized address-by-address from original structured results; raw files and program identity remain in this package. No reanalysis, renaming or target execution.','functions':exports}
(p/'guild-level-addressed-ghidra.json').write_text(json.dumps(out,indent=2))
print(json.dumps({'logical_members':len(names),'old_size':312,'modern32_header_derived':384,'modernWin64_header_derived':536,'addressed_exports':len(exports)}))
