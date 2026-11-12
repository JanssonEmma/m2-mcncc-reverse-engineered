from pathlib import Path
import json
x = json.loads(Path('work/pc_quest_storage/metadata.json').read_text(encoding='utf-8-sig'))
for r in x['methods'] + x['inline_instances']:
    if r['name'] in ('AArgScript', 'TQuestStateChangeInfo'):
        print(r['unit'], r['tag'], r['placement'], {k: hex(v) if k.endswith('_pc') and v is not None else v for k, v in r['attrs'].items()}, [(a['name'], a['type'].get('name'), a['type'].get('type', {}).get('name')) for a in r['params']])
