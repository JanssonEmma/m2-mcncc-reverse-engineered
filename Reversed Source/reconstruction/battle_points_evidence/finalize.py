import pathlib,json
p=pathlib.Path('work/battle_points');d=json.loads((p/'constants-elf.json').read_text())
p.joinpath('constants.h.additions.candidate').write_text('// .rodata symbol0821b540,164 bytes; reference-guided const int spelling.\nextern const int aiPolymorphPowerByLevel[41];\n')
p.joinpath('constants.cpp.additions.candidate').write_text('const int aiPolymorphPowerByLevel[41] = {\n    '+', '.join(map(str,d['aiPolymorphPowerByLevel']['values']))+'\n};\n')
e=json.loads((p/'enums-dwarf.json').read_text());p.joinpath('enum-declarations.candidate').write_text('// Complete original enum metadata; merge into original owning headers only if needed.\n'+''.join('enum '+x['node']['name']+' {\n'+',\n'.join('    '+c['name']+' = '+str(c['value']) for c in x['children'])+'\n};\n' for x in e))
# Minimal signatures extracted precisely rather than borrowing modern qualifiers.
a=json.loads((p/'dwarf.json').read_text());names=['ComputeBattlePoints','GetPolymorphPoint','GetPolymorphPower','GetSkillLevel','GetMobTable','GetType','GetSubType','IsEquipped']
p.joinpath('signature-evidence.json').write_text(json.dumps([x for x in a if x['node']['name'] in names and x['attrs'].get('DW_AT_declaration')],indent=2))
