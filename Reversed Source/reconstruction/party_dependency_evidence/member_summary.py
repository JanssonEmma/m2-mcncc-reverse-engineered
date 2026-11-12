import json,pathlib
x=json.loads(pathlib.Path('work/party_dependency/dwarf.json').read_text())
for r in x:
 if r['node']['name']=='SMember' and r['node']['size']: print(json.dumps(r,indent=2))
