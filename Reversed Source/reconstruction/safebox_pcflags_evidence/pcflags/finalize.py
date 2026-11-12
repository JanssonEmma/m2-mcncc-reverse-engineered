from pathlib import Path
import json,hashlib,ast
p=Path(__file__).resolve().parent
base=p.parents[1]/'quest_pc_lookup_o2.py'
# Existing helper is at workspace work/, not quest_level/.
base=p.parents[1]/'quest_pc_lookup_o2.py'
s=base.read_text().replace('quest_pc_lookup','pc_flags_timers').replace('quest-pc-lookup','pc-flags-timers').replace('PC lifecycle/lookup','PC flags/timers')
# The helper lives in this package, so logs/receipt remain beside the handoff.
(p/'run_optimized.py').write_text(s)
for q in [p/'prepare_integration.py',p/'apply_integration.py',p/'run_optimized.py']:ast.parse(q.read_text())
methods=[('0805de00','SetFlag'),('0805dfe0','DeleteFlag'),('0805e0b0','GetFlag'),('0805e180','SaveFlag'),('0805ec00','AddTimer'),('0805ed20','RemoveTimerNotCancel'),('0805ee10','RemoveTimer')]
sha=lambda q:hashlib.sha256(q.read_bytes()).hexdigest()
exports=[]
for address,name in methods:
 evidence=p/(address+'.json');json.loads(evidence.read_text())
 exports.append({'address':address,'name':'quest::PC::'+name,'original_file':'questpc.cpp','candidate':'questpc.cpp.fragment','candidate_anchor':'PC::'+name+'(','source':'game/src/questpc.cpp','source_anchor':'PC::'+name+'(','test':'reconstruction/tests/pc_flags_timers_regression.cpp','test_group':'Flags' if 'Flag' in name else 'TimerCases','evidence':evidence.name,'evidence_sha256':sha(evidence),'semantic_original_count':1})
(p/'normalized-originals.json').write_text(json.dumps({'elf_sha256':'61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c','exports':exports,'exclusions':['No constructor/destructor variants counted','No Lua VM lifecycle recovery','No database persistence: SaveFlag only changes pending map'],'candidate_sha256':sha(p/'questpc.cpp.fragment')},indent=2))
print('Seven original mappings and unexecuted optimized helper saved; Python syntax checked.')
