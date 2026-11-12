import json,pathlib
x=json.loads(pathlib.Path('work/point_change/dwarf.json').read_text())
for r in x:
 if r['attrs'].get('DW_AT_declaration') and r['node']['tag']=='DW_TAG_subprogram':
  print(r['node']['name'],r['node'].get('type'),r['attrs'],[(c['node'].get('type'),c['attrs']) for c in r['children'] if c['node']['tag']=='DW_TAG_formal_parameter'])
