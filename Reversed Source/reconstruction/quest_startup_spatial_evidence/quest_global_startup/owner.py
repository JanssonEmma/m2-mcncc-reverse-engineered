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
from elftools.elf.elffile import ELFFile
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
    elf=ELFFile(f)
    sym=elf.get_section_by_name('.symtab')
    wanted={0x08052b40,0x08053e70,0x08053eb0,0x08055b70,0x08055df0,0x08056270,0x08056490,0x08057ac0,0x08057b90,0x0805a4b0}
    for s in sym.iter_symbols():
        if s['st_value'] in wanted or 'GetServerTimerArg' in s.name:
            print(hex(s['st_value']),s.name)
    for cu in elf.get_dwarf_info().iter_CUs():
        top=cu.get_top_DIE()
        lo=top.attributes.get('DW_AT_low_pc')
        hi=top.attributes.get('DW_AT_high_pc')
        if lo and hi and any(lo.value<=p<hi.value for p in wanted):
            print(top.attributes['DW_AT_name'].value,hex(lo.value),hex(hi.value))
    dwarf=elf.get_dwarf_info()
    for cu in dwarf.iter_CUs():
        top=cu.get_top_DIE()
        cu_name=top.attributes['DW_AT_name'].value
        if cu_name not in (b'questlua.cpp',b'questlua_global.cpp',b'questmanager.cpp'):
            continue
        line=dwarf.line_program_for_CU(cu)
        for entry in line.get_entries():
            st=entry.state
            if st and st.address in wanted|{0x081a80e0}:
                print(cu_name,hex(st.address),line['file_entry'][st.file-1].name,st.line)
