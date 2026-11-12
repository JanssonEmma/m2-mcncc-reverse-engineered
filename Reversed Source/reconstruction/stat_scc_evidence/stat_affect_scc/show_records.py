from pathlib import Path
import json
x=json.loads(Path('work/stat_affect_scc/original-metadata.json').read_text());
for r in x['records']:
 if r['record']['name'] in ('FFindNearVictim','packet_fly','TPacketGCAffectRemove','SPacketGDRemoveAffect'):print(r['record']['name'],[(m['name'],m['location'],m['type']['tag'],m['type']['name']) for m in r['members']])
