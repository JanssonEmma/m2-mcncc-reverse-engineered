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
import sys,json,hashlib
sys.path.insert(0,str(Path('work/python-deps').resolve()))
exec(Path('work/cleanup_types/extract.py').read_text().split("with open(r'F:")[0])
p=Path('work/quest_registration');out=[]
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 elf=ELFFile(f);dw=elf.get_dwarf_info()
 for cu in dw.iter_CUs():
  if val(cu.get_top_DIE(),'DW_AT_name') not in ('questmanager.cpp','config.cpp'):continue
  for d in cu.iter_DIEs():
   if d.tag=='DW_TAG_variable' and val(d,'DW_AT_name')=='g_setQuestObjectDir':out.append({'cu':val(cu.get_top_DIE(),'DW_AT_name'),'die':hex(d.offset),'type':desc(typ(d)),'declaration':val(d,'DW_AT_declaration'),'external':val(d,'DW_AT_external'),'location':val(d,'DW_AT_location')})
 syms=[{'name':s.name,'address':format(s['st_value'],'08x'),'size':s['st_size']} for s in elf.get_section_by_name('.symtab').iter_symbols() if s.name in ('g_setQuestObjectDir','_ZN5quest13CQuestManager14LoadStartQuestERKSsj','_ZN5quest13CQuestManager13RegisterQuestERKSsj')]
(p/'global-api-evidence.json').write_text(json.dumps({'globals':out,'symbols':syms,'actual_owner':'game/src/config.cpp','actual_declaration':'game/src/config.h','existing_runtime_object':'item_socket_config.o includes actual config.cpp'},indent=2))
a=json.loads(Path('work/quest_manager_indices/full-original-manager-data-api.json').read_text())[0];(p/'method-api-evidence.json').write_text(json.dumps([c for c in a['children'] if c.get('name') in ('LoadStartQuest','RegisterQuest')],indent=2))
q=p/'quest_registration_regression.cpp';s=q.read_text();needle='    g_setQuestObjectDir=saved;';s=s.replace(needle,'    ResetLogs();manager.RegisterQuest("maximum",UINT_MAX);\n    assert(!loadLogs&&registerLogs==1&&manager.GetQuestIndexByName("maximum")==UINT_MAX&&manager.GetQuestNameByIndex(UINT_MAX)=="maximum");\n'+needle);q.write_text(s)
print('Saved genuine config-global and two full original API signatures; no Fwrites.')
