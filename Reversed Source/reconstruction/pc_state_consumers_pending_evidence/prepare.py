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
# Freeze workspace-only original consumer candidate against current PC source; no project writes/builds.
from pathlib import Path
import json,hashlib,ast,shutil
p=Path(__file__).resolve().parent;root=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'));h=lambda q:hashlib.sha256(Path(q).read_bytes()).hexdigest();src=root/'game/src/questpc.cpp';text=src.read_text(encoding='utf-8-sig')
assert 'void PC::SendFlagList(' not in text and 'void PC::SetCurrentQuestStateName(' not in text
candidate='#include "char.h"\n#include "questmanager.h"\n'+text+'\n'+(p/'questpc.cpp.fragment').read_text(encoding='utf-8-sig');(p/'questpc.cpp.candidate').write_text(candidate,encoding='utf-8')
sdk=Path('work/quest_lua_queries/sdk/liblua');sdkfiles=[{'path':'liblua/'+q.relative_to(sdk).as_posix(),'sha256':h(q)} for q in sorted(sdk.rglob('*')) if q.is_file()];assert len(sdkfiles)>=58 and sum(x['path'].endswith('.c') for x in sdkfiles)==28
api=Path('work/quest_level/pc_lookup/pc-full-record-api-dwarf.json')
if api.exists():shutil.copyfile(api,p/'original-full-PC-api.json')
for q in p.glob('*.py'):ast.parse(q.read_text(encoding='utf-8-sig'),filename=str(q))
m={'target_sha256':'61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c','status':'prepared_only_uncompiled_unapplied_WAIT_FOR_VERIFIED108','originals':[{'name':'PC::SetCurrentQuestStateName','address':'0805e2a0','instructions':61,'calls':7},{'name':'PC::SetQuestState(std::string,std::string)','address':'0805e380','instructions':35,'calls':3},{'name':'PC::SendFlagList','address':'08061980','instructions':121,'calls':8}],'files':[{'path':'game/src/questpc.cpp','before':h(src),'candidate':'questpc.cpp.candidate','after':h(p/'questpc.cpp.candidate')}],'current_read_guards':[{'path':r,'sha256':h(root/r)} for r in ['game/src/questpc.h','game/src/quest.h','common/singleton.h','game/src/char.cpp','game/src/char.h']],'future_query_dependencies':['GetQuestStateName081a5130','GetQuestStateIndex081a5240','real58file Lua5.0.3 supplied SDK in liblua and actual28object SDK archives'],'runtime_after_verified108':109,'sdk_expected_files':sdkfiles,'sdk_policy':'Supplied5.0.3 externalSDK, not recoveredold5.0 or proven internalABI. No fakeVM or packinglayoutrepair. PublicopaqueAPI/actualsource only.','package_files':{q.name:h(q)for q in p.iterdir()if q.is_file()and q.name!='MANIFEST.json'},'builds_or_project_mutations':False}
(p/'MANIFEST.json').write_text(json.dumps(m,indent=2)+'\n',encoding='utf-8');print('Frozen whole3consumer candidate+source guards+completeSDK expectations; uncompiled/unapplied; deferintegration untilverified108')
