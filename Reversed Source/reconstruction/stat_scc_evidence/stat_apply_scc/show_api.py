import json
from pathlib import Path
x=json.loads(Path('work/stat_apply_scc/original-api.json').read_text());print(json.dumps(x['strings'],indent=2));
for a in x['declarations']:
 if a['name'] in ('SetAccessorySocketExpireEvent','item_vid_event_info','ApplyPoint','ModifyPoints'):
  print(a['name'],a['attrs'],[(c['name'],c['type'].get('name')) for c in a['children']])
