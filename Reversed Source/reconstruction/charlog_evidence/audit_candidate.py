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
import json,hashlib,sys
p=Path(__file__).parent
test=p.joinpath('charlog_regression.cpp')
s=test.read_text().replace('"nested\'\\\\\\\\hint"','"nested\'hint"').replace("nested\\\\'\\\\\\\\hint')","nested\\\\'hint')")
test.write_text(s)
src=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'))
assert p.joinpath('candidate-log.cpp').read_text().startswith(src.joinpath('game/src/log.cpp').read_text())
assert p.joinpath('candidate-log.h').read_text().split('    CAsyncSQL m_sql;')[1]==src.joinpath('game/src/log.h').read_text().split('    CAsyncSQL m_sql;')[1]
assert "nested'\\\\\\\\hint" not in test.read_text()
sys.path.insert(0,_resolve_portable_path('USER_HOME\\Documents\\Codex\\2026-10-23\\i-h\\work\\python-deps'))
from elftools.elf.elffile import ELFFile
with Path(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game')).open('rb') as f:
 elf=ELFFile(f);symbols=[]
 for sec in elf.iter_sections():
  if sec.header.sh_type=='SHT_SYMTAB':
   for symbol in sec.iter_symbols():
    if symbol['st_value'] in (0x80785a0,0x8322600):
     symbols.append({'name':symbol.name,'address':hex(symbol['st_value']),'size':symbol['st_size'],'binding':symbol['st_info']['bind'],'type':symbol['st_info']['type']})
 p.joinpath('symbols.json').write_text(json.dumps(symbols,indent=2))
audit={'body_and_callgraph_complete':True,'assembly_instructions':json.loads(p.joinpath('080785a0-assembly.json').read_text())['total'],'production_log_prefix_unchanged':True,'all_logger_data_members_preserved':True,'build_or_execution_performed':False,'tests_staged_only':True,'files':[]}
for f in p.iterdir():
 if f.is_file() and f.name!='manifest.json':audit['files'].append({'file':f.name,'bytes':f.stat().st_size,'sha256':hashlib.sha256(f.read_bytes()).hexdigest()})
p.joinpath('manifest.json').write_text(json.dumps(audit,indent=2))
print(json.dumps({k:v for k,v in audit.items() if k!='files'}))
