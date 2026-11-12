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
import json
from pathlib import Path
exec(Path('work/cleanup_types/extract.py').read_text().split('with open(')[0])
names={'SetWear','SetImmuneFlag','StopAccessorySocketExpireEvent','ModifyPoints','Unequip','SyncQuickslot','ChainQuickslotItem','RemoveFromCharacter','DestroyItem','AutoGiveItem','FN_check_item_socket','GetEmptyInventory','IsEmptyItemGrid','GetQuickslot','SetQuickslot','DelQuickslot','SetCount','AddToGround','StartDestroyEvent','CreateItem','ItemLog'}
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 dw=ELFFile(f).get_dwarf_info(); out=[]
 for cu in dw.iter_CUs():
  if val(cu.get_top_DIE(),'DW_AT_name') not in ('char_item.cpp','char_quickslot.cpp','item.cpp','item_manager.cpp','log.cpp'):continue
  for d in cu.iter_DIEs():
   if d.tag!='DW_TAG_subprogram' or val(d,'DW_AT_name') not in names:continue
   out.append({'cu':val(cu.get_top_DIE(),'DW_AT_name'),'name':val(d,'DW_AT_name'),'pc':val(d,'DW_AT_low_pc'),'return':desc(typ(d)),'linkage':val(d,'DW_AT_MIPS_linkage_name'),'params':[{'name':val(c,'DW_AT_name'),'type':desc(typ(c))} for c in d.iter_children() if c.tag=='DW_TAG_formal_parameter']})
 Path('work/point_change/auto_give/set_count/api-dwarf.json').write_text(json.dumps(out,indent=2),encoding='utf8')
 print(len(out))


