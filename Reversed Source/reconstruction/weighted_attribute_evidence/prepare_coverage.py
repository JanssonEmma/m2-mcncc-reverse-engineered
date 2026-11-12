from pathlib import Path
import json,hashlib
p=Path(__file__).resolve().parent;sha='61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c'
digest=lambda q:hashlib.sha256(q.read_bytes()).hexdigest()
def ref(rel,anchor):
 f=p/'integration-ready'/rel;t=f.read_text();assert anchor in t,(rel,anchor)
 return {'path':rel,'anchor':anchor,'line':t[:t.index(anchor)].count('\n')+1,'staged_file_sha256':digest(f),'status':'pending_parent_integration_and_validation'}
spec=[
 ('08186ec0','game/src/item_attribute.cpp','void CItem::AddAttr(','static void AddAttrCases(','Complete AddAttr: proto/normal duplicate, bytelevel0, full5, real map zero-insertion, clamp5, long-to-short before real SetAttribute; zero-before-narrow quirk/rare exclusion and actual packet/save/SQL.'),
 ('08187060','game/src/item_attribute.cpp','void CItem::PutAttributeWithLevel(','static void WeightedCases(','All8sets/weighted boundaries, loop82 operator[] default rows, zero level/empty/zero-weight draws, signed32 wrapped totals, apply0 failure sentinel, live weight/cap reread via external RNG mutation.'),
 ('081874d0','game/src/item_attribute.cpp','void CItem::PutAttribute(int*','static void LevelCases(','All100 level percentages for both exact5-entry tables, nonconst API but read-only inputs, undersum->level6 guard, signed32 subtraction wrap.'),
 ('081681e0','game/src/item.cpp','void CItem::AlterToMagicItem()','static void MagicCases(','Complete locale/type/subtype thresholds, independent2nd/3rd boundary draws, real map/RNG/SetAttribute chain, cached thresholds across SQL callback locale mutation, existing/full normal slots and invalid set early return.')]
exports=[];reviews=[]
for addr,source,anchor,ta,scope in spec:
 f=p/(addr+'.json');x=json.loads(f.read_text())['d']['structuredContent'];assert x['function']['address']==addr and not x.get('truncated')
 exports.append(dict(x,requested_address=addr,provenance={'saved_export':str(f),'sha256':digest(f)}))
 reviews.append({'address':addr,'source_refs':[ref(source,anchor)],'test_refs':[ref('reconstruction/tests/weighted_attribute_regression.cpp',ta)],'review_scope':scope,'reviewer':'pending parent complete focused/ABI/O2/strict/full receipt','limitations':'Workspace-only candidates/tests not compiled/run. Full real map value records and STL lifecycle, typed synthetic row fixtures only; no fake Boot/data loader. Full original CInputDB::Boot retained as evidence but not implemented/narrowed. Prototype required by HasAttr, allocation failure and invalid raw pointers not dynamically forced. No original ELF execution or full factory/quest/stat/server equivalence claim.'})
(p/'weighted-addressed-ghidra.json').write_text(json.dumps({'target_sha256':sha,'origin':'Four complete addressed original application methods. Private AttrAdd32/AttrSub32 are translation helpers preserving x86 wrap, not separately recovered originals. Compiler/STL map lifecycle retained separately as evidence.','functions':exports},indent=2))
(p/'weighted-coverage-mappings.pending.json').write_text(json.dumps({'target_sha256':sha,'status':'pending_parent_integration_and_validation','reviews':reviews},indent=2))
(p/'weighted-symbol-manifest.json').write_text(json.dumps({'target_sha256':sha,'functions':json.loads((p/'method-symbols.json').read_text()),'data':json.loads((p/'globals-tables.json').read_text()),'complete_record':'TItemAttrTable69, fields char33/DWORD/DWORD/long[5]/BYTE[8], packed offsets0/33/37/41/61; real map<DWORD,TItemAttrTable> normal+rare, no oldSTLsizeof assertion.','data_refs':[ref('common/tables.h','typedef struct SItemAttrTable'),ref('game/src/constants.cpp','TItemAttrMap g_map_itemAttr;'),ref('game/src/constants.cpp','TItemAttrMap g_map_itemRare;'),ref('game/src/constants.cpp','const int aiItemMagicAttributePercentHigh[5]'),ref('game/src/constants.cpp','const int aiItemMagicAttributePercentLow[5]')]},indent=2))
print('Four normalized original exports and explicit pending source/test mappings saved.')
