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
import json,hashlib,ast
p=Path(__file__).resolve().parent;root=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'));h=lambda q:hashlib.sha256(Path(q).read_bytes()).hexdigest();load=lambda q:json.loads(Path(q).read_text(encoding='utf-8-sig'));src=root/'game/src/questmanager.cpp';header=root/'game/src/questmanager.h';npc=root/'game/src/questnpc.cpp';fixture=root/'reconstruction/tests/quest_flag_fixture.cpp';text=src.read_text(encoding='utf-8-sig');ht=header.read_text(encoding='utf-8-sig')
assert 'void CQuestManager::Destroy(' not in text and 'CQuestManager::~CQuestManager(' not in text and not npc.exists()
assert 'CQuestManager::~CQuestManager()' in fixture.read_text(encoding='utf-8-sig') and 'NPC::~NPC()' in fixture.read_text(encoding='utf-8-sig')
(p/'questmanager.cpp.candidate').write_text(text+'\n'+(p/'questmanager.cpp.fragment').read_text(encoding='utf-8-sig'),encoding='utf-8');ht=ht.replace('    CQuestManager();',(p/'questmanager.h.fragment').read_text(encoding='utf-8-sig')+'\n    CQuestManager();',1);(p/'questmanager.h.candidate').write_text(ht,encoding='utf-8')
sdkfiles=[]
for row in load(Path('work/quest_lua_queries/sdk-provenance.json'))['files']:
 rel='liblua/'+Path(row['path']).as_posix();assert h(root/rel)==row['sha256'];sdkfiles.append(dict(path=rel,sha256=row['sha256']))
rel='liblua/RECONSTRUCTION_PROVENANCE.md';sdkfiles.append(dict(path=rel,sha256=h(root/rel)));assert len(sdkfiles)==59
addresses=['081a0720','0819fa20','0819fe70','081a02c0','081adf50','081ae0c0','081ae180','081ae260'];original=[]
for a in addresses:
 d=load(p/(a+'-decompile.json'));asm=load(p/(a+'-assembly.json'));calls=load(p/(a+'-calls.json'));assert not d['truncated'] and not asm['has_more'] and not calls['callees']['has_more'];original.append(d)
target='61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c';(p/'quest-ownership-addressed-ghidra.json').write_text(json.dumps(dict(target_sha256=target,functions=original),indent=2)+'\n',encoding='utf-8')
files=[]
for rel,candidate in [('game/src/questmanager.cpp','questmanager.cpp.candidate'),('game/src/questmanager.h','questmanager.h.candidate'),('game/src/questnpc.cpp','questnpc.cpp.candidate'),('reconstruction/tests/quest_flag_fixture.cpp','quest_flag_fixture.cpp.candidate')]:
 q=root/rel;files.append(dict(path=rel,before=h(q) if q.exists() else None,candidate=candidate,after=h(p/candidate)))
m=dict(target_sha256=target,status='PREPARED_ONLY_UNCOMPILED_UNAPPLIED_NEXTWINDOW',semantic_methods=4,original_address_mappings=8,compiler_copy_context_only='081ab460 implicitNPCcopy, NOTdefaultctor and NOTnewstandalonebody mapping',files=files,current_read_guards=[dict(path=r,sha256=h(root/r)) for r in ['game/src/questnpc.h','game/src/questpc.cpp','game/src/questpc.h','game/src/quest.h','common/singleton.h','game/src/questevent.cpp','game/src/event.cpp','libthecore/include/main.h']],sdk_expected_files=sdkfiles,originals=[dict(address=a,instructions=load(p/(a+'-assembly.json'))['total'],callees=load(p/(a+'-calls.json'))['callees']['total']) for a in addresses],runtime_before=113,runtime_after=114,builds_or_project_mutations=False,scope='WholeDestroy/CQMvirtualdtor/fullNPCdefaultctor/fullNPCdtor; genuine fullcompiler-ownedfield/base tear; retireoldfixtureonlyafterglobalNPCsource propagation. PopulatedAArgScript recordconstructor unresolved/staticonly, emptyargvec/reserve/nullQuestStateptr tests only. SuppliedSDKrevision/internalABI limits unchanged.')
(p/'MANIFEST.json').write_text(json.dumps(m,indent=2)+'\n',encoding='utf-8')
for q in p.glob('*.py'):ast.parse(q.read_text(encoding='utf-8-sig'))
print('Prepared ownership4semantic/8originaladdressvariants +implicitcopycontext, fullsource/header/fixture candidate guards. NoFwrites/builds.')
