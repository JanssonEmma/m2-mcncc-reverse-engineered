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
def rt(d):
 if d is None:return 'void'
 n=val(d,'DW_AT_name')
 if n:return n
 t=rt(typ(d));return {'DW_TAG_pointer_type':t+'*','DW_TAG_reference_type':t+'&','DW_TAG_const_type':'const '+t,'DW_TAG_volatile_type':'volatile '+t}.get(d.tag,t)
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 dw=ELFFile(f).get_dwarf_info()
 for cu in dw.iter_CUs():
  if val(cu.get_top_DIE(),'DW_AT_name')!='db.cpp':continue
  for d in cu.iter_DIEs():
   if val(d,'DW_AT_name')!='DBManager' or d.tag!='DW_TAG_structure_type' or not val(d,'DW_AT_byte_size'):continue
   lines=['// Full original declaration API from db.cpp DWARF; no method bodies.','// Original DBManager sizeof=2372, CAsyncSQL sizeof=1152 (old toolchain).','class DBManager : public singleton<DBManager> {','public:']
   for c in d.iter_children():
    if c.tag!='DW_TAG_subprogram' or val(c,'DW_AT_name')=='operator=':continue
    name=val(c,'DW_AT_name');params=list(c.iter_children()); ps=[rt(typ(p)) for p in params if p.tag=='DW_TAG_formal_parameter'][1:]
    if any(p.tag=='DW_TAG_unspecified_parameters' for p in params):ps.append('...')
    pre='virtual ' if val(c,'DW_AT_virtuality') else ''
    if val(c,'DW_AT_accessibility')==3:lines.append('private:')
    else:lines.append('public:')
    lines.append('    '+pre+('' if name in ('DBManager','~DBManager') else rt(typ(c))+' ')+name+'('+', '.join(ps)+');')
   lines+=['private:','    CAsyncSQL m_sql;','    CAsyncSQL m_sql_direct;','    bool m_bIsConnect;','    std::map<std::string,std::string> m_map_dbstring;','    std::vector<std::string> m_vec_GreetMessage;','    std::map<DWORD,CLoginData*> m_map_pkLoginData;','    std::map<std::string,CLoginData*> mapLDBilling;','    std::vector<TUseTime> m_vec_kUseTime;','};']
   Path('work/point_change/auto_give/DBManager.declarations.candidate').write_text('\n'.join(lines)+'\n',encoding='utf8')
