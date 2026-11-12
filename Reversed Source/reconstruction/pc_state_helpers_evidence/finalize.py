from pathlib import Path
import json,hashlib,ast
p=Path(__file__).resolve().parent
p.joinpath('questpc.cpp.fragment').write_text('// Original questpc.cpp standalone export 080603e0.\nvoid PC::SetSendFlag(int mask) { m_iSendToClient|=mask; }\n')
a=json.loads((p/'inline-dwarf.json').read_text());u={}
for x in a:
 k=x['attrs']['DW_AT_MIPS_linkage_name']
 if x['attrs']['DW_AT_low_pc']:u.setdefault(k,[]).append(x)
sha=lambda q:hashlib.sha256(q.read_bytes()).hexdigest()
exports=[]
for k,v in u.items():
 x=v[0];n=x['attrs']['DW_AT_name'];stand=n=='SetSendFlag';adr=x['attrs']['DW_AT_low_pc']
 exports.append({'name':'quest::PC::'+n+('(DWORD)' if n=='IsConfirmWait' else ''),'linkage':k,'kind':'standalone original function export' if stand else 'original inline semantic body; not a standalone export','original_address':format(adr,'08x') if stand else None,'inline_ranges':[] if stand else [{'low_pc':format(z['attrs']['DW_AT_low_pc'],'08x'),'high_pc_exclusive':format(z['attrs']['DW_AT_high_pc'],'08x'),'die':z['die'],'cu':z['cu']} for z in v],'source':'game/src/questpc.cpp' if stand else 'game/src/questpc.h','source_anchor':'PC::SetSendFlag' if stand else n+'(','test':'reconstruction/tests/pc_state_helpers_regression.cpp','evidence':'080603e0.json' if stand else format(adr,'08x')+'-parent-asm.json'})
# The first GetRunningQuestState instance is cmd_gm.cpp; all chosen evidence files exist.
for x in exports:assert (p/x['evidence']).exists(),x
(p/'normalized-originals.json').write_text(json.dumps({'elf_sha256':'61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c','standalone_original_exports':1,'inline_semantic_bodies':8,'entries':exports,'excluded_reference_only':['PC::IsConfirmWait()','PC::ClearSendFlag()'],'evidence_sha256':{q.name:sha(q) for q in p.glob('*-parent-asm.json')}},indent=2))
# Derive parent-executed optimized graph from the proved PC lookup helper.
s=(p.parents[1]/'quest_pc_lookup_o2.py').read_text().replace('quest_pc_lookup','pc_state_helpers').replace('quest-pc-lookup','pc-state-helpers').replace('PC lifecycle/lookup','PC state helpers')
s=s.replace('pc_state_helpers_desc_output_syscalls.c','desc_output_syscalls.c')
(p/'run_optimized.py').write_text(s)
for name in ('prepare_integration.py','apply_integration.py','run_optimized.py'):ast.parse((p/name).read_text())
# Compact original instruction slices: surrounding branches retained where code was scheduled outside the DWARF inline range.
proof=[]
for x in exports:
 if x['original_address']:continue
 adr=int(x['inline_ranges'][0]['low_pc'],16);d=json.loads((p/x['evidence']).read_text());d=d.get('structuredContent') or json.loads(d['content'][0]['text'])
 proof.append({'name':x['name'],'ranges':x['inline_ranges'],'instructions':[i for i in d['items'] if adr-8<=int(i['address'],16)<adr+64]})
(p/'reviewed-inline-instructions.json').write_text(json.dumps(proof,indent=2))
print('Nine body mappings: ONE original standalone address, eight inline bodies. Python syntax checked; no builds.')
