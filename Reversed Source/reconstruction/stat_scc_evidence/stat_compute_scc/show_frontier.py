from pathlib import Path
import json
for addr in ['0813e780','0813e880','08073890']:
 x=json.loads(Path('work/mount_polymorph/'+addr+'-calls.json').read_text());x=x.get('structuredContent') or (json.loads(x['content'][0]['text']) if 'content'in x else x);print(addr,sorted(set(y['name'] for y in x.get('callees',{}).get('items',[]))))
