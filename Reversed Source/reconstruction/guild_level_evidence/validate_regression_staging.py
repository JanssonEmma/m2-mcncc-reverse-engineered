import json,re,hashlib
from pathlib import Path
p=Path('work/guild_level');s=(p/'guild_level_regression.cpp').read_text()
assert not re.search(r'(?:void|int|BYTE|DWORD|TGuildMember\*)\s+(?:CGuild|CHARACTER|DESC|CLIENT_DESC)::',s)
assert '.erase(' not in s
assert 'if(obs.mutate&&!obs.mutated)' in s and 'in==obs.member' in s
old=json.loads((p/'wire-constants-and-vectors.json').read_text())
for name,key in [('goldenDb','DB_frame23'),('goldenClient','client_packet17')]:
 body=re.search(name+r'\[\]=\{([^}]+)\}',s,re.S).group(1)
 raw=bytes(int(x.strip(),0) for x in body.split(','))
 assert raw.hex()==old[key],(name,raw.hex(),old[key])
assert 'std::memset(storage.raw,0xa5' in s and 'storage.raw[12]==0xa5' in s
f=(p/'guild_level_desc_output_syscalls.c').read_text()
assert '#include "desc_output_syscalls.c"' in f and 'abort();' in f
r=(p/'Makefile.guild-level.inc').read_text()
assert 'guild_lifecycle_fixture.cpp' in r and 'TARGET_WALKING_SOURCES' in r and 'TARGET_WALKING_OBJECTS' in r
files=['guild_level_regression.cpp','guild_level_desc_output_syscalls.c','Makefile.guild-level.inc','guild_level_abi.cpp','REGRESSION_HANDOFF.md','packet-wrapper-verification.json']
out={'validation':'Static checks only; not compiled or run; no production/status edits or builds.','golden_vectors_match_old_evidence':True,'only_mapped_fields_mutated_in_header_callback':True,'no_test_definitions_of_tested_application_methods':True,'constructor_storage_byte':'offset12 preservation checked through raw representation','files':{name:hashlib.sha256((p/name).read_bytes()).hexdigest() for name in files}}
(p/'regression-staging-validation.json').write_text(json.dumps(out,indent=2));print(json.dumps(out))
