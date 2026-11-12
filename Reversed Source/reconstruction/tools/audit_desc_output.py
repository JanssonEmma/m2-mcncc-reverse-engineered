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
import json,re
r=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source\\reconstruction'))
x=json.loads((r/'descriptor-callers-ghidra.json').read_text(encoding='utf-8-sig'))
f=next(f for f in x['functions'] if f['function']['address']=='08139bb0')
source=(r.parent/'game/src/desc.cpp').read_text()
out={'target_sha256':x['target_sha256'],'scope':'Partial DESC module: ProcessOutput only; complete member declarations are metadata-backed, not recovery of all methods.','method':{'name':'DESC::ProcessOutput','address':'0x08139bb0','complete_decompilation':not f['truncated'],'source_definition_present':bool(re.search(r'int DESC::ProcessOutput\(\)',source))},'evidence':['descriptor-callers-ghidra.json','descriptor-output-assembly.json','desc-layout.json','desc-output-types.json'],'layout_validation':{'desc_size':580,'source_member_offsets':51,'packed_member_offsets':25,'account_size':333,'player_size':63,'string_size':4,'compatible_compiler_flags':['-m32','-D_GLIBCXX_USE_CXX11_ABI=0','-D_USE_32BIT_TIME_T']},'remaining_production_methods':['Constructor/Initialize','Destructor/Destroy','GetType','SetPhase','Input/Setup and other nonvirtual API'],'test_only_lifecycle_definitions':'reconstruction/tests/desc_fixture.cpp'}
(r/'desc-output-coverage.json').write_text(json.dumps(out,indent=2),encoding='utf-8')
print(json.dumps(out['method']))
