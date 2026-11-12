import json,pathlib
x=json.loads(pathlib.Path('work/level_pk/dwarf.json').read_text())
for r in x:
 if r['cu']=='char.cpp' and r['node']['name'] in ('gm_level','level','m_bPKMode','GetGMLevel','SetLevel'):
  print(r['node'],r['attrs'])
