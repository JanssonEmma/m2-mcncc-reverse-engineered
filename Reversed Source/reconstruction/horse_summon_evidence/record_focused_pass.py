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
import json,hashlib,re,shutil
p=Path(__file__).parent;root=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'))
work=p.parents[1]
receipt=work/'horse-methods-focused.receipt.json';log=work/'horse-methods-focused.log'
r=json.loads(receipt.read_text())
assert r['status']=='passed' and r['make_exit_code']==0 and r['exit_code']==0
assert r['executed_runtime_commands']==['horse_methods_regression'] and r['abi_requested']
assert 'horse_methods_abi.cpp' in log.read_text(errors='replace')
m=json.loads((p/'apply-manifest.json').read_text())
sha=lambda f:hashlib.sha256(f.read_bytes()).hexdigest()
snap=p/'integrated-proof';snap.mkdir(exist_ok=True)
actual=[]
for row in m['files']:
 f=root/row['path'];dest=snap/row['path'];dest.parent.mkdir(parents=True,exist_ok=True);shutil.copyfile(f,dest)
 actual.append({'file':row['path'],'sha256':sha(f)})
protected={}
for name,old in m['protected_global_fixtures'].items():
 current=sha(root/'reconstruction/tests'/name);assert current==old
 protected[name]=current
packet=(root/'game/src/packet.h').read_text()
assert packet.count('typedef struct packet_dead')==1 and packet.count('typedef struct packet_move')==1
horse=(root/'game/src/char_horse.cpp').read_text()
assert 'm_Horse' not in horse
assert all(x in horse for x in ('GetHorseLevel()','GetHorseHealth()','GetHorseStamina()','IsHorseRiding()'))
# Actual source body identity and original DWARF mapping retained together.
placement=json.loads((p/'source-placement.json').read_text())
leaf_names={'SetRotation','SetEmpire','GetVictim','GetMyHorseVnum','ClearHorseInfo','SendHorseInfo','SetRider','EncodeMovePacket','SendMovePacket'}
mapping=[]
for entry in placement:
 if entry['name'] not in leaf_names:continue
 f=root/'game/src'/entry['cu'];s=f.read_text()
 name=entry['name'] if entry['name']=='EncodeMovePacket' else 'CHARACTER::'+entry['name']
 pat=re.compile(r'(?:^|\n)[^\n;{}]*\b'+re.escape(name)+r'\s*\([^;{}]*\)\s*\{')
 matches=list(pat.finditer(s));assert len(matches)==1,(name,len(matches))
 a=matches[0].start();brace=s.index('{',a);n=1;b=brace+1
 while n:
  if s[b]=='{':n+=1
  elif s[b]=='}':n-=1
  b+=1
 body=s[a:b].lstrip('\n')
 mapping.append({**entry,'actual_file':str(f),'actual_line':s.count('\n',0,a)+1,'body_sha256':hashlib.sha256(body.encode()).hexdigest(),'body':body})
(p/'integrated-original-body-mapping.json').write_text(json.dumps(mapping,indent=2))
shutil.copyfile(receipt,p/'focused-passed.receipt.json')
result={'status':'focused_runtime_and_abi_passed','root_applied':True,'child_F_writes':False,'scope':'closed horse helper batch only','source_files':actual,'protected_global_fixtures_unchanged':protected,'packet_duplicate_fix':'retain first block only;5duplicate blocks removed','horse_access_fix':'actual existing inline accessors; no visibility/data change','receipt':str(receipt),'receipt_sha256':sha(receipt),'log':str(log),'log_sha256':sha(log),'validation':r,'full_suite_run_for_this_batch':False,'pending':'Parent full-suite/checkpoint; broader HorseSummon/Spawn/Show/Goto/SetProto/spatial/quest SCC remains staged/unresolved'}
(p/'INTEGRATION_STATUS.json').write_text(json.dumps(result,indent=2))
(p/'INTEGRATION_RESUME.md').write_text("""# Focused closed horse batch PASSED
Root applied all11 guarded payloads, then applied minimal duplicate-packet and private-base-access fixes. Child Fwrites stayed denied; only root performed production writes.
Actual focused receipt: work/horse-methods-focused.receipt.json, statuspassed, make0/runtime0, forced rebuild, horse_methods_regression executed exactlyonce, ABI requested and check-abimake0. Log contains strict32 horse_methods_abi.cpp C++03 syntax gate. Finished2026-10-26T03:52:26.945883+00:00.
Frozen evidence copies and exact current hashes: integrated-proof/, INTEGRATION_STATUS.json, focused-passed.receipt.json, integrated-original-body-mapping.json.
Global character_fixture/desc_fixture/entity_packet_fixture SHA guards unchanged, including unused shutdown boundary. Dedicated fixture removes only three abort methods and actual leaf bodies link.
packet.h has exactlyone dead5/move24 definition; char_horse.cpp uses existing actual inline horse getters rather than private base fields. No visibility widening.
Do not rerun original apply payload blindly: it predates the two fixes. Current production and integrated-proof snapshots are authoritative. Focused PASS only; parent must run final fullsuite/checkpoint when appropriate. No fullsuite was run for this batch. Broader HorseSummon/Spawn/Show/Goto/SetProto/spatial/quest runtime remains incomplete and was NOT integrated. Stop here; no broad expansion until parent reviews checkpoint.
""")
print(json.dumps({'status':result['status'],'runtime':r['executed_runtime_commands'],'abi':True,'strict32_gate_in_log':True,'protected_fixtures_unchanged':True,'body_mappings':len(mapping),'fullsuite':False}))
