from pathlib import Path
import json
p=Path('work/level_pk');a=json.loads((p/'dwarf.json').read_text());done=set();s='// Add EGMLevels to common/length.h and EPKModes to char.h, as original.\n'
for r in a:
 if r['node']['tag']=='DW_TAG_enumeration_type' and r['node']['name'] not in done:
  done.add(r['node']['name']);s+='enum '+r['node']['name']+' {\n'+',\n'.join('    '+x['node']['name']+' = '+str(x['attrs']['DW_AT_const_value']) for x in r['children'])+'\n};\n'
(p/'enums.h.additions.candidate').write_text(s)
(p/'char.h.additions.candidate').write_text('void SetLevel(BYTE level);\nBYTE GetGMLevel(); // Old API is NON-CONST, unlike newer reference.\nvoid SetPKMode(BYTE bPKMode);\n')
(p/'config.additions.candidate').write_text('// config.h: mutable one-byte config symbol, not const/enum.\nextern BYTE PK_PROTECT_LEVEL;\n// config.cpp (or existing config storage module):\nBYTE PK_PROTECT_LEVEL=30;\n')
