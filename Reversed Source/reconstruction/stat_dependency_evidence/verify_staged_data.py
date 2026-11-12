import json,re,struct,hashlib
from pathlib import Path
p=Path('work/stat_dependency');d=json.loads((p/'old-data-metadata.json').read_text())
defs=(p/'constants-stat-definitions.inc').read_text()
jobtext,applytext=defs.split('const TApplyInfo',1)
rows=[[int(n) for n in text.split(',')] for text in re.findall(r'\{([0-9, \-]+)\}',jobtext)]
jobraw=b''.join(struct.pack('<16i',*r) for r in rows)
points={v['name']:v['value'] for e in d['enums'] if e['name']=='EPointTypes' for v in e['values']}
applyraw=bytes(points[name] for name in re.findall(r'\{(POINT_[A-Z0-9_]+)\}',applytext))
assert jobraw.hex()==next(g['raw_hex'] for g in d['globals'] if g['name']=='JobInitialPoints')
assert applyraw.hex()==next(g['raw_hex'] for g in d['globals'] if g['name']=='aApplyInfo')
checks={}
for filename,enum_names in [('common-length-emisc.inc',['EMisc']),('common-length-stat-enums.inc',['EJobs','EApplyTypes']),('char-point-enum.inc',['EPointTypes'])]:
 text=(p/filename).read_text()
 for enum_name in enum_names:
  e=next(e for e in d['enums'] if e['name']==enum_name)
  values={v['name']:v['value'] for v in e['values']}
  for name,value in values.items():assert re.search(r'\b'+name+r'\s*=\s*'+str(value)+r'\s*,',text),(name,value)
  checks[enum_name]=len(values)
notes=p/'RECOVERY_PLAN.md';text=notes.read_text().replace('52 complete decompilations','57 complete decompilations')
text += '\n## Complete enum and qualifier follow-up\n\ncommon-length-emisc.inc stages all40 original EMisc enumerators, replacing the anonymous CHAT_MAX_LEN declaration when integrated. Use it instead of point-max-num.inc alone; do not add duplicate enumerator names. constant-qualifiers-provenance.json confirms exact DWARF tags SJobInitialPoints, TJobInitialPoints, SApplyInfo and TApplyInfo and the typedef-to-record links. Original ELF JobInitialPoints resides in writable .data (flags3); aApplyInfo resides in nonwritable .rodata (flags2). Preserve mutable job table and const apply table. No member was guessed.\n'
notes.write_text(text)
out={'target_sha256':'61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c','validation':'Data-only static checks; no build, tests, target execution or production mutation','job_definitions_match_original_bytes':True,'apply_definitions_match_original_bytes':True,'enum_enumerator_counts':checks,'decompiled_functions':57,'full_instruction_counts':{'PointChange':1342,'ApplyPoint':255,'ModifyPoints':335},'data_files':{f.name:hashlib.sha256(f.read_bytes()).hexdigest() for f in p.glob('*.inc')}}
(p/'data-staging-validation.json').write_text(json.dumps(out,indent=2))
print(json.dumps(out))
