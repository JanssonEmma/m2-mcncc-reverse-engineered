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
import pathlib,json
exec(pathlib.Path('work/cleanup_types/extract.py').read_text().split('with open(')[0])
out=[]
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 e=ELFFile(f);dw=e.get_dwarf_info()
 for cu in dw.iter_CUs():
  cn=val(cu.get_top_DIE(),'DW_AT_name')
  if cn not in ('char.cpp','char_skill.cpp'):continue
  lp=dw.line_program_for_CU(cu)
  def filepath(i):
   if not i:return None
   fe=lp['file_entry'][i-1];name=fe.name.decode('utf8','replace');dr=fe.dir_index
   return (lp['include_directory'][dr-1].decode('utf8','replace')+'/' if dr else '')+name
  for d in cu.iter_DIEs():
   if val(d,'DW_AT_name') not in ('GetSkillLevel','GetMobTable','GetPolymorphPower','GetPolymorphPoint'):continue
   if d.tag!='DW_TAG_subprogram':continue
   out.append({'cu':cn,'die':hex(d.offset),'name':val(d,'DW_AT_name'),'decl_file':filepath(val(d,'DW_AT_decl_file')),'decl_line':val(d,'DW_AT_decl_line'),'inline':val(d,'DW_AT_inline'),'low_pc':val(d,'DW_AT_low_pc'),'linkage':val(d,'DW_AT_MIPS_linkage_name')})
 syms=[]
 for s in e.get_section_by_name('.symtab').iter_symbols():
  if s.name=='_ZN9CHARACTER13GetSkillLevelEm':syms.append({'name':s.name,'binding':s['st_info']['bind'],'size':s['st_size'],'address':hex(s['st_value'])})
p=pathlib.Path('work/battle_points');p.joinpath('helper-placement-evidence.json').write_text(json.dumps({'dwarf':out,'symbols':syms},indent=2));print(json.dumps({'dwarf':out,'symbols':syms},indent=2))
