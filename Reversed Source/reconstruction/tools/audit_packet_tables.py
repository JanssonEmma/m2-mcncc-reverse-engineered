import json, re
from pathlib import Path
root=Path(__file__).resolve().parent.parent.parent
r=root/'reconstruction'
e=json.loads((r/'packet-tables-ghidra.json').read_text(encoding='utf-8-sig'))
manifest=json.loads((r/'packet-registration-manifest.json').read_text())
s='\n'.join((root/'game/src'/n).read_text() for n in ('packet_info.cpp','input_udp.cpp'))
summary={}
for cls in ('CG','GG','UDP'):
 name='CPacketInfo'+cls
 body=re.search(name+r'::'+name+r'\(\)\s*\{(.*?)\n\}',s,re.S).group(1)
 rows=[(int(h,0),int(z,0),n,b=='true') for h,z,n,b in re.findall(r'Set\((\w+),\s*(\w+),\s*"([^"]+)",\s*(true|false)\)',body)]
 assert rows==[(x['header'],x['size'],x['name'],x['sequence']) for x in manifest[cls]]
 assert len({x[0] for x in rows})==len(rows)
 constructors=destructors=0
 for address,x in e.items():
  assert not x['truncated']
  if x['function']['name']==name+'::'+name:
   recovered=[(int(h,0),int(z,0),n,b=='true') for h,z,n,b in re.findall(r'CPacketInfo::Set\([^,]+,\s*(\w+),\s*(\w+),\s*"([^"]+)",\s*(true|false)\)',x['code'])]
   assert recovered==rows, address
   constructors+=1
  if x['function']['name']==name+'::~'+name:
   expected={'CG':'packet_info.txt','GG':'p2p_packet_info.txt','UDP':'udp_packet_info.txt'}[cls]
   code=x['code']
   assert re.findall(r'CPacketInfo::Log\([^,]+,"([^"]+)"\)',code)==[expected]
   assert code.find('CPacketInfo::~CPacketInfo')>code.find('CPacketInfo::Log')
   destructors+=1
 assert constructors==2 and destructors==3
 summary[cls]={'registrations':len(rows),'constructor_variants':constructors,'destructor_variants':destructors}
print(json.dumps(summary,indent=2))
