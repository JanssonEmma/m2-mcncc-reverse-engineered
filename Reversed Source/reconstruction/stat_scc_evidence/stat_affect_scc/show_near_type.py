import json
from pathlib import Path
x=json.loads(Path('work/stat_affect_scc/original-metadata.json').read_text());r=next(r for r in x['records'] if r['record']['name']=='FFindNearVictim');print(json.dumps(r['members'][-1],indent=2))
