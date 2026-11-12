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
import pathlib,json,struct
exec(pathlib.Path('work/cleanup_types/extract.py').read_text().split('with open(')[0])
p=pathlib.Path('work/party_dependency');a=json.loads((p/'dwarf.json').read_text());cl=next(x for x in a if x['node']['name']=='CParty' and x['node']['size'])
def offset(at):
 v=at.get('DW_AT_data_member_location');n=0;shift=0
 if v:
  for b in v[1:]:
   n|=(b&127)<<shift;shift+=7
 return n

def fmt(t):
 if not t:return 'void'
 n=t.get('name');tag=t.get('tag')
 if n in ('LPCHARACTER','LPDUNGEON'):return {'LPCHARACTER':'CHARACTER*','LPDUNGEON':'CDungeon*'}[n]
 if n=='string' or n and n.startswith('basic_string<'):return 'std::string'
 if n and n.startswith('map<long unsigned int,CParty::SMember'):return 'TMemberMap'
 if n and n.startswith('map<std::basic_string'):return 'TFlagMap'
 if n and n.startswith('_Rb_tree_iterator<'):return 'TMemberMap::iterator'
 if tag=='DW_TAG_pointer_type':return fmt(t.get('type'))+'*'
 if tag=='DW_TAG_reference_type':return fmt(t.get('type'))+'&'
 if tag=='DW_TAG_const_type':return 'const '+fmt(t.get('type'))
 return n or ('void' if tag is None else 'UNRESOLVED')
methods=[]
for c in cl['children']:
 n=c['node'];at=c['attrs']
 if n['tag']!='DW_TAG_subprogram' or at.get('DW_AT_artificial'):continue
 params=[]
 for x in c['children']:
  if x['attrs'].get('DW_AT_artificial'):continue
  if x['node']['tag']=='DW_TAG_formal_parameter':params.append(fmt(x['node'].get('type')))
  if x['node']['tag']=='DW_TAG_unspecified_parameters':params.append('...')
 sig=('virtual ' if at.get('DW_AT_virtuality') else '')+('' if n['name'] in ('CParty','~CParty') else fmt(n.get('type'))+' ')+n['name']+'('+', '.join(params)+');'
 methods.append({'name':n['name'],'signature':sig,'access':at.get('DW_AT_accessibility',1),'virtual':at.get('DW_AT_virtuality',0),'vtable':at.get('DW_AT_vtable_elem_location'),'linkage':at.get('DW_AT_MIPS_linkage_name'),'die':n['die']})
(p/'method-api.json').write_text(json.dumps(methods,indent=2))
fields=[{'name':c['node']['name'],'type':fmt(c['node'].get('type')),'offset':offset(c['attrs']),'node':c['node']} for c in cl['children'] if c['node']['tag']=='DW_TAG_member']
(p/'layout.json').write_text(json.dumps({'CParty_size':160,'SMember_size':12,'fields':fields},indent=2))
header='''// Complete old CParty declaration recovered from party.cpp DWARF.
// Historical GCC3 STL layout: class160/member12; native STL ABI may differ.
#ifndef MCNCC_GAME_PARTY_H
#define MCNCC_GAME_PARTY_H
#include <map>
#include <string>
#include "../../libthecore/include/typedef.h"
#include "event.h"
class CHARACTER;
class CDungeon;
'''
for r in a:
 if r['node']['tag']=='DW_TAG_enumeration_type':
  header+='enum '+r['node']['name']+' {\n'+',\n'.join('    '+c['node']['name']+' = '+str(c['attrs']['DW_AT_const_value']) for c in r['children'])+'\n};\n'
header+='''class CParty {
public:
    struct SMember {
        CHARACTER* pCharacter; // +0
        bool bNear;           // +4
        BYTE bRole;           // +5
        BYTE bLevel;          // +6
        std::string strName;  // +8 (historical4-byte COW string)
    };
    typedef SMember TMember;
    typedef std::map<DWORD,TMember> TMemberMap;
    typedef std::map<std::string,int> TFlagMap;
'''
for access,label in [(1,'public'),(2,'protected'),(3,'private')]:
 header+=label+':\n'
 for m in methods:
  if m['access']==access:
   if '<' in m['name']:header+='    // Emitted template instantiation: '+m['signature']+'\n'
   else:header+='    '+m['signature']+'\n'
header+='protected:\n'
for c in cl['children']:
 n=c['node'];at=c['attrs']
 if n['tag']!='DW_TAG_member' or at.get('DW_AT_artificial'):continue
 if n['name']=='m_iExpDistributionMode':header+='private:\n'
 t=n['type'];suffix=''
 if t['tag']=='DW_TAG_array_type':suffix='['+str(t['bounds'][0]+1)+']';t=t['type']
 header+='    '+fmt(t)+' '+n['name']+suffix+'; // +'+str(offset(at))+'\n'
header+='};\n#endif\n';(p/'party.h.candidate').write_text(header)
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 e=ELFFile(f)
 for s in e.iter_segments():
  start=s['p_vaddr']
  if start<=0x8228580<start+s['p_filesz']:
   raw=s.data()[0x8228580-start:0x8228598-start];(p/'vtable.json').write_text(json.dumps({'address':'0x8228580','words':[hex(v) for v in struct.unpack('<6I',raw)]},indent=2));break
print('methods',len(methods),'fields',len(fields));print(header)
