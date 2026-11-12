from pathlib import Path
import json,hashlib
p=Path('work/pc_quest_info');p.joinpath('MANIFEST.json').write_text(json.dumps({'scope':'PREPARATION ONLY13complete original bodies+fullpacket+code-backedcontextgetter, no tests/builds/Fwrites/integration/status claims','files':[{'path':str(f).replace('\\','/'),'sha256':hashlib.sha256(f.read_bytes()).hexdigest()} for f in sorted(p.rglob('*')) if f.is_file() and f.name!='MANIFEST.json' and '__pycache__' not in str(f)]},indent=2)+'\n',encoding='utf-8')
