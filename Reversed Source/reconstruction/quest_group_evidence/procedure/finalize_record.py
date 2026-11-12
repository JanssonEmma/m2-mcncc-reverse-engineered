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
import json,hashlib,shutil,subprocess,sys
p=Path(__file__).resolve().parent;w=p.parent;root=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'));rec=root/'reconstruction';prepared=p/'prepared'
load=lambda q:json.loads(Path(q).read_text(encoding='utf-8-sig'))
h=lambda q:hashlib.sha256(Path(q).read_bytes()).hexdigest()
m=load(prepared/'RECORD_MANIFEST.json');assert m['checkpoint']==95 and m['status']=='passed' and m['full_runtime_commands']==106
assert m['runtime_scope']['Build_matching_suffix']=='static_only_indeterminate_counter_copy'
for key,name in [('baseline_overrides_sha256','function-coverage-overrides.json'),('baseline_validation_status_sha256','validation-status.json')]:assert h(rec/name)==m[key],name
for rel,want in {**m['source_guards'],**m['support_read_guards']}.items():assert h(root/rel)==want,rel
for q,want in m['proof_sha256'].items():assert h(q)==want,q
for rel,want in m['original_evidence_sha256'].items():assert h(w.parent/rel)==want,rel
for q,want in m['input_sha256'].items():assert h(q)==want,q
for rel,want in m['baseline_metadata_sha256'].items():assert h(rec/rel)==want,rel
plan=load(p/'integration/plan.json')
for source,want in plan['parent_plan_sha256'].items():assert h(source)==want,source
archive=rec/'quest_group_evidence';assert not archive.exists(),'Do not repeat finalization'
backup=p/'record-backup';backup.mkdir(exist_ok=True)
for name in ['validation-status.json','function-coverage-overrides.json','CURRENT_STATUS.md','DEPENDENCY_FRONTIER.md','RESUME_CHECKPOINT.md','function-coverage-ledger.json','function-coverage-ledger.md']:shutil.copy2(rec/name,backup/name)
archive.mkdir();packages=archive/'packages';packages.mkdir();ignore=shutil.ignore_patterns('*.exe','*.o','__pycache__')
for name in ['pc_build_cancel','pc_reward_producers']:shutil.copytree(w/name,packages/name,ignore=ignore)
shutil.copytree(p/'integration',archive/'group-integration',ignore=ignore)
for name in ['RECORD_MANIFEST.json','quest-setloaded-inline-review.json','quest-group-recovery.md','pc-quest-group-ghidra.json']:shutil.copy2(prepared/name,archive/name)
def proofdest(q):
 q=Path(q)
 return archive/('optimized-proofs/'+q.parent.name if q.parent.name in ['pc_build_cancel_validation','pc_reward_producers_validation'] else 'proofs')/q.name
proofnames={}
for q,want in m['proof_sha256'].items():
 dest=proofdest(q);assert str(dest) not in proofnames or proofnames[str(dest)]==want,'Proof basename collision';proofnames[str(dest)]=want
for q in m['proof_sha256']:
 dest=proofdest(q);dest.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(q,dest)
procedure=archive/'procedure';procedure.mkdir()
for name in ['prepare_record.py','finalize_record.py','update_docs.py','source-guards.json','OUTLINE.md']:shutil.copy2(p/name,procedure/name)
copy_index=[]
for source,want in m['proof_sha256'].items():
 dest=proofdest(source);assert h(dest)==want;copy_index.append(dict(source=source,destination=str(dest),sha256=want))
for source,want in m['original_evidence_sha256'].items():
 q=w.parent/source;dest=packages/q.relative_to(w);assert h(dest)==want;copy_index.append(dict(source=str(q),destination=str(dest),sha256=want))
for row in plan['files']:
 q=p/'integration/payload'/row['path'];dest=archive/'group-integration/payload'/row['path'];assert h(dest)==row['after'];copy_index.append(dict(source=str(q),destination=str(dest),sha256=row['after']))
(archive/'COPY_INDEX.json').write_text(json.dumps(copy_index,indent=2)+'\n',encoding='utf-8')
for name in ['validation-status.json','function-coverage-overrides.json','pc-quest-group-ghidra.json']:shutil.copy2(prepared/name,rec/name)
shutil.copy2(prepared/'quest-group-recovery.md',rec/'quest-group-recovery.md')
subprocess.run([sys.executable,str(rec/'tools/update_coverage.py'),'--inventory',str(rec/'target-function-inventory.json'),'--root',str(root)],check=True)
ledger=load(rec/'function-coverage-ledger.json');assert not ledger['warnings'] and ledger['function_count']==7238
print(json.dumps({'checkpoint':95,'evidence_available':ledger['evidence_available'],'summary':ledger['summary'],'warnings':len(ledger['warnings'])}))
