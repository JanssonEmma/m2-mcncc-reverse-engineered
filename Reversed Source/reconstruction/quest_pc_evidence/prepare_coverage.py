from pathlib import Path
import json,hashlib
p=Path(__file__).resolve().parent;stage=p/'integration-ready';sha='61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c'
digest=lambda q:hashlib.sha256(q.read_bytes()).hexdigest()
def ref(rel,anchor):
 f=stage/rel;t=f.read_text();assert anchor in t,(rel,anchor)
 return {'path':rel,'anchor':anchor,'line':t[:t.index(anchor)].count('\n')+1,'staged_file_sha256':digest(f)}
spec=[
('0805d580','game/src/questpc.cpp','PC::PC():','static void Lifecycle()','PC base ctor, same full initializer, LastState indeterminate.'),
('0805d790','game/src/questpc.cpp','PC::PC():','static void Lifecycle()','PC complete ctor; full native containers/known scalar defaults.'),
('0805d900','game/src/questpc.cpp','PC::~PC()','static void DestructorOwnedTimer()','Base dtor variant, actual populated timer ownership cleanup.'),
('0805db30','game/src/questpc.cpp','PC::~PC()','static void DestructorOwnedTimer()','Complete dtor and whole member teardown, actual populated timer cleanup.'),
('0805dd60','game/src/questpc.cpp','void PC::Destroy()','static void Lifecycle()','Timer-only Destroy preserves other PC data, repeat empty cleanup.'),
('0805dd70','game/src/questpc.cpp','void PC::SetID(','static void Lifecycle()','Done reset and DWORD ID store, no loaded/context reset.'),
('0805dd90','game/src/questpc.cpp','const std::string& PC::GetCurrentQuestName()','static void Lifecycle()','Old nonconst method returning actual const string reference identity.'),
('0805ef10','game/src/questpc.cpp','void PC::ClearTimer()','static void Lifecycle()','Complete lexical/embeddedNUL map cancel, actual registry/free/queue semantics.'),
('081ad980','game/src/questevent.cpp','void CancelTimerEvent(','static void Processing()','Old numeric registry lookup, name-null before actual free, queued/processing/null-name/missing-info branch inputs.'),
('081a4d40','game/src/questmanager.cpp','PC* CQuestManager::GetPCForce(','static void Lookup(','Complete real PC value insertion/lifetime, uint32 keys/identity and SetID only new key.'),
('081a4e50','game/src/questmanager.cpp','PC* CQuestManager::GetPC(','static void Lookup(','Actual PID map/context success and absence/null failure preservation.'),
('0819f0f0','game/src/questmanager.cpp','CQuestManager::CQuestManager():L(0)','int main()','Manager base ctor full STL/singleton; onlyL scalar initialized.'),
('0819f5e0','game/src/questmanager.cpp','CQuestManager::CQuestManager():L(0)','int main()','Manager complete ctor same full initializer, no Lua created.')]
exports=[];reviews=[]
for a,rel,anchor,ta,scope in spec:
 f=p/(a+'.json');x=json.loads(f.read_text())['d']['structuredContent'];assert x['function']['address']==a and not x.get('truncated')
 exports.append(dict(x,requested_address=a,provenance={'saved_export':str(f),'sha256':digest(f)}))
 reviews.append({'address':a,'source_refs':[ref(rel,anchor)],'test_refs':[ref('reconstruction/tests/quest_pc_lookup_regression.cpp',ta)],'review_scope':scope,'reviewer':'pending parent focused/ABI/O2/strict32/full96','limitations':'13 distinct variants represent10 semantic source methods, not13 features. No allocation-failure/unwind proof or old STL layout/copy-count equivalence. PC LastState remains indeterminate and original implicit map copy may read it; avoiding explicit test reads does not establish definedness/portability. Never zero/deepcopy production. Manager Lua-owning destructor/NPC execution remain honest TEST boundaries. No original ELF/full leveling equivalence.'})
copy=json.loads((p/'081ab2b0.json').read_text())['d']['structuredContent']
(p/'pc-addressed-ghidra.json').write_text(json.dumps({'target_sha256':sha,'origin':'13 unique addressed variants/10 semantic methods. Inline IsLoaded no invented address; implicit copy compiler evidence only.','functions':exports,'compiler_copy_evidence':copy},indent=2))
(p/'pc-coverage-mappings.pending.json').write_text(json.dumps({'target_sha256':sha,'status':'pending_parent_validation','reviews':reviews},indent=2))
print('13 addressed associations/10 semantic methods saved; no ledger writes.')
