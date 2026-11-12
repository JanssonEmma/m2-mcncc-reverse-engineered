from pathlib import Path
import json
p=Path('work/battle_points');a=json.loads((p/'dwarf.json').read_text());r=[x for x in a if x['node']['name']=='IsEquipped'];print(len(r));print([(x['cu'],x['node'],x['attrs']) for x in r][:3]);print('Combined candidate lines',len(p.joinpath('char-battle-complete.cpp.candidate').read_text().splitlines()))
