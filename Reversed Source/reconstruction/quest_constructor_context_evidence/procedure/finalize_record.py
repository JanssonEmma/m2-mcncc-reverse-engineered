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
p=Path(__file__).resolve().parent;w=p.parent;root=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'));rec=root/'reconstruction';prepared=p/'prepared';load=lambda q:json.loads(Path(q).read_text(encoding='utf-8-sig'));h=lambda q:hashlib.sha256(Path(q).read_bytes()).hexdigest()
m=load(prepared/'RECORD_MANIFEST.json');assert m['checkpoint']==102 and m['status']=='passed_focused' and m['validation_scope']=='focused_only' and m['registered_runtime_commands']==116 and m['focused_runtime_commands']==4 and 'full_runtime_commands'not in m
assert m['last_full_validation']['checkpoint']==101 and m['last_full_validation']['runtime_commands']==114
for rel,want in {**m['source_guards'],**m['support_read_guards']}.items():assert h(root/rel)==want
for q,want in m['proof_sha256'].items():assert h(q)==want
for rel,want in m['original_evidence_sha256'].items():assert h(w.parent/rel)==want
for q,want in m['input_sha256'].items():assert h(q)==want
for rel,want in m['baseline_metadata_sha256'].items():assert h(rec/rel)==want
st=load(prepared/'validation-status.json');assert st['runtime_suite_count']==st['runtime_command_count_verified']==114 and st['runtime_suites']==m['last_full_validation']['suites'] and st['registered_runtime_suite_count']==116 and st['latest_validation_scope']=='focused_only'
archive=rec/'quest_constructor_context_evidence';assert not archive.exists(),'Do not repeat finalization';backup=p/'record-backup';backup.mkdir(exist_ok=True)
for name in ['validation-status.json','function-coverage-overrides.json','CURRENT_STATUS.md','DEPENDENCY_FRONTIER.md','RESUME_CHECKPOINT.md','function-coverage-ledger.json','function-coverage-ledger.md']:shutil.copy2(rec/name,backup/name)
archive.mkdir();packages=archive/'packages';packages.mkdir();ignore=shutil.ignore_patterns('*.exe','*.o','__pycache__')
for name in ['arg_script_constructor','arg_script_constructor_integration','quest_npc_context','checkpoint102_quest_constructor_context','quest_next_dependency']:shutil.copytree(w/name,packages/name,ignore=ignore)
shutil.copytree(p/'reviewed_fixes',archive/'reviewed_fixes',ignore=ignore)
for name in ['RECORD_MANIFEST.json','quest-constructor-context-ghidra.json','arg-script-inline-review.json','quest-constructor-context-recovery.md','VALIDATION_POLICY.md']:shutil.copy2(prepared/name,archive/name)
procedure=archive/'procedure';procedure.mkdir()
for name in ['prepare_record.py','finalize_record.py','update_docs.py','source-guards.json','VALIDATION_POLICY.md']:shutil.copy2(p/name,procedure/name)
copy_index=[]
for source,want in m['proof_sha256'].items():
 q=Path(source);dest=archive/'proofs'/q.parent.name/q.name;dest.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(q,dest);assert h(dest)==want;copy_index.append(dict(source=source,destination=str(dest),sha256=want))
for source,want in m['original_evidence_sha256'].items():
 q=w.parent/source;dest=packages/q.relative_to(w);assert h(dest)==want;copy_index.append(dict(source=str(q),destination=str(dest),sha256=want))
(archive/'COPY_INDEX.json').write_text(json.dumps(copy_index,indent=2)+'\n',encoding='utf-8')
for name in ['validation-status.json','function-coverage-overrides.json','quest-constructor-context-ghidra.json','arg-script-inline-review.json','quest-constructor-context-recovery.md','VALIDATION_POLICY.md']:shutil.copy2(prepared/name,rec/name)
subprocess.run([sys.executable,str(rec/'tools/update_coverage.py'),'--inventory',str(rec/'target-function-inventory.json'),'--root',str(root)],check=True)
ledger=load(rec/'function-coverage-ledger.json');assert ledger['function_count']==7238 and not ledger['warnings'];print(json.dumps({'checkpoint':102,'validation_scope':'focused_only','registered':116,'focused':4,'last_full':'101/114','evidence':ledger['evidence_available'],'summary':ledger['summary']}))
