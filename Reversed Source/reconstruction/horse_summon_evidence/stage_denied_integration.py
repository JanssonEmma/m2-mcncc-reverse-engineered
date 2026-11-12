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
import shutil,json,difflib,hashlib
p=Path(__file__).parent
root=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'))
ready=p/'integration-ready';ready.mkdir(exist_ok=True)
oldwrite=Path.write_text;oldcopy=shutil.copyfile
def route(dst):
 path=Path(dst)
 try:rel=path.relative_to(root)
 except ValueError:return path
 out=ready/rel;out.parent.mkdir(parents=True,exist_ok=True);return out
def safe_write(self,*args,**kwargs):return oldwrite(route(self),*args,**kwargs)
def safe_copy(source,dest,*args,**kwargs):return oldcopy(source,route(dest),*args,**kwargs)
Path.write_text=safe_write;shutil.copyfile=safe_copy
s=p.joinpath('integrate_closed.py').read_text()
s=s.replace("after={rel:hashlib.sha256((root/rel).read_bytes()).hexdigest() for rel in files}","after={rel:hashlib.sha256((ready/rel).read_bytes()).hexdigest() for rel in files}")
s=s.replace("'step':'integrated_not_built'","'step':'staged_only_first_production_write_permission_denied'")
s=s.replace("'changed_files':files","'planned_files':files")
s=s.replace('Integrated CLOSED horse methods only','STAGED ONLY: first production write denied; no production file changed. Apply integration-ready/ files or pending-integration.patch. CLOSED horse methods only')
exec(compile(s,str(p/'integrate_closed.py'),'exec'),{'__name__':'__main__','__file__':str(p/'integrate_closed.py'),'ready':ready})
# Use exact already-known project ABI flags rather than an independently spelled flag.
make=ready/'reconstruction/tests/Makefile'
text=make.read_text();baseline=(root/'reconstruction/tests/Makefile').read_text()
known=next(l for l in baseline.splitlines() if 'guild_level_abi.cpp' in l)
lines=text.splitlines(True)
for i,line in enumerate(lines):
 if 'horse_methods_abi.cpp' in line and line.startswith('\t$(CXX)'):
  lines[i]=known.replace('guild_level_abi.cpp','horse_methods_abi.cpp')+'\n'
make.write_text(''.join(lines))
patch=[]
for f in sorted(ready.rglob('*')):
 if not f.is_file():continue
 rel=f.relative_to(ready).as_posix();original=root/rel
 old=original.read_text().splitlines(True) if original.exists() else []
 patch.extend(difflib.unified_diff(old,f.read_text().splitlines(True),fromfile='a/'+rel if old else '/dev/null',tofile='b/'+rel))
p.joinpath('pending-integration.patch').write_text(''.join(patch))
status=json.loads(p.joinpath('INTEGRATION_STATUS.json').read_text())
status.update(production_write_denied=True,production_files_changed=False,validation_run=False,patch=str(p/'pending-integration.patch'),ready_root=str(ready))
status['after']={str(f.relative_to(ready)):hashlib.sha256(f.read_bytes()).hexdigest() for f in ready.rglob('*') if f.is_file()}
p.joinpath('INTEGRATION_STATUS.json').write_text(json.dumps(status,indent=2))
p.joinpath('INTEGRATION_RESUME.md').write_text("""# Integration blocked by filesystem
FIRST production write failed with PermissionError13 at game/src/char.cpp. No production/header/Makefile/globalfixture file was changed.
Exact additive integration is saved in pending-integration.patch and complete integration-ready/ files. Workspace pre-change backups in integration-backup/. Preserve original source placement; broad HorseSummon/Spawn/Show/Goto candidates are NOT included. Global character/desc/entity fixtures unchanged, including thecore_shutdown boundary.

Parent must apply the prepared patch/files using its already-authorized access. Then run existing validation tool focused only, with known compilers:
USER_HOME\\.cache\\codex-runtimes\\codex-primary-runtime\\dependencies\\python\\python.exe PROJECT_ROOT\\Reversed Source\\reconstruction\\tools\\validate.py run --suite horse_methods_regression --abi --make C:\\Strawberry\\c\\bin\\mingw32-make.exe --cc C:\\Strawberry\\c\\bin\\gcc.exe --cxx C:\\Strawberry\\c\\bin\\g++.exe --log [workspace]/horse-focused.log
No fullsuite requested. Inspect receipt/session/log after each step. Stop after focused pass or95%quota/repeatedfailure.
No focused build/ABI/runtime validation happened because production integration could not occur.
""")
print(json.dumps({'step':'staged_permission_denied','production_changes':False,'ready_files':len(status['after']),'patch_bytes':p.joinpath('pending-integration.patch').stat().st_size,'validation_run':False}))
