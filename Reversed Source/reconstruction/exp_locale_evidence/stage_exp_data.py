import json,hashlib,struct
from pathlib import Path
p=Path('work/exp_locale');d=json.loads((p/'data.json').read_text());names=['exp_table_euckr','exp_table_gb2312','exp_table_sjis','exp_table_hk','exp_table_newcibn']
rows={r['name']:r for r in d['symbols']}
decl=['// Insert in game/src/constants.h before final guard. Exact old100-DWORD symbols.']
defs=['// Append to game/src/constants.cpp after including constants.h declarations.','// Mutable pointer initiallyNULL (.bss); selection is intentionally not implemented here.','const DWORD* exp_table=0;','']
gold=['// Original ELF bytes, independent of C++ DWORD initializer formatting.','#ifndef MCNCC_EXP_DATA_GOLDEN_H','#define MCNCC_EXP_DATA_GOLDEN_H','static const unsigned char exp_data_golden[5][400] = {']
for name in names:
 r=rows[name];raw=bytes.fromhex(r['raw_hex']);assert r['section']=='.rodata' and r['size']==400 and len(r['u32'])==100
 assert struct.pack('<100I',*r['u32'])==raw and hashlib.sha256(raw).hexdigest()==r['sha256']
 decl.append('extern const DWORD '+name+'[100];')
 defs.append('// ELF '+r['address']+' .rodata,400 bytes, SHA-256 '+r['sha256'])
 defs.append('const DWORD '+name+'[100] = {')
 for i in range(0,100,10):defs.append('    '+', '.join(str(v)+'UL' for v in r['u32'][i:i+10])+',')
 defs.extend(['};',''])
 gold.append('    { // '+name)
 for i in range(0,400,20):gold.append('        '+', '.join('0x%02x'%v for v in raw[i:i+20])+',')
 gold.append('    },')
decl.append('extern const DWORD* exp_table; // pointer mutable, pointee const; original .bss initial0')
gold.extend(['};','#endif',''])
assert rows['exp_table']['section']=='.bss' and rows['exp_table']['u32']==[0]
(p/'constants-exp-declarations.inc').write_text('\n'.join(decl)+'\n')
(p/'constants-exp-definitions.inc').write_text('\n'.join(defs)+'\n')
(p/'exp_data_golden.h').write_text('\n'.join(gold))
api=json.loads((p/'api-metadata.json').read_text())
assert any(r['name']=='GetNextExp' and r['linkage']=='_ZN9CHARACTER10GetNextExpEv' and r['return']['name']=='DWORD' for r in api['methods'])
(p/'char-get-next-exp.inc').write_text('// Add #include "constants.h" outside the class in char.h.\n// Original inline char.h584; nonconstDWORD GetNextExp(), no extra NULL guard.\nDWORD GetNextExp() {\n    if(m_points.level>=99) return 2000000000UL;\n    return exp_table[m_points.level];\n}\n')
print(json.dumps({'arrays':names,'all_raw_bytes_verified':True,'pointer_initial':'NULL','GetNextExp':'DWORD, nonconst, char.h584'}))
