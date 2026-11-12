import json,pathlib
x=json.loads(pathlib.Path('work/party_dependency/dwarf.json').read_text())
for r in x:
 if r['node']['name']=='CParty' and r['node']['size']:
  for c in r['children']:
   n=c['node'];a=c['attrs'];print(n['tag'],n['name'],n.get('type'),a)
