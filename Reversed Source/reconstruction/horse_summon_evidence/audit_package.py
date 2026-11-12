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
p=Path(__file__).parent;root=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'))
exports=[];edges=[];assembly=[]
for f in p.glob('*-decompile.json'):
 d=json.loads(f.read_text());assert not d.get('truncated')
 exports.append({'file':f.name,'function':d['function'],'ghidra_warning':bool('WARNING:' in d.get('code',''))})
 f.with_suffix('.c').write_text(d['code'])
for f in p.glob('*-calls.json'):
 d=json.loads(f.read_text());assert not d['callees'].get('has_more')
 edges.extend({'source':d['function']['address'],**x} for x in d['callees']['items'])
for f in p.glob('*-assembly.json'):
 d=json.loads(f.read_text());assert not d['has_more'];assert len(d['items'])==d['total']
 assembly.append({'file':f.name,'instructions':d['total']})
sources=list(root.joinpath('game/src').glob('*.cpp'))+list(root.joinpath('libthecore/src').glob('*.c'))
def present(name):
 pat=re.compile(r'\b'+re.escape(name)+r'\s*\([^;{}]*\)\s*(?:const\s*)?\{')
 result=[]
 for f in sources:
  s=f.read_text(errors='replace')
  for m in pat.finditer(s):result.append({'file':str(f),'line':s.count('\n',0,m.start())+1,'sha256':hashlib.sha256(f.read_bytes()).hexdigest()})
 return result
backend={n:present(n) for n in ('CHARACTER::ChatPacket','CHorseRider::GetHorseMaxHealth','CHorseRider::GetHorseMaxStamina','CHARACTER_MANAGER::Find','CEntity::PacketView','get_dword_time')}
assert all(backend.values()),backend
# Exact source placement and virtual resolution assertions.
placement=json.loads(p.joinpath('source-placement.json').read_text())
assert next(x for x in placement if x['name']=='GetVictim')['cu']=='char_battle.cpp'
calls=json.loads(p.joinpath('virtual-call-resolution.json').read_text())['calls']
assert [x['entry']['value'] for x in calls]==['0x816df10','0x816e000','0x816e060']
fixture=p.joinpath('horse_methods_character_fixture.cpp').read_text()
original=root.joinpath('reconstruction/tests/character_fixture.cpp').read_text()
expected=original.replace('DWORD CHARACTER::GetMyHorseVnum() { std::abort(); }','// Actual GetMyHorseVnum is linked by this leaf suite.').replace('void CHARACTER::SendHorseInfo() { std::abort(); }','// Actual SendHorseInfo is linked by this leaf suite.').replace('void CHARACTER::ClearHorseInfo() { std::abort(); }','// Actual ClearHorseInfo is linked by this leaf suite.')
assert fixture==expected
assert 'bool CHARACTER::StopRiding() { std::abort(); }' in fixture
for f in p.glob('*.cpp'):
 s=f.read_text();assert 'static_assert(' not in s and '<type_traits>' not in s
assert len(json.loads(p.joinpath('0816d940-assembly.json').read_text())['items'])==366
# Full current records remain copied rather than reduced.
char=root.joinpath('game/src/char.h').read_text()
adds=p.joinpath('char-api-additions.inc').read_text().rstrip('\n').splitlines()
expected=char.replace('    virtual ~CHARACTER();','    virtual ~CHARACTER();\n    '+'\n    '.join(adds))
assert p.joinpath('candidate-char.h').read_text()==expected
guild=root.joinpath('game/src/guild.h').read_text()
assert p.joinpath('candidate-guild.h').read_text()==guild.replace('    DWORD GetID()','    DWORD GetMasterPID() { return m_data.master_pid; }\n    DWORD GetID()')
p.joinpath('closure-audit.json').write_text(json.dumps({'current_production_backends':backend,'dedicated_fixture_exactly_removes_three_abort_methods':True,'whole_HorseSummon_runtime_closed':False,'closed_leaf_package':'char.cpp/char_battle.cpp/char_horse.cpp fragments +metadata-backed inline getter +dead5/move24 +Trunc32 helper','remaining':'SetProto and branches, Show virtual insertion/StartAffectEvent, MonsterChat/script data, full lifecycle/spatial loaders; no fake body'},indent=2))
out={'target_sha256':'61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c','frozen':True,'production_or_global_fixture_changed':False,'builds_or_tests_or_original_execution':False,'complete_decompiles':exports,'callsites':len(edges),'assembly_exports':assembly,'source_only_checks_passed':True,'files':[]}
for f in p.iterdir():
 if f.is_file() and f.name!='manifest.json':out['files'].append({'file':f.name,'bytes':f.stat().st_size,'sha256':hashlib.sha256(f.read_bytes()).hexdigest()})
p.joinpath('manifest.json').write_text(json.dumps(out,indent=2))
print(json.dumps({'decompiles':len(exports),'callsites':len(edges),'assembly':assembly,'source_checks':True,'test_status':'STAGED UNRUN','files':len(out['files'])}))
