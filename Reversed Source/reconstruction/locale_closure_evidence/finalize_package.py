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
import json,hashlib,datetime
from pathlib import Path
p=Path('work/locale_closure');root=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'))
names=['game/src/locale_service.cpp','game/src/locale_service.h','game/src/locale.cpp','game/src/config.cpp','game/src/config.h','game/src/constants.cpp','game/src/constants.h','game/src/mob_manager.cpp','game/src/char_manager.cpp','common/singleton.h']
audit={'checked_utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),'production_files':{},'actual_dependencies':['locale_init','locale_add','locale_find','g_bAuthServer','test_server','g_iUseLocale','CMobManager::Get(const char*,bool)','singleton<CTableBySkill> template base'],'upcoming_parent_integration':['CTableBySkill complete package'],'missing_dependencies':['CBanwordManager complete class/lifecycle/CheckString','str_lower global function','all check_name methods','SJIS helpers/table','quest config globals/default init','__CheckPlayerSlot literal matrix','service init bodies','LoadEmpireTextConvertTable'],'fixture_policy':'No test fixture counted as actual application dependency.'}
for n in names:
 f=root/n;audit['production_files'][n]={'bytes':f.stat().st_size,'sha256':hashlib.sha256(f.read_bytes()).hexdigest()}
(p/'audit.json').write_text(json.dumps(audit,indent=2))
exports=[]
for f in sorted(p.glob('*-decompile.json')):
 r=json.loads(f.read_text());assert not r['truncated']
 exports.append({'address':r['function']['address'],'name':r['function']['name'],'code':r['code'],'raw_file':f.name,'raw_sha256':hashlib.sha256(f.read_bytes()).hexdigest(),'status':'evidence_available_candidate_or_dependency_unvalidated'})
(p/'addressed-evidence.json').write_text(json.dumps({'target_sha256':'61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c','functions':exports,'note':'Original30 exports preserved under work/exp_locale; no full recovery claim.'},indent=2))
(p/'RESUME.md').write_text('Bounded foundation staged. See RECOVERY_HANDOFF.md for exact complete candidates, current-production hashes, tests/gates and ordered missing dependencies. No builds/production edits. Full service initialization and TransferDefaultSetting remain pending; do not introduce partial setter.\n')
files={}
for f in sorted(p.iterdir()):
 if f.is_file() and f.name!='manifest.json':files[f.name]={'bytes':f.stat().st_size,'sha256':hashlib.sha256(f.read_bytes()).hexdigest()}
(p/'manifest.json').write_text(json.dumps({'target_sha256':'61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c','six_combat_array_bytes':824,'validation':'Static evidence/review only; no builds or original ELF execution.','files':files},indent=2))
print({'files_hashed':len(files),'new_full_exports':len(exports),'closed_candidate_functions':7,'combat_bytes':824})
