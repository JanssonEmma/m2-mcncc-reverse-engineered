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
from elftools.elf.elffile import ELFFile
with open(_resolve_portable_path('PROJECT_ROOT/Original Binaries/game'), 'rb') as stream:
    elf = ELFFile(stream)
    dwarf = elf.get_dwarf_info()
    def describe(die):
        out = {'die': hex(die.offset), 'tag': die.tag}
        for key in ('DW_AT_name', 'DW_AT_byte_size', 'DW_AT_external', 'DW_AT_location'):
            if key in die.attributes:
                val = die.attributes[key].value
                out[key] = val.decode('utf8', 'replace') if isinstance(val, bytes) else val
        if 'DW_AT_type' in die.attributes:
            out['type'] = describe(die.get_DIE_from_attribute('DW_AT_type'))
        if die.tag == 'DW_TAG_array_type':
            out['subranges'] = [{key: child.attributes[key].value for key in ('DW_AT_upper_bound', 'DW_AT_count') if key in child.attributes} for child in die.iter_children()]
        return out
    records = []
    for cu in dwarf.iter_CUs():
        if cu.get_top_DIE().attributes.get('DW_AT_name').value != b'tea.c':
            continue
        for die in cu.iter_DIEs():
            name = die.attributes.get('DW_AT_name')
            if name and name.value in (b'tea_code', b'tea_decode', b'TEA_Encrypt', b'TEA_Decrypt', b'tea_nilbuf'):
                record = describe(die)
                if die.tag == 'DW_TAG_subprogram':
                    record['parameters'] = [describe(c) for c in die.iter_children() if c.tag == 'DW_TAG_formal_parameter']
                records.append(record)
    records.append({'tea_nilbuf_address': '0x0842d440', 'section': [{'name': sec.name, 'type': sec['sh_type'], 'address': hex(sec['sh_addr']), 'size': sec['sh_size']} for sec in elf.iter_sections() if sec['sh_addr'] <= 0x0842d440 < sec['sh_addr'] + sec['sh_size']]})
    print(json.dumps(records, indent=2))
