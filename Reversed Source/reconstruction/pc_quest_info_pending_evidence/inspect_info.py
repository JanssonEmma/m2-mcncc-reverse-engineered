import json
from pathlib import Path
x=json.loads(Path('work/pc_quest_info/metadata.json').read_text());print('ClearSend',[(r['tag'],r['placement'],r['attrs']) for r in x['methods']+x['inline'] if r['name']=='ClearSendFlag'])
