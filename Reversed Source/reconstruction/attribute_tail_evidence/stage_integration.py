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
import json,hashlib,difflib,argparse
p=Path(__file__).resolve().parent
ap=argparse.ArgumentParser();ap.add_argument('--root',default=_resolve_portable_path('PROJECT_ROOT\\Reversed Source'));a=ap.parse_args();root=Path(a.root);out=p/'integration-ready'
h=lambda q:hashlib.sha256(q.read_bytes()).hexdigest()
files={}
def stage(rel,s):
 q=out/rel;q.parent.mkdir(parents=True,exist_ok=True);q.write_text(s,encoding='utf-8')
 old=root/rel;files[rel]={'before_sha256':h(old) if old.exists() else None,'after_sha256':h(q)}
 if old.exists():
  (p/('review-'+rel.replace('/','_')+'.diff')).write_text(''.join(difflib.unified_diff(old.read_text(encoding='utf-8').splitlines(True),s.splitlines(True),fromfile=rel,tofile='staged/'+rel)),encoding='utf-8')
rel='game/src/item.h';s=(root/rel).read_text(encoding='utf-8');assert 'void ChangeAttribute(' not in s
assert s.count('    void AlterToMagicItem();')==1
stage(rel,s.replace('    void AlterToMagicItem();','    void AlterToMagicItem();\n'+(p/'item.h.fragment').read_text(encoding='utf-8').rstrip()))
rel='game/src/item_attribute.cpp';s=(root/rel).read_text(encoding='utf-8');assert 'void CItem::ChangeAttribute(' not in s
assert 'bool CItem::RemoveAttributeType(BYTE bType)' in s and 'RemoveAttributeType(static_cast<BYTE>(index))' in s
stage(rel,s+'\n#include "../../libthecore/include/memcpy.h"\n'+(p/'item_attribute.cpp.fragment').read_text(encoding='utf-8'))
for name in ['attribute_tail_regression.cpp','attribute_tail_abi.cpp','attribute_tail_desc_output_syscalls.c']:stage('reconstruction/tests/'+name,(p/name).read_text(encoding='utf-8'))
rel='reconstruction/tests/Makefile';s=(root/rel).read_text(encoding='utf-8');assert 'attribute_tail_regression' not in s
lines=s.splitlines(True);runtime=next(i for i,x in enumerate(lines) if x.startswith('RUNTIME_TESTS = '));before=lines[runtime].split('=',1)[1].split();lines[runtime]=lines[runtime].replace('RUNTIME_TESTS = ','RUNTIME_TESTS = attribute_tail_regression ',1);s=''.join(lines)
s=s.replace('\t./weighted_attribute_regression$(EXEEXT)','\t./attribute_tail_regression$(EXEEXT)\n\t./weighted_attribute_regression$(EXEEXT)',1)
s=s.replace('\t$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only weighted_attribute_abi.cpp','\t$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only attribute_tail_abi.cpp\n\t$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only weighted_attribute_abi.cpp',1)
s=s.replace('\t$(RM) weighted_desc_output_syscalls.o','\t$(RM) attribute_tail_desc_output_syscalls.o\n\t$(RM) weighted_desc_output_syscalls.o',1)
start=s.index('weighted_attribute_regression$(EXEEXT):');snippet=s[start:]
assert len(snippet.splitlines())==5 or snippet.count('weighted_attribute_regression$(EXEEXT):')==1
# Existing weighted tail recipes are last in current Makefile; select exact rule+object block, not future additions.
raw=s[start:].splitlines(True);n=next(i for i,x in enumerate(raw) if x.startswith('weighted_desc_output_syscalls.o:'))
recipe=''.join(raw[:n+2]);assert '$(CC)' in raw[n+1]
recipe=recipe.replace('weighted_attribute_regression','attribute_tail_regression').replace('weighted_desc_output_syscalls','attribute_tail_desc_output_syscalls')
s+='\n'+recipe
assert len(before)==len(set(before));stage(rel,s)
manifest={'target_sha256':'61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c','files':files,'baseline_runtime_count':len(before),'expected_runtime_count':len(before)+1,'status':'staged_not_compiled_or_run','preservation':'Only additive original10 methods/API/one canonical runtime+ABI+observer rule; existing production fields/API/bodies/old standalone LTO/weighted locale include untouched.'}
(p/'manifest.json').write_text(json.dumps(manifest,indent=2)+'\n',encoding='utf-8');print(json.dumps({'staged_files':len(files),'baseline':len(before),'expected':len(before)+1}))
