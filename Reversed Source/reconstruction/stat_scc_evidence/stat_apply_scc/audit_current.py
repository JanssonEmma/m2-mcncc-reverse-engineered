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
import json,re,hashlib
p=Path('work/stat_apply_scc');root=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'));meta=json.loads(Path('work/stat_dependency/old-data-metadata.json').read_text());glob=next(x for x in meta['globals'] if x['name']=='aApplyInfo');mapping=json.loads(Path('work/stat_dependency/apply-point-map.json').read_text());s=(root/'game/src/constants.cpp').read_text();body=s.split('const TApplyInfo aApplyInfo[MAX_APPLY_NUM] = {',1)[1].split('};',1)[0];names=re.findall(r'\{(POINT_\w+)\}',body);actual=[next(r['point'] for r in mapping if r['point_name']==n) for n in names];assert actual==glob['byte_values'] and len(actual)==82
sources={str(f.relative_to(root)).replace('\\','/'):f.read_text(errors='replace') for f in (root/'game/src').glob('*.cpp')};need=['CHARACTER::ApplyPoint','CHARACTER::PointChange','CHARACTER::ComputeBattlePoints','CHARACTER::ComputePoints','CHARACTER::GetJob','CHARACTER::GetOriginalPart','CHARACTER::SetPart','CHARACTER::SetWear','CHARACTER::SetItem','CHARACTER::RemoveAffect','CHARACTER::IsAffectFlag','CItem::ModifyPoints','CItem::AccessorySocketDegrade','CItem::StartAccessorySocketExpireEvent','CItem::SetAccessorySocketExpireEvent','CItem::StopAccessorySocketExpireEvent','CItem::SetAccessorySocketGrade','CItem::SetAccessorySocketDownGradeTime','CItem::IsAccessoryForSocket','ITEM_MANAGER::GetTable','ITEM_MANAGER::FindByVID','CItem::Unequip','CItem::RemoveFromCharacter','ITEM_MANAGER::DestroyItem']
rows=[]
for name in need:
 matches=[]
 for file,s in sources.items():
  for m in re.finditer(re.escape(name)+r'\s*\(',s):
   # Report source occurrence separately from definition evidence; avoid claiming a call is real body.
   before=s[max(0,m.start()-50):m.start()];after=s[m.end():];end=after.find(')');suffix=after[end+1:].lstrip() if end>=0 else ''
   if suffix.startswith('{'):matches.append({'file':file,'line':s.count('\n',0,m.start())+1,'sha256':hashlib.sha256((root/file).read_bytes()).hexdigest()})
 rows.append({'method':name,'current_definition_candidates':matches,'status':'actual-current-definition' if matches else 'not-found-current-cpp-definition; inspect staged/inline API before promotion'})
report={'scope':'Read-only snapshot, source definition candidates are not functional validation','table':{'name':'aApplyInfo','address':'0821ae80','count':82,'record_size':1,'const':True,'raw_sha256':glob['sha256'],'current_values_exact_original':True},'methods':rows}
p.joinpath('current-closure-audit.json').write_text(json.dumps(report,indent=2)+'\n');p.joinpath('aApplyInfo-original.json').write_text(json.dumps({'global':glob,'mapping':mapping,'records':[r for r in meta['records'] if r['name'] in ('SApplyInfo','SJobInitialPoints')],'enums':[e for e in meta['enums'] if any(x['name']=='MAX_APPLY_NUM' for x in e['values'])]},indent=2)+'\n');print([(x['method'],bool(x['current_definition_candidates'])) for x in rows])
