import json,pathlib
x=json.loads(pathlib.Path('work/party_dependency/records-enums-dwarf.json').read_text())
for r in x[:2]:
 for c in r['children']:
  if c['node']['tag']=='DW_TAG_member': print(c['node']['name'],c['node'].get('type'))
