import json,hashlib
from pathlib import Path
p=Path('work/guild_level');i=json.loads(Path('work/target-function-inventory.json').read_text())
exact=[f for f in i['functions'] if f['name'] in ('CHARACTER::Packet','CHARACTER::BufferedPacket')]
assert not exact
d=json.loads((p/'old-types-signatures.json').read_text())
character=[r for r in d['methods'] if (r['linkage'] or '').startswith('_ZN9CHARACTER') and r['name'] in ('Packet','BufferedPacket')]
assert not character
out={'target_sha256':i['target_sha256'],'CHARACTER_Packet_BufferedPacket_inventory_matches':exact,'inspected_CHAR_DWARF_matches':character,'actual_LevelChange_calls':'0808f5d0-calls.json','existing_complete_wrappers':{'CLIENT_DESC::DBPacket':'0813fbd0-decompile.json','CLIENT_DESC::DBPacketHeader':'0813fb40-decompile.json','DESC::BufferedPacket':'08139cb0-decompile.json','DESC::Packet':'08139d10-decompile.json'},'conclusion':'No target CHARACTER::Packet/BufferedPacket methods found; LevelChange directly uses the complete real DESC methods after GetDesc. Do not invent CHAR wrappers or substitute app mocks. CGuild::Packet is a separate helper, not a LevelChange dependency.'}
(p/'packet-wrapper-verification.json').write_text(json.dumps(out,indent=2));print(json.dumps(out))
