import json
from pathlib import Path
p=Path('work/stat_dependency'); d=json.loads((p/'old-data-metadata.json').read_text())
enums={e['name']:e for e in d['enums']}
def enumtext(name):
 e=enums[name];return 'enum '+name+' {\n'+''.join('    '+v['name']+' = '+str(v['value'])+',\n' for v in e['values'])+'};\n'
print(list(enums))
jobs=next(e for e in d['enums'] if any(v['name']=='JOB_MAX_NUM' for v in e['values']))
(p/'common-length-stat-enums.inc').write_text('// Insert at original common/length.h locations. Old DWARF values only.\n'+enumtext(jobs['name'])+enumtext('EApplyTypes')+'// POINT_MAX_NUM belongs to old EMisc; add to its existing declaration when present.\n')
(p/'char-point-enum.inc').write_text('// Insert in game/src/char.h, original EPointTypes location.\n'+enumtext('EPointTypes'))
fields=[m['name'] for r in d['records'] if r['name']=='SJobInitialPoints' and r['unit']=='char.cpp' for m in r['members']]
decl='// Insert into game/src/constants.h; retain existing accessory declarations.\n#include "../../common/length.h"\n#include "../../libthecore/include/typedef.h"\ntypedef struct SJobInitialPoints {\n'+''.join('    int '+f+';\n' for f in fields)+'} TJobInitialPoints;\ntypedef struct SApplyInfo { BYTE bPointType; } TApplyInfo;\nextern TJobInitialPoints JobInitialPoints[JOB_MAX_NUM];\nextern const TApplyInfo aApplyInfo[MAX_APPLY_NUM];\n'
(p/'constants-stat-declarations.inc').write_text(decl)
rows=next(g['signed_int_rows'] for g in d['globals'] if g['name']=='JobInitialPoints')
applies=next(g['byte_values'] for g in d['globals'] if g['name']=='aApplyInfo')
pointnames={v['value']:v['name'] for v in enums['EPointTypes']['values']}
definitions='// Insert into game/src/constants.cpp; retain accessory definitions.\n// constants.h and char.h (EPointTypes) must be included by the translation unit.\nTJobInitialPoints JobInitialPoints[JOB_MAX_NUM] = {\n'+''.join('    {'+', '.join(map(str,r))+'},\n' for r in rows)+'};\nconst TApplyInfo aApplyInfo[MAX_APPLY_NUM] = {\n'+''.join('    {'+pointnames.get(v,str(v))+'}, // apply '+str(i)+'\n' for i,v in enumerate(applies))+'};\n'
(p/'constants-stat-definitions.inc').write_text(definitions)
abi=['// Staged ABI check. Do not build until candidate fragments are integrated.','#include "../../game/src/constants.h"','#include "../../game/src/char.h"','#include <cstddef>','#define STAT_ASSERT(name,expr) typedef char stat_assert_##name[(expr)?1:-1]','STAT_ASSERT(int32,sizeof(int)==4);','STAT_ASSERT(long32,sizeof(long)==4);','STAT_ASSERT(job64,sizeof(TJobInitialPoints)==64);','STAT_ASSERT(apply1,sizeof(TApplyInfo)==1);','STAT_ASSERT(jobcount,JOB_MAX_NUM==4);','STAT_ASSERT(applycount,MAX_APPLY_NUM==82);','STAT_ASSERT(pointcount,POINT_MAX_NUM==255);']
abi += ['STAT_ASSERT(job_'+f+',offsetof(TJobInitialPoints,'+f+')=='+str(i*4)+');' for i,f in enumerate(fields)]
abi += ['STAT_ASSERT(apply_offset,offsetof(TApplyInfo,bPointType)==0);','STAT_ASSERT(job_table256,sizeof(JobInitialPoints)==256);','STAT_ASSERT(apply_table82,sizeof(aApplyInfo)==82);','STAT_ASSERT(character_point1056,sizeof(CHARACTER_POINT)==1056);','STAT_ASSERT(character_point_instant1688,sizeof(CHARACTER_POINT_INSTANT)==1688);','']
(p/'stat_data_abi.cpp').write_text('\n'.join(abi))
(p/'point-max-num.inc').write_text('// Add to common/length.h EMisc (existing or recovered), not EPointTypes.\n// Old char.cpp/constants.cpp/item.cpp DWARF: POINT_MAX_NUM = 255.\n// Enumerator fragment:\nPOINT_MAX_NUM = 255,\n')
print('Staged enum, record, table and ABI fragments; no build executed')
