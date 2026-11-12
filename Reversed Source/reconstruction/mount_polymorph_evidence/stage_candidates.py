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
p=Path(__file__).parent
src=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'))
h=src.joinpath('game/src/char.h').read_text()
methods=[
 ('SetPolymorph','void SetPolymorph(DWORD race,bool maintain);'),
 ('MountVnum','void MountVnum(DWORD vnum);'),
 ('ComputePoints','void ComputePoints();'),
 ('ComputeSkillPoints','void ComputeSkillPoints();'),
 ('ComputeBattlePoints','void ComputeBattlePoints();'),
 ('PointChange','void PointChange(BYTE type,int amount,bool includeAmount,bool broadcast);'),
 ('RefreshAffect','void RefreshAffect();'),
 ('ComputeAffect','void ComputeAffect(CAffect* affect,bool add);'),
 ('StartMuyeongEvent','void StartMuyeongEvent();'),
 ('StopMuyeongEvent','void StopMuyeongEvent();'),
 ('HorseSummon','void HorseSummon(bool summon,bool fromFar);')]
adds=[decl for name,decl in methods if not re.search(r'\b'+name+r'\s*\(',h)]
newh=h.replace('    virtual ~CHARACTER();','    virtual ~CHARACTER();\n    '+'\n    '.join(adds))
p.joinpath('candidate-char.h').write_text(newh)
p.joinpath('char-api-additions.inc').write_text('\n'.join(adds)+'\n')
item=src.joinpath('game/src/item.h').read_text()
assert 'void ModifyPoints(' not in item
p.joinpath('candidate-item.h').write_text(item.replace('    void Save();','    void Save();\n    void ModifyPoints(bool add);'))
p.joinpath('item-api-additions.inc').write_text('void ModifyPoints(bool add);\n')
for name in ('horse_rider.h','affect_flag.h','affect.h','constants.h','guild.h'):
 p.joinpath('current-'+name).write_bytes(src.joinpath('game/src').joinpath(name).read_bytes())
# Separate a genuinely closed actual leaf for selective integration.
p.joinpath('compute-skill-points.cpp').write_text('#include "char.h"\n//0806cd70: actual empty original body; see complete4instruction evidence.\nvoid CHARACTER::ComputeSkillPoints() {}\n')
p.joinpath('mount-polymorph-abi.cpp').write_text(r"""
#include "../../game/src/char.h"
#include "../../game/src/constants.h"
typedef void(CHARACTER::*PolymorphSignature)(DWORD,bool);
typedef void(CHARACTER::*MountSignature)(DWORD);
typedef void(CHARACTER::*ComputeSignature)();
typedef bool(CHARACTER::*StopRidingSignature)();
PolymorphSignature exact_polymorph=&CHARACTER::SetPolymorph;
MountSignature exact_mount=&CHARACTER::MountVnum;
ComputeSignature exact_compute=&CHARACTER::ComputePoints;
ComputeSignature exact_skill_compute=&CHARACTER::ComputeSkillPoints;
StopRidingSignature exact_stop=&CHARACTER::StopRiding;
typedef char TargetInt32[sizeof(int)==4?1:-1];
typedef char TargetDWORD32[sizeof(DWORD)==4?1:-1];
typedef char JobRecord64[sizeof(TJobInitialPoints)==64?1:-1];
typedef char HorseRecord48[sizeof(THorseStat)==48?1:-1];
// No obsolete whole STL-containing class-size gate against a modern compiler.
""")
for f in p.glob('*-decompile.json'):
 d=json.loads(f.read_text());assert not d['truncated'];f.with_suffix('.c').write_text(d['code'])
tables=json.loads(p.joinpath('vtables.json').read_text())
v=next(t for t in tables if t['symbol']=='_ZTV9CHARACTER')
slot=v['words'][15]
assert slot['value']=='0x816d720'
resolution={'call_site':'08135c47','instruction':'CALL dword ptr [EAX+0x34]','vtable_symbol':v['symbol'],'vtable_address':v['address'],'address_point':'08226d08','slot_index':13,'entry':slot,'resolved_method':'virtual bool CHARACTER::StopRiding()','resolution':'raw ELF words + symbol identity + DWARF original virtual API'}
p.joinpath('setpolymorph-virtual-resolution.json').write_text(json.dumps(resolution,indent=2))
# Build immediate saved dependency graph/frontier; application edges remain real.
definitions={}
edges=[]
for f in p.glob('*-decompile.json'):
 d=json.loads(f.read_text());definitions[d['function']['address']]=d['function']['name']
for f in p.glob('*-calls.json'):
 d=json.loads(f.read_text())
 for e in d['callees']['items']:edges.append({'source':d['function']['address'],**e})
p.joinpath('dependency-graph.json').write_text(json.dumps({'saved_definitions':definitions,'edges':edges,'indirect_resolution':resolution},indent=2))
assert 'static_assert' not in p.joinpath('mount-polymorph-abi.cpp').read_text()
assert newh.replace('    '+'\n    '.join(adds)+'\n','')==h
print(json.dumps({'new_declarations':adds,'complete_decompiles':len(definitions),'callsites':len(edges),'virtual_slot13':slot['symbols']}))
