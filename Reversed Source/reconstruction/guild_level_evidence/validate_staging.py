import json,re,hashlib
from pathlib import Path
p=Path('work/guild_level');m=json.loads((p/'old-types-signatures.json').read_text());src=(p/'guild.cpp').read_text()
for linkage,name,return_name in [('_ZN9CHARACTER8GetLevelEv','GetLevel','int'),('_ZN9CHARACTER11GetPlayerIDEv','GetPlayerID','DWORD')]:
 rows=[r for r in m['methods'] if r['linkage']==linkage]
 assert rows and all(r['return']['name']==return_name for r in rows)
 assert any(r['unit']=='guild.cpp' for r in rows)
for file,tag,fields,size in [('tables-guild-member-change.inc','SPacketGuildChangeMemberData',['guild_id','pid','offer','level','grade'],14),('packet-guild-header.inc','packet_guild',['header','size','subheader'],4)]:
 text=(p/file).read_text();body=re.search(r'struct\s+'+tag+r'\s*\{(.*?)\}',text,re.S).group(1)
 actual=re.findall(r'(?:DWORD|BYTE|WORD)\s+(\w+)\s*;',body);assert actual==fields
 rec=next(r for r in m['records'] if r['unit']=='guild.cpp' and r['name']==tag);assert rec['size']==size and [c['name'] for c in rec['members']]==fields
constructors=src.split('//0808cf20')[0]
assert '_dummy(' not in constructors and '->_dummy' not in constructors
assert src.index('db_clientdesc->DBPacket')<src.index('member->second._dummy=0')
assert 'descriptor->BufferedPacket(&packet,sizeof(packet));' in src and 'descriptor->Packet(&member->second,13);' in src
assert 'member->second.level==level' not in src
result={'status':'Static candidate/evidence checks passed. No build, test, original execution or production edits.','GetLevel':'int, nonconst','GetPlayerID':'DWORD, nonconst; replace preexisting const getter','packets':{'DB_payload':14,'client_header':4,'member_prefix':13,'client_total':17},'constructors':'Both original overloads preserve uninitialized _dummy','ordering':'DBPacket before _dummy reset; BufferedPacket4 then live member prefix13; online advance after sends','source_sha256':hashlib.sha256(src.encode()).hexdigest()}
(p/'staging-validation.json').write_text(json.dumps(result,indent=2));print(json.dumps(result))
