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
import json,re,sys,hashlib
p=Path(__file__).parent
sys.path.insert(0,_resolve_portable_path('USER_HOME\\Documents\\Codex\\2026-10-23\\i-h\\work\\python-deps'))
exec(Path(_resolve_portable_path('USER_HOME\\Documents\\Codex\\2026-10-23\\i-h\\work\\cleanup_types\\extract.py')).read_text().split("with open(r'F:")[0])
targets={0x812d390,0x812a660,0x8153f20,0x816df10,0x816e000,0x816e060,0x816d8c0,0x81282a0,0x812dde0,0x816d940,0x816d890,0x818d010,0x8129ae0,0x812d810}
mapping=[]
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 dw=ELFFile(f).get_dwarf_info()
 for cu in dw.iter_CUs():
  name=val(cu.get_top_DIE(),'DW_AT_name')
  if name not in ('char.cpp','char_horse.cpp','char_battle.cpp','char_manager.cpp'):continue
  for d in cu.iter_DIEs():
   if d.tag=='DW_TAG_subprogram' and val(d,'DW_AT_low_pc') in targets:
    mapping.append({'cu':name,'name':val(d,'DW_AT_name'),'address':hex(val(d,'DW_AT_low_pc')),'linkage':val(d,'DW_AT_MIPS_linkage_name'),'decl_line':val(d,'DW_AT_decl_line')})
p.joinpath('source-placement.json').write_text(json.dumps(mapping,indent=2))
def body(text,name):
 start=text.index(name)
 # Start at declaration's beginning of line; retain original complete method.
 start=text.rfind('\n',0,start)+1
 brace=text.index('{',start);level=1;end=brace+1
 while level:
  if text[end]=='{':level+=1
  elif text[end]=='}':level-=1
  end+=1
 return text[start:end]+'\n'
leaf=p.joinpath('closed-horse-methods.cpp').read_text()
move=p.joinpath('closed-move-packet.cpp').read_text()
char='#include "char.h"\n#include "packet.h"\n#include "../../libthecore/include/utils.h"\n#include "horse-summon-detail.h"\n'
for n in ('CHARACTER::SetRotation','CHARACTER::SetEmpire'):char+=body(leaf,n)+'\n'
for n in ('EncodeMovePacket','CHARACTER::SendMovePacket'):char+=body(move,n)+'\n'
p.joinpath('char.cpp-closed-additions.inc').write_text(char)
horse='#include "char.h"\n#include "guild.h"\n'
for n in ('CHARACTER::GetMyHorseVnum','CHARACTER::ClearHorseInfo','CHARACTER::SendHorseInfo','CHARACTER::SetRider'):horse+=body(leaf,n)+'\n'
p.joinpath('char_horse.cpp-closed-additions.inc').write_text(horse)
p.joinpath('char_battle.cpp-closed-additions.inc').write_text('#include "char.h"\n#include "char_manager.h"\n'+body(leaf,'CHARACTER::GetVictim'))
print(json.dumps(mapping))
