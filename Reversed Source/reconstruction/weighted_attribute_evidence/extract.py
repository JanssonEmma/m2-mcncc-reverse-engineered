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
import sys,json,struct
sys.path.insert(0,_resolve_portable_path('USER_HOME\\Documents\\Codex\\2026-10-23\\i-h\\work\\python-deps'))
exec(Path(r'work/cleanup_types/extract.py').read_text().split("with open(r'F:")[0])
p=Path(__file__).resolve().parent;out=[]
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 elf=ELFFile(f);dw=elf.get_dwarf_info();ex=DWARFExprParser(dw.structs)
 names={'AddAttr','PutAttribute','PutAttributeWithLevel','AlterToMagicItem','aiItemMagicAttributePercentHigh','aiItemMagicAttributePercentLow','g_map_itemAttr','g_map_itemRare'}
 for cu in dw.iter_CUs():
  cuName=val(cu.get_top_DIE(),'DW_AT_name')
  if cuName not in ('constants.cpp','item_attribute.cpp','item.cpp'):continue
  lp=dw.line_program_for_CU(cu)
  for d in cu.iter_DIEs():
   name=val(d,'DW_AT_name')
   if name not in names and not (name and 'ItemAttrTable' in name):continue
   x={'cu':cuName,'node':desc(d),'pc':val(d,'DW_AT_low_pc'),'declaration':val(d,'DW_AT_declaration'),'children':[]}
   fileIdx=val(d,'DW_AT_decl_file');x['file']=lp['file_entry'][fileIdx-1].name.decode(errors='replace') if fileIdx else None
   for c in d.iter_children():
    a=c.attributes.get('DW_AT_data_member_location');loc=None
    if a:loc=[{'op':o.op_name,'args':o.args} for o in ex.parse_expr(a.value)] if isinstance(a.value,list) else a.value
    x['children'].append({'tag':c.tag,'name':val(c,'DW_AT_name'),'type':desc(typ(c)),'location':loc,'pc':val(c,'DW_AT_low_pc'),'linkage':val(c,'DW_AT_MIPS_linkage_name')})
   out.append(x)
 def data(a,n):
  seg=next(s for s in elf.iter_segments() if s['p_type']=='PT_LOAD' and s['p_vaddr']<=a<s['p_vaddr']+s['p_filesz'])
  f.seek(seg['p_offset']+a-seg['p_vaddr']);return f.read(n)
 symbols=[];tables=[]
 for s in elf.get_section_by_name('.symtab').iter_symbols():
  if s.name in ('aiItemMagicAttributePercentHigh','aiItemMagicAttributePercentLow') or s.name.startswith('g_map_item'):
   symbols.append({'name':s.name,'address':'%08x'%s['st_value'],'size':s['st_size'],'section':s['st_shndx']})
   if s.name.startswith('aiItem'):
    b=data(s['st_value'],s['st_size']);tables.append({'symbol':s.name,'address':'%08x'%s['st_value'],'bytes':b.hex(),'values':list(struct.unpack('<'+'i'*(len(b)//4),b))})
(p/'full-records-api-dwarf.json').write_text(json.dumps(out,indent=2));(p/'globals-tables.json').write_text(json.dumps({'symbols':symbols,'tables':tables},indent=2))
print(json.dumps({'symbols':symbols,'tables':tables},indent=2))
print([(x['cu'],x['node']['name'],x['node'].get('size'),x['pc'],x['file']) for x in out if x['pc'] or x['node'].get('size')])
