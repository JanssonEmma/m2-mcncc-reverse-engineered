# Resolve portable paths from the repository root, regardless of current working directory.
import sys as _portable_sys
from pathlib import Path as _portable_Path
for _portable_parent in (_portable_Path(__file__).resolve().parent, *_portable_Path(__file__).resolve().parents):
    if (_portable_parent / 'project_paths.py').is_file():
        _portable_sys.path.insert(0, str(_portable_parent))
        break
else:
    raise RuntimeError('project_paths.py not found in repository ancestors')
from project_paths import resolve_path as _resolve_portable_path
from pathlib import Path
import json,hashlib,difflib,re,ast
p=Path(__file__).resolve().parent;root=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'));stage=p/'integration-ready';changes={}
def old(rel):
 q=root/rel;return q.read_bytes() if q.exists() else None
def text(rel):return old(rel).decode('utf-8-sig').replace('\r\n','\n')
def put(rel,data):
 b=data.encode() if isinstance(data,str) else data;before=old(rel);q=stage/rel;q.parent.mkdir(parents=True,exist_ok=True);q.write_bytes(b)
 sha=lambda v:hashlib.sha256(v).hexdigest()
 changes[rel]={'before_sha256':sha(before) if before is not None else None,'after_sha256':sha(b)}
 if before is not None:(p/('review-'+rel.replace('/','_')+'.diff')).write_text(''.join(difflib.unified_diff(before.decode('utf-8-sig').replace('\r\n','\n').splitlines(True),b.decode().splitlines(True),fromfile='CURRENT/'+rel,tofile='PCSTATE/'+rel)))
rel='game/src/questpc.h';s=text(rel)
replacements={
'bool IsRunning();':'bool IsRunning() { return m_RunningQuestState!=0; }',
'QuestState* GetRunningQuestState();':'QuestState* GetRunningQuestState() { return m_RunningQuestState; }',
'bool HasReward();':'bool HasReward() { return !m_vRewardData.empty() || m_bIsGivenReward; }',
'void SetSendDoneFlag();':'void SetSendDoneFlag() { m_bShouldSendDone=true; }',
'bool GetAndResetDoneFlag();':'bool GetAndResetDoneFlag() { bool previous=m_bShouldSendDone; m_bShouldSendDone=false; return previous; }',
'void SetConfirmWait(DWORD);':'void SetConfirmWait(DWORD id) { m_dwWaitConfirmFromPID=id; m_bConfirmWait=true; }',
'void ClearConfirmWait();':'void ClearConfirmWait() { m_bConfirmWait=false; }',
'bool IsConfirmWait(DWORD);':'bool IsConfirmWait(DWORD id) { return m_bConfirmWait && id==m_dwWaitConfirmFromPID; }'}
for a,b in replacements.items():assert s.count(a)==1,a;s=s.replace(a,b,1)
put(rel,s);(p/'header-inline-replacements.json').write_text(json.dumps(replacements,indent=2))
rel='game/src/questpc.cpp';s=text(rel);assert not re.search(r'PC::SetSendFlag\s*\(',s)
i=s.rfind('\n}');assert i>=0
put(rel,s[:i]+'\n// Original questpc.cpp 080603e0: mask OR, not bit-index shift.\nvoid PC::SetSendFlag(int mask) { m_iSendToClient|=mask; }\n'+s[i:])
for name in ('pc_state_helpers_regression.cpp','pc_state_helpers_abi.cpp'):
 rel='reconstruction/tests/'+name;assert old(rel) is None;put(rel,(p/name).read_bytes())
rel='reconstruction/tests/Makefile';s=text(rel);assert 'pc_state_helpers_regression' not in s
names=lambda t:next(x for x in t.splitlines() if x.startswith('RUNTIME_TESTS = ')).split('=',1)[1].split()
ns=names(s);s=s.replace('RUNTIME_TESTS = ','RUNTIME_TESTS = pc_state_helpers_regression ',1)
s=s.replace('check: $(addsuffix $(EXEEXT),$(RUNTIME_TESTS))\n','check: $(addsuffix $(EXEEXT),$(RUNTIME_TESTS))\n\t./pc_state_helpers_regression$(EXEEXT)\n',1)
s=s.replace('check-abi:\n','check-abi:\n\t$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only pc_state_helpers_abi.cpp\n',1)
rows=s.splitlines();i=next(i for i,x in enumerate(rows) if x.startswith('quest_pc_lookup_regression$(EXEEXT):'))
lines=[rows[i],rows[i+1]]
for i,line in enumerate(lines):lines[i]=line.replace('quest_pc_lookup_regression','pc_state_helpers_regression').replace('quest_pc_lookup_desc_output_syscalls.o','desc_output_syscalls.o')
assert '-DMCNCC_TEST_REAL_EVENTS' in lines[1] and '../../game/src/event.cpp' in lines[1]
# Explicit header prerequisite: inline-body edits must trigger this executable.
lines[0]+=' ../../game/src/questpc.h'
s+='\n'+'\n'.join(lines)+'\n';assert names(s)==['pc_state_helpers_regression']+ns;put(rel,s)
(p/'integration-manifest.json').write_text(json.dumps({'root':str(root),'files':changes,'runtime_before':len(ns),'runtime_after':len(ns)+1,'baseline_policy':'Rerender after parent combined89 terminal; no fixed runtime count or stale snapshots.'},indent=2))
print('Staged',len(changes),'files;',len(ns),'->',len(ns)+1,'canonical runtimes; no Fwrites/builds.')
