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
import re,json,hashlib,datetime
p=Path(__file__).parent;root=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'))
prod=list(root.joinpath('game/src').glob('*.cpp'))+list(root.joinpath('game/src').glob('*.h'))
tests=list(root.joinpath('reconstruction/tests').glob('*.cpp'))
names=['CHARACTER::SetPolymorph','CHARACTER::MountVnum','CHARACTER::ComputePoints','CHARACTER::ComputeBattlePoints','CHARACTER::PointChange','CHARACTER::StopRiding','CHARACTER::HorseSummon','CHARACTER::RefreshAffect','CHARACTER::ComputeAffect','CHARACTER::StartMuyeongEvent','CHARACTER::StopMuyeongEvent','CHARACTER::ComputeSkillPoints','CHorseRider::StopRiding','CHorseRider::StartStaminaRegenEvent','CHorseRider::StartStaminaConsumeEvent','CHorseRider::UpdateHorseStamina','CHorseRider::CheckHorseHealthDropTime','horse_stamina_regen_event','horse_stamina_consume_event','skill_muyoung_event','CItem::ModifyPoints','CHARACTER::ApplyPoint','CItem::IsAccessoryForSocket','ITEM_MANAGER::GetTable','CHARACTER::GetWear','CEntity::ViewReencode','CHARACTER::UpdatePacket','CHARACTER::GetPoint','CHARACTER::SetPoint','CHARACTER::GetRealPoint','CHARACTER::SetRealPoint','CHARACTER::GetOriginalPart','CHARACTER::SetPart','CHARACTER::GetJob','CHARACTER::GetName','CGuild::UnderAnyWar','CQuestManager::Unmount','CQuestManager::GetPC','CQuestManager::GetPCForce','NPC::OnUnmount','NPC::HandleReceiveAllEvent','CQuestManager::CanStartQuest','CQuestManager::ExecuteQuestScript','CHARACTER::Save']
def find(name,files):
 pattern=re.compile(r'\b'+re.escape(name)+r'\s*\([^;{}]*\)\s*(?:const\s*)?\{')
 found=[]
 for f in files:
  text=f.read_text(errors='replace')
  for m in pattern.finditer(text):
   line=text.count('\n',0,m.start())+1
   found.append({'file':str(f),'line':line,'definition':text[m.start():m.end()],'sha256':hashlib.sha256(f.read_bytes()).hexdigest()})
 return found
rows=[]
for n in names:
 pr=find(n,prod);tr=find(n,tests)
 rows.append({'name':n,'production_status':'actual_body_present' if pr else 'missing_actual_production_body','production':pr,'test_definitions':tr,'staged_evidence':'mount_polymorph and/or quest_level saved exports; staging is not production'})
# Known source membership and event graph differences from current Makefile/fixtures.
make=root/'reconstruction/tests/Makefile';lines=make.read_text().splitlines()
group_lines=[{'line':i+1,'text':l} for i,l in enumerate(lines) if (l.startswith(('DESC_PACKET_SOURCES =','TARGET_WALKING_SOURCES =','POLY_HELPER_SOURCES =','GUILD_LEVEL_SOURCES =','stop_muyeong_regression$(')) or ('-DMCNCC_TEST_REAL_EVENTS' in l and 'stop_muyeong_regression' in l))]
fixtures=[]
for rel in ('reconstruction/tests/character_fixture.cpp','reconstruction/tests/desc_fixture.cpp','reconstruction/tests/entity_packet_fixture.cpp','reconstruction/tests/quest_flag_fixture.cpp','reconstruction/tests/horse_regression.cpp'):
 f=root/rel
 fixtures.append({'file':str(f),'sha256':hashlib.sha256(f.read_bytes()).hexdigest(),'relevant_lines':[{'line':i+1,'text':l} for i,l in enumerate(f.read_text().splitlines()) if any(t in l for t in ('StopRiding','MCNCC_TEST_REAL_EVENTS','event_cancel','event_create_ex','CQuestManager::CQuestManager','CQuestManager::~CQuestManager','CHorseRider::','horse_stamina'))]})
out={'audit_date':'2026-10-25','scope':'Current production bodies versus separately linked test definitions; no equivalence/build claim','rows':rows,'current_test_graph':group_lines,'fixtures':fixtures,'corrected_conclusion':'CHorseRider::StopRiding, stamina regen/consume setup and callbacks already actual horse_rider.cpp production bodies; CHARACTER virtual StopRiding is separate and remains test-boundary-only. Default DESC event fixture differs from actual event.cpp unless MCNCC_TEST_REAL_EVENTS and real event sources are selected.','makefile_sha256':hashlib.sha256(make.read_bytes()).hexdigest()}
p.joinpath('closure-audit.json').write_text(json.dumps(out,indent=2))
print(json.dumps([{'name':r['name'],'production':bool(r['production']),'test_definitions':len(r['test_definitions'])} for r in rows]))
