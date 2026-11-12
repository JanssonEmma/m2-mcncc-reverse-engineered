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
import json,re,hashlib,ast
p=Path('work/quest_manager_indices')
s=Path('work/quest_pc_lookup_o2.py').read_text().replace('quest_pc_lookup','quest_manager_indices').replace('quest-pc-lookup','quest-manager-indices').replace('PC lifecycle/lookup','quest manager index queries')
s=s.replace("'quest_flag_fixture.cpp','desc_fixture.cpp'","'quest_flag_fixture.cpp','desc_fixture.cpp','quest_manager_indices_desc_fixture.cpp'")
needle=" subprocess.run([str(cwd/'quest_manager_indices_o2.exe')],cwd=cwd,stdout=log,stderr=subprocess.STDOUT,check=True)"
assert needle in s
s=s.replace(needle,needle+"\n fatal=subprocess.run([str(cwd/'quest_manager_indices_o2.exe'),'--fatal-missing'],cwd=cwd,stdout=log,stderr=subprocess.STDOUT)\n assert fatal.returncode not in (0,9),'Missing-index assertion unexpectedly returned'\n log.flush()\n assert 'INDEX-MISSING 777' in logpath.read_text(errors='replace')\n assert 'descriptor fixture assertion' in logpath.read_text(errors='replace')")
s=s.replace("'real_events':True,","'real_events':True,'fatal_missing_expected_nonzero':fatal.returncode,")
(p/'run_optimized.py').write_text(s)
for n in ('prepare_integration.py','apply_integration.py','run_optimized.py'):ast.parse((p/n).read_text())
a=json.loads((p/'full-original-manager-data-api.json').read_text())[0];methods=[x for x in a['children'] if x['tag']=='DW_TAG_subprogram'];before=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source\\game\\src\\questmanager.h')).read_text()
newnames=['FindNPCIDByName','GetCurrentQuestName','GetQuestIndexByName','GetQuestNameByIndex']
(p/'api-audit.json').write_text(json.dumps({'old_record':a['record'],'metadata_method_entries':len(methods),'new_genuine_public_declarations':[x for x in methods if x['name'] in newnames],'existing_header_policy':'Full logical data fields retained. Existing API is partial; only four proven declarations added. This does NOT claim complete CQuestManager API.','remaining_metadata_names_not_in_current_header':sorted({x['name'] for x in methods if x['name'] and x['name'] not in before and x['name'] not in newnames})},indent=2))
sha=lambda q:hashlib.sha256(q.read_bytes()).hexdigest()
entries=[]
for addr,name,test in [('081a1f00','FindNPCIDByName','Lookup'),('081a5720','GetCurrentQuestName','Lookup'),('081a5a90','GetQuestIndexByName','Lookup'),('081a5b60','GetQuestNameByIndex','Lookup')]:
 entries.append({'name':'quest::CQuestManager::'+name,'address':addr,'kind':'standalone original function','source':'game/src/questmanager.cpp','source_anchor':'CQuestManager::'+name+'(','test':'reconstruction/tests/quest_manager_indices_regression.cpp','test_anchor':test,'evidence':addr+'.json','evidence_sha256':sha(p/(addr+'.json'))})
(p/'normalized-originals.json').write_text(json.dumps({'elf_sha256':'61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c','standalone_original_exports':4,'entries':entries,'hash_inline_dependency':{'name':'quest::CQuestManager::stringhash::operator()(const std::string&) const','source':'game/src/questmanager.h','test_anchor':'HashVectors','evidence':['hash-inline-dwarf.json','081a5a90.json','081a5770.json'],'standalone_original_address':None,'not_a_standalone_export':True},'excluded':['RegisterQuest081a5770','LoadStartQuest081a4690','Lua-backed state-index/name resolution','manager destructor Lua ownership']},indent=2))
print('C++03 ABI/source candidates, four original mappings+inlinehash distinction, API audit and unexecuted optimized/fatal helper saved.')
