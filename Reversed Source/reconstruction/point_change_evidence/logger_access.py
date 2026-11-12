import json,pathlib
x=json.loads(pathlib.Path('work/point_change/logger-quest-dwarf.json').read_text())
for r in x:
 if r['node']['name']=='LogManager' and r['node'].get('size'):
  print([(c['node']['name'],c['node']['tag'],c['attrs'].get('DW_AT_accessibility')) for c in r['children']])
