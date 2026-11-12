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
m=load(prepared/'RECORD_MANIFEST.json');assert m['checkpoint']==101 and m['status']=='passed'
for key,name in [('baseline_overrides_sha256','function-coverage-overrides.json'),('baseline_validation_status_sha256','validation-status.json')]:assert h(rec/name)==m[key],name
for rel,want in {**m['source_guards'],**m['support_read_guards']}.items():assert h(root/rel)==want,rel
for q,want in m['proof_sha256'].items():assert h(q)==want,q
for rel,want in m['original_evidence_sha256'].items():assert h(w.parent/rel)==want,rel
for q,want in m['input_sha256'].items():assert h(q)==want,q
for rel,want in m['baseline_metadata_sha256'].items():assert h(rec/rel)==want,rel
assert m['standalone_function_mappings']==8 and m['semantic_methods']==4 and m['compiler_copy_context_only']=='081ab460'
archive=rec/'quest_ownership_evidence';assert not archive.exists(),'Do not repeat finalization'
backup=p/'record-backup';backup.mkdir(exist_ok=True)
for name in ['validation-status.json','function-coverage-overrides.json','CURRENT_STATUS.md','DEPENDENCY_FRONTIER.md','RESUME_CHECKPOINT.md','function-coverage-ledger.json','function-coverage-ledger.md']:shutil.copy2(rec/name,backup/name)
archive.mkdir();shutil.copytree(w/'quest_ownership',archive/'package',ignore=shutil.ignore_patterns('*.exe','*.o','__pycache__'))
for name in ['RECORD_MANIFEST.json','quest-ownership-owner-sdk-review.json','quest-ownership-recovery.md','quest-ownership-ghidra.json']:shutil.copy2(prepared/name,archive/name)
for q in m['proof_sha256']:
 q=Path(q);folder=archive/('optimized-proofs/'+q.parent.name if q.parent.name in ['quest_ownership_validation','quest_lua_queries_validation'] else 'proofs');folder.mkdir(parents=True,exist_ok=True);shutil.copy2(q,folder/q.name)
procedure=archive/'procedure';procedure.mkdir()
for name in ['prepare_record.py','finalize_record.py','update_docs.py','source-guards.json']:shutil.copy2(p/name,procedure/name)
copy_index=[]
for source,want in m['proof_sha256'].items():
 q=Path(source);dest=archive/('optimized-proofs/'+q.parent.name if q.parent.name in ['quest_ownership_validation','quest_lua_queries_validation'] else 'proofs')/q.name;assert h(dest)==want;copy_index.append(dict(source=source,destination=str(dest),sha256=want))
for source,want in m['original_evidence_sha256'].items():
 q=w.parent/source;dest=archive/'package'/q.relative_to(w/'quest_ownership');assert h(dest)==want;copy_index.append(dict(source=str(q),destination=str(dest),sha256=want))
(archive/'COPY_INDEX.json').write_text(json.dumps(copy_index,indent=2)+'\n',encoding='utf-8')
for name in ['validation-status.json','function-coverage-overrides.json','quest-ownership-ghidra.json']:shutil.copy2(prepared/name,rec/name)
shutil.copy2(prepared/'quest-ownership-recovery.md',rec/'quest-ownership-recovery.md')
subprocess.run([sys.executable,str(rec/'tools/update_coverage.py'),'--inventory',str(rec/'target-function-inventory.json'),'--root',str(root)],check=True)
ledger=load(rec/'function-coverage-ledger.json');assert not ledger['warnings'];assert ledger['function_count']==7238
print(json.dumps({'checkpoint':101,'evidence_available':ledger['evidence_available'],'summary':ledger['summary'],'warnings':len(ledger['warnings'])}))


