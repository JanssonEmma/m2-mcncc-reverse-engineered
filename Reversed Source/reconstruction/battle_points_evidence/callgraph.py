from pathlib import Path
import json,re
p=Path('work/battle_points');out={}
for name in ['compute','polymorph','power','skill-level','mob-table']:
 s=p.joinpath(name+'-assembly.txt').read_text();calls=[]
 for l in s.splitlines():
  m=re.search(r'^\s*([0-9a-f]+):.*\bcall\s+([0-9a-f]+)\s+<([^>]+)>',l)
  if m:calls.append({'site':'0x'+m[1],'address':'0x'+m[2],'symbol':m[3]})
 out[name]=calls
p.joinpath('direct-callgraph.json').write_text(json.dumps(out,indent=2))
