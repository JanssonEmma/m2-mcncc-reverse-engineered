from pathlib import Path
import json,hashlib,re
p=Path(__file__).parent
f=p/'stop_muyeong_regression.cpp'
s=f.read_text().replace('LPHEART thecore_heart=&testHeart;\n','')
s=s.replace('    desc_test_allow_clock();quest::CQuestManager quests;CHARACTER character;','    LPHEART previousHeart=thecore_heart;thecore_heart=&testHeart;\n    desc_test_allow_clock();quest::CQuestManager quests;CHARACTER character;')
s=s.replace('    event_destroy();','    event_destroy();\n    thecore_heart=previousHeart; // Existing DESC fixture owns the global definition.')
f.write_text(s)
f=p/'under_any_war_regression.cpp';s=f.read_text().replace('int main() {','void CheckGuildWarLookup() {');f.write_text(s)
(p/'UNDER_ANY_WAR_CONSTRUCTOR_GAP.md').write_text("""# Optimized-away constructor state
SGuildWar(BYTE) is inline only (DWARF guild.h140). Retained DeclareWar080ae600 and ReserveWar080aed80 inline contexts zero time/score/map and set type, but both overwrite state before observation. Supplied reference uses GUILD_WAR_RECV_DECLARE; that initializer is NOT called binary-proven here. No production constructor body has been added.

The parent explicitly approved TEST ONLY sguildwar_lifecycle_fixture.cpp after inspecting this gap. All five named fields are set by under_any_war_regression.cpp before actual UnderAnyWar reads them. Existing CGuild lifecycle fixture remains test-only. Tests are staged, not compiled/run.

Integration: under_any_war_regression.cpp exports void CheckGuildWarLookup(); call it from existing guild_level_regression main, add this TU and sguildwar_lifecycle_fixture.cpp to that source group. Reuse full actual guild/map graph and established boundaries; no separate synthetic war executable is required.
""")
# Refresh readable exports/dependency graph after all evidence collection.
definitions={};edges=[];asm=[]
for f in p.glob('*-decompile.json'):
 d=json.loads(f.read_text());assert not d.get('truncated')
 definitions[d['function']['address']]=d['function']['name']
 f.with_suffix('.c').write_text(d['code'])
for f in p.glob('*-calls.json'):
 d=json.loads(f.read_text())
 assert not d['callees'].get('has_more')
 for e in d['callees']['items']:edges.append({'source':d['function']['address'],**e})
for f in p.glob('*-assembly.json'):
 d=json.loads(f.read_text());assert not d['has_more'];assert len(d['items'])==d['total']
 asm.append({'file':f.name,'instructions':d['total']})
r=json.loads((p/'setpolymorph-virtual-resolution.json').read_text())
assert r['entry']['value']=='0x816d720'
(p/'dependency-graph.json').write_text(json.dumps({'saved_definitions':definitions,'edges':edges,'indirect_resolution':r},indent=2))
for f in p.glob('*.cpp'):
 assert 'static_assert(' not in f.read_text() and '<type_traits>' not in f.read_text()
assert 'LPHEART thecore_heart=' not in (p/'stop_muyeong_regression.cpp').read_text()
assert 'void CheckGuildWarLookup()' in (p/'under_any_war_regression.cpp').read_text()
out={'target_sha256':'61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c','production_changed':False,'builds_tests_or_original_execution':False,'complete_decompiles':len(definitions),'callsites':len(edges),'assembly_exports':asm,'virtual_slot13_resolved':'CHARACTER::StopRiding0816d720','SGuildWar_default_state':'unknown optimized away; explicit TEST ONLY ctor fixture','files':[]}
for f in p.iterdir():
 if f.is_file() and f.name!='manifest.json':out['files'].append({'file':f.name,'bytes':f.stat().st_size,'sha256':hashlib.sha256(f.read_bytes()).hexdigest()})
(p/'manifest.json').write_text(json.dumps(out,indent=2))
print(json.dumps({k:v for k,v in out.items() if k!='files'}))
