import json,struct,hashlib
from pathlib import Path
p=Path('work/locale_closure');d=json.loads((p/'data-metadata.json').read_text())
arrays=[r for r in d['symbols'] if r['section']=='.rodata']
defs=['// Append to original constants.cpp after constants.h declarations.']
decl=['// Append to original constants.h. Original symbol sizes31/41 signed32 ints.']
gold=['#ifndef MCNCC_LOCALE_COMBAT_GOLDEN_H','#define MCNCC_LOCALE_COMBAT_GOLDEN_H']
for r in sorted(arrays,key=lambda x:x['name']):
 n=r['name'];vals=r['i32'];raw=bytes.fromhex(r['raw_hex'])
 assert struct.pack('<'+'i'*len(vals),*vals)==raw and len(vals) in (31,41)
 decl.append('extern const int '+n+'['+str(len(vals))+'];')
 defs.extend(['// '+r['address']+' SHA256 '+r['sha256'],'const int '+n+'['+str(len(vals))+'] = {'])
 for i in range(0,len(vals),10):defs.append('    '+', '.join(map(str,vals[i:i+10]))+',')
 defs.append('};')
 gold.append('static const unsigned char golden_'+n+'['+str(len(raw))+'] = {')
 for i in range(0,len(raw),20):gold.append('    '+', '.join('0x%02x'%x for x in raw[i:i+20])+',')
 gold.append('};')
for n in ('aiPercentByDeltaLev','aiPercentByDeltaLevForBoss','aiChainLightningCountBySkillLevel'):
 r=next(r for r in d['symbols'] if r['name']==n)
 assert r['section']=='.bss' and r['raw_hex']=='00000000'
 decl.append('extern const int* '+n+';')
 defs.append('const int* '+n+'=0;')
gold.append('#endif')
for name,lines in [('constants-combat-declarations.inc',decl),('constants-combat-definitions.inc',defs),('locale_combat_golden.h',gold)]:
 (p/name).write_text('\n'.join(lines)+'\n')
print({'arrays':len(arrays),'table_bytes':sum(r['size'] for r in arrays),'pointers':3})
