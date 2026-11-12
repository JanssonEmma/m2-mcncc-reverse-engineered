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
from elftools.elf.elffile import ELFFile
from elftools.dwarf.dwarf_expr import DWARFExprParser
exec(Path('work/cleanup_types/extract.py').read_text(encoding='utf-8-sig').split("with open(r'F:")[0])
import json, subprocess
p = Path('work/pc_quest_storage')
out = {'methods': [], 'inline_instances': [], 'records': []}
names = {'quest_begin', 'quest_end', 'quest_find', 'HasQuest', 'GetQuest', 'GetCurrentQuestIndex', 'SetQuest', 'SetQuestState', 'AddQuestStateChange', 'ClearQuest', 'TQuestStateChangeInfo', 'AArgScript'}
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'), 'rb') as f:
    dw = ELFFile(f).get_dwarf_info()
    ex = DWARFExprParser(dw.structs)
    for cu in dw.iter_CUs():
        unit = val(cu.get_top_DIE(), 'DW_AT_name')
        if unit not in ('questpc.cpp', 'questnpc.cpp', 'questmanager.cpp', 'questlua.cpp', 'questlua_quest.cpp'):
            continue
        lp = dw.line_program_for_CU(cu)

        def place(d):
            i = val(d, 'DW_AT_decl_file')
            return {'file': lp['file_entry'][i - 1].name.decode() if i else None, 'line': val(d, 'DW_AT_decl_line')}
        for d in cu.iter_DIEs():
            n = val(d, 'DW_AT_name')
            if n not in names:
                continue
            if d.tag in ('DW_TAG_structure_type', 'DW_TAG_class_type') and val(d, 'DW_AT_byte_size') and (n in ('AArgScript', 'TQuestStateChangeInfo')):
                r = {'unit': unit, 'record': desc(d), 'placement': place(d), 'fields': []}
                for c in d.iter_children():
                    if c.tag != 'DW_TAG_member':
                        continue
                    a = c.attributes.get('DW_AT_data_member_location')
                    loc = [{'op': o.op_name, 'args': o.args} for o in ex.parse_expr(a.value)] if a and isinstance(a.value, list) else a.value if a else None
                    r['fields'].append({'name': val(c, 'DW_AT_name'), 'type': desc(typ(c)), 'location': loc})
                out['records'].append(r)
            if d.tag in ('DW_TAG_subprogram', 'DW_TAG_inlined_subroutine'):
                r = {'unit': unit, 'tag': d.tag, 'name': n, 'die': hex(d.offset), 'placement': place(d), 'return': desc(typ(d)), 'attrs': {k: val(d, k) for k in ('DW_AT_inline', 'DW_AT_low_pc', 'DW_AT_high_pc', 'DW_AT_ranges', 'DW_AT_artificial', 'DW_AT_MIPS_linkage_name')}, 'params': [{'name': val(c, 'DW_AT_name'), 'type': desc(typ(c))} for c in d.iter_children() if c.tag == 'DW_TAG_formal_parameter']}
                if 'DW_AT_ranges' in d.attributes:
                    base = cu.get_top_DIE().attributes.get('DW_AT_low_pc')
                    base = base.value if base else 0
                    rr = []
                    for q in dw.range_lists().get_range_list_at_offset(d.attributes['DW_AT_ranges'].value, cu=cu):
                        if hasattr(q, 'base_address'):
                            base = q.base_address
                        elif hasattr(q, 'begin_offset'):
                            rr.append([q.begin_offset + (0 if q.is_absolute else base), q.end_offset + (0 if q.is_absolute else base)])
                    r['ranges'] = rr
                out['inline_instances' if d.tag == 'DW_TAG_inlined_subroutine' else 'methods'].append(r)
p.joinpath('metadata.json').write_text(json.dumps(out, indent=2) + '\n', encoding='utf-8')
for name, a, b in [('SetQuest', 134604192, 134605824), ('storage', 134614032, 134614624), ('state-int', 134603776, 134604192), ('index', 134602144, 134602240), ('ClearQuest', 134617072, 134617664)]:
    r = subprocess.run(['C:\\Strawberry\\c\\bin\\objdump.exe', '-d', '--start-address=' + hex(a), '--stop-address=' + hex(b), _resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game')], capture_output=True, text=True, check=True)
    p.joinpath('evidence/' + name + '-assembly.txt').write_text(r.stdout, encoding='utf-8')
print('inline counts', [(n, sum((r['name'] == n for r in out['inline_instances']))) for n in ['quest_begin', 'quest_end', 'quest_find', 'AArgScript', 'TQuestStateChangeInfo']])
print([(r['unit'], r['attrs'], r.get('ranges')) for r in out['inline_instances'] if r['name'] == 'AArgScript'][:3])
