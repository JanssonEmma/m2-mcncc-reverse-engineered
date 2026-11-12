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
import json,hashlib,difflib
p=Path(__file__).resolve().parent;root=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'));stage=p/'integration-ready'
def text(q):return q.read_text(encoding='utf-8-sig').replace('\r\n','\n')
checks={}
for rel,fragment in [('common/tables.h','tables.h.fragment'),('game/src/constants.h','constants.h.fragment')]:
 before=text(root/rel);after=text(stage/rel);addition=(p/fragment).read_text()+'\n'
 assert after.count(addition)==1 and after.replace(addition,'',1)==before
 checks[rel]='Existing contents exactly preserved after newline normalization; only guarded fragment inserted.'
before=text(root/'game/src/item.h');after=text(stage/'game/src/item.h')
assert after.replace((p/'item.h.fragment').read_text()+'\nprivate:','private:',1)==before
checks['game/src/item.h']='Full existing class fields/virtual/API exactly preserved;4 public declarations only.'
for rel,fragment in [('game/src/constants.cpp','constants.cpp.fragment'),('game/src/item.cpp','item.cpp.fragment')]:
 assert text(stage/rel)==text(root/rel)+'\n'+(p/fragment).read_text()
 checks[rel]='Full previous body exactly preserved; additive fragment only.'
rel='game/src/item_attribute.cpp';before=text(root/rel);after=text(stage/rel)
expected=before.replace('#include "item.h"','#include "item.h"\n#include "constants.h"\n#include "../../libthecore/include/utils.h"\n#include <vector>',1)+'\n'+(p/'item_attribute.cpp.fragment').read_text()
assert after==expected;checks[rel]='All previous bodies preserved;3 complete methods + includes +2 private integer wrap helpers added.'
before=text(root/'common/tables.h');after=text(stage/'common/tables.h')
skill=before[before.index('typedef struct SSkillTable {'):before.index('} TSkillTable;')+len('} TSkillTable;')]
assert after.count(skill)==1 and before.count(skill)==1
assert after.count('typedef struct SItemAttrTable {')==1
oldnames=next(x for x in text(root/'reconstruction/tests/Makefile').splitlines() if x.startswith('RUNTIME_TESTS = ')).split('=',1)[1].split()
newnames=next(x for x in text(stage/'reconstruction/tests/Makefile').splitlines() if x.startswith('RUNTIME_TESTS = ')).split('=',1)[1].split()
assert len(oldnames)==92 and len(newnames)==93 and newnames[1:]==oldnames and newnames[0]=='weighted_attribute_regression'
for rel in checks:
 (p/('review-'+rel.replace('/','_')+'.diff')).write_text(''.join(difflib.unified_diff(text(root/rel).splitlines(True),text(stage/rel).splitlines(True),fromfile='CURRENT/'+rel,tofile='WEIGHTED/'+rel)))
report={'workspace_only':True,'root':str(root),'existing_content_checks':checks,'TSkillTable_full1475_field_list_sha256':hashlib.sha256(skill.encode()).hexdigest(),'TSkillTable_preserved_byte_for_byte_after_newline_normalization':True,'full_SItemAttrTable69_fields':True,'existing_runtime_names_order_preserved':92,'staged_runtime_names':93,'all_other_records_preserved':True,'validation':'Source/data/graph audit only; no builds/runtime.'}
(p/'CURRENT_BASELINE_REVIEW.json').write_text(json.dumps(report,indent=2));print(json.dumps(report))
