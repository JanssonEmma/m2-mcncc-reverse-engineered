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
import json,re
from pathlib import Path
from elftools.elf.elffile import ELFFile
p=Path('work/locale_closure');ins=json.loads((p/'080686c0-instructions.json').read_text())['items'];out=[]
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 elf=ELFFile(f)
 def string(a):
  for s in elf.iter_sections():
   if s['sh_addr']<=a<s['sh_addr']+s['sh_size']:
    raw=s.data()[a-s['sh_addr']:];return raw.split(b'\0',1)[0].decode('ascii')
  raise ValueError(hex(a))
 last=None
 for i,r in enumerate(ins):
  m=re.match(r'MOV (?:EAX|ECX|EDX),0x(81f[0-9a-f]+)$',r['instruction'])
  if m:last=(int(m.group(1),16),r['address'])
  if r['instruction']=='CALL 0x0804bd0c':
   assert last
   # Each compare's next immediate scalar move is its matching enum value.
   for following in ins[i+1:]:
    m=re.match(r'MOV (?:EAX|ECX|EDX),0x([0-9a-f]+)$',following['instruction'])
    if m and 1<=int(m.group(1),16)<=18:
     out.append({'name':string(last[0]),'literal_address':hex(last[0]),'literal_load':last[1],'compare':r['address'],'enum':int(m.group(1),16),'enum_load':following['address']});break
   else:raise ValueError('missing enum')
assert len(out)==18 and [r['enum'] for r in out]==list(range(1,19))
(p/'localization-dispatch-proof.json').write_text(json.dumps(out,indent=2))
enums=['YMIR','LETSGAME','CIBN','JAPAN','ENGLISH','HONGKONG','NEWCIBN','GERMANY','KOREA','FRANCE','ITALY','SPAIN','UK','TURKEY','POLAND','PORTUGAL','CANADA','BRAZIL']
body=['// Complete original locale_service.cpp1017, 080686c0.','bool LC_InitLocalization(const std::string& szLocal)','{','    g_stLocal=szLocal;']
for i,r in enumerate(out):body.append('    '+('if' if i==0 else 'else if')+'(g_stLocal=='+json.dumps(r['name'])+') g_eLocalType=LC_'+enums[i]+';')
body+=['    else return false;','    return true;','}']
(p/'locale-localization-init.inc').write_text('\n'.join(body)+'\n')
print(out)
