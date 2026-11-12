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
import json,hashlib,shutil
p=Path(__file__).parent;root=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'));src=root/'game/src';tests=root/'reconstruction/tests'
backup=p/'integration-backup';backup.mkdir(exist_ok=True)
files=['game/src/char.cpp','game/src/char_battle.cpp','game/src/char_horse.cpp','game/src/char.h','game/src/guild.h','game/src/packet.h','game/src/horse-summon-detail.h','reconstruction/tests/Makefile','reconstruction/tests/horse_methods_regression.cpp','reconstruction/tests/horse_methods_abi.cpp','reconstruction/tests/horse_methods_character_fixture.cpp']
before={}
for rel in files:
 f=root/rel;before[rel]=hashlib.sha256(f.read_bytes()).hexdigest() if f.exists() else None
 if f.exists():
  dest=backup/rel;dest.parent.mkdir(parents=True,exist_ok=True);shutil.copyfile(f,dest)
protected=['character_fixture.cpp','desc_fixture.cpp','entity_packet_fixture.cpp']
protected_hash={f:hashlib.sha256((tests/f).read_bytes()).hexdigest() for f in protected}
for target,fragment in [('char.cpp','char.cpp-closed-additions.inc'),('char_battle.cpp','char_battle.cpp-closed-additions.inc'),('char_horse.cpp','char_horse.cpp-closed-additions.inc')]:
 f=src/target;old=f.read_text() if f.exists() else ''
 assert 'CHARACTER::SetRider(' not in old if target=='char_horse.cpp' else True
 f.write_text(old+'\n'+(p/fragment).read_text())
char=src/'char.h';s=char.read_text()
adds=['void SetRotation(float rotation);','void SetEmpire(BYTE empire);','LPCHARACTER GetVictim();','void SetRider(LPCHARACTER rider);','void SendMovePacket(BYTE function,BYTE argument,DWORD x,DWORD y,DWORD duration,DWORD time,int rotation);']
for decl in adds:assert decl.split('(')[0].split()[-1]+'(' not in s
s=s.replace('    virtual ~CHARACTER();','    virtual ~CHARACTER();\n    '+'\n    '.join(adds));char.write_text(s)
guild=src/'guild.h';s=guild.read_text();assert 'GetMasterPID(' not in s
guild.write_text(s.replace('    DWORD GetID()','    DWORD GetMasterPID() { return m_data.master_pid; }\n    DWORD GetID()'))
packet=src/'packet.h';s=packet.read_text();assert 'packet_move' not in s and 'packet_dead' not in s
packet.write_text(s.replace('#pragma pack(pop)',(p/'packet-record-additions.inc').read_text()+'\n#pragma pack(pop)'))
shutil.copyfile(p/'horse-summon-detail.h',src/'horse-summon-detail.h')
for f in ('horse_methods_regression.cpp','horse_methods_abi.cpp'):shutil.copyfile(p/f,tests/f)
fixture=(tests/'character_fixture.cpp').read_text()
for old,new in [('DWORD CHARACTER::GetMyHorseVnum() { std::abort(); }','// Actual GetMyHorseVnum is linked by this leaf suite.'),('void CHARACTER::SendHorseInfo() { std::abort(); }','// Actual SendHorseInfo is linked by this leaf suite.'),('void CHARACTER::ClearHorseInfo() { std::abort(); }','// Actual ClearHorseInfo is linked by this leaf suite.')]:
 assert old in fixture;fixture=fixture.replace(old,new)
(tests/'horse_methods_character_fixture.cpp').write_text(fixture)
make=tests/'Makefile';s=make.read_text();assert 'horse_methods_regression' not in s
s=s.replace('RUNTIME_TESTS = ','RUNTIME_TESTS = horse_methods_regression ',1)
needle='check: $(addsuffix $(EXEEXT),$(RUNTIME_TESTS))\n';assert needle in s
s=s.replace(needle,needle+'\t./horse_methods_regression$(EXEEXT)\n',1)
needle='check-abi:\n';assert needle in s
s=s.replace(needle,needle+'\t$(CXX) $(CPPFLAGS) $(CXXFLAGS) -Wno-invalid offsetof -m32 -fsyntax-only horse_methods_abi.cpp\n'.replace('-Wno-invalid offsetof','-Wno-invalid offsetof').replace('-Wno-invalid offsetof','-Wno-invalid'+ '-offsetof'),1)
handoff=(p/'Makefile.horse-methods.inc').read_text()
start=handoff.index('HORSE_METHOD_SOURCES =');end=handoff.index('# Add runtime binary')
s+='\n# Closed old horse/helper integration, original source placements.\n'+handoff[start:end]
make.write_text(s)
assert protected_hash=={f:hashlib.sha256((tests/f).read_bytes()).hexdigest() for f in protected}
after={rel:hashlib.sha256((root/rel).read_bytes()).hexdigest() for rel in files}
status={'step':'integrated_not_built','changed_files':files,'before':before,'after':after,'global_fixtures_unchanged':protected_hash,'full_suite_run':False,'runtime_test_pending':True,'abi_pending':True}
(p/'INTEGRATION_STATUS.json').write_text(json.dumps(status,indent=2))
(p/'INTEGRATION_RESUME.md').write_text('Integrated CLOSED horse methods only in original source placement; no broad HorseSummon/Spawn/Show/Goto. Exact changed files/before-after hashes/backups in INTEGRATION_STATUS.json and integration-backup/. Global character/desc/entity fixtures unchanged including shutdown boundary. Next run focused validate.py --suite horse_methods_regression --abi with existing compilers; no fullsuite. Save receipts/logs here. Stop after focused success or quota95/repeatedfail.\n')
print(json.dumps({'step':status['step'],'files':files,'protected_global_fixtures_unchanged':True}))
