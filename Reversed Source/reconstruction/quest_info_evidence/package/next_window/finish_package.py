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
p=Path('work/pc_quest_info/next_window/pc_quest_info_regression.cpp');s=p.read_text(encoding='utf-8-sig');s=s.replace('    pc->m_iSendToClient=INT_MIN;pc->SetCurrentQuestTitle','    const unsigned savedA=buffer_size(outputA),savedB=buffer_size(outputB);\n    pc->m_iSendToClient=INT_MIN;pc->SetCurrentQuestTitle');needle=' && !a->bStart);\n    // Temporary title';assert s.count(needle)==1;s=s.replace(needle,' && !a->bStart);assert(buffer_size(outputA)==savedA && buffer_size(outputB)==savedB);\n    // Temporary title');p.write_text(s,encoding='utf-8')
base=Path('work/pc_quest_storage')
p=Path('work/pc_quest_info/next_window');s=(base/'apply.py').read_text(encoding='utf-8-sig').replace('STALE actual manager dependency','STALE actual dependency');p.joinpath('apply.py').write_text(s,encoding='utf-8')
s=(base/'validate.py').read_text(encoding='utf-8-sig').replace('pc_quest_storage','pc_quest_info').replace('pc-quest-storage','pc-quest-info');s=s.replace("'../../game/src/questmanager.cpp']:assert required in cmd,required","'../../game/src/questmanager.cpp','../../game/src/buffer_manager.cpp']:assert required in cmd,required");p.joinpath('validate.py').write_text(s,encoding='utf-8')
f=p/'prepare.py';s=f.read_text(encoding='utf-8-sig');s=s.replace("'runtime_count_after':len(registry),'files':[]}","'runtime_count_after':len(registry),'dependencies':[{'path':rel,'sha256':hashlib.sha256((root/rel).read_bytes()).hexdigest()} for rel in ['game/src/questmanager.cpp','game/src/questpc.h','game/src/buffer_manager.cpp','game/src/buffer_manager.h']],'files':[]}");f.write_text(s,encoding='utf-8')
make=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source\\reconstruction\\tests\\Makefile')).read_text(encoding='utf-8-sig')
for line in make.splitlines():
 if line.startswith('quest_flag_regression$(EXEEXT):'):print(line[:250])
