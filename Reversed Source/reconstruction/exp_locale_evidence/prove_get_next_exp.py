import json
from pathlib import Path
p=Path('work/exp_locale');meta=json.loads((p/'api-metadata.json').read_text())
instructions=json.loads((p/'0812a2d0-instructions.json').read_text())['items']
other=json.loads(Path('work/stat_dependency/0812e340-instructions.json').read_text())
for page in other:instructions+=page['items']
out=[]
for r in meta['inline_ranges']:
 lo=r['low_pc'];hi=r['high_pc']
 subset=[i for i in instructions if lo<=int(i['address'],16)<hi]
 out.append({'metadata':r,'instructions':subset})
(p/'get-next-exp-inline-proof.json').write_text(json.dumps(out,indent=2))
print(json.dumps([{'range':[hex(r['metadata']['low_pc']),hex(r['metadata']['high_pc'])],'instructions':[i['instruction'] for i in r['instructions']]} for r in out]))
