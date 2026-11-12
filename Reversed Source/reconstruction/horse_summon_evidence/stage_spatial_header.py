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
import json,re,sys
p=Path(__file__).parent
sys.path.insert(0,_resolve_portable_path('USER_HOME\\Documents\\Codex\\2026-10-23\\i-h\\work\\python-deps'))
exec(Path(_resolve_portable_path('USER_HOME\\Documents\\Codex\\2026-10-23\\i-h\\work\\cleanup_types\\extract.py')).read_text().split("with open(r'F:")[0])
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 dw=ELFFile(f).get_dwarf_info();enums=[]
 for cu in dw.iter_CUs():
  if val(cu.get_top_DIE(),'DW_AT_name')!='sectree_manager.cpp':continue
  for d in cu.iter_DIEs():
   if d.tag=='DW_TAG_enumeration_type' and val(d,'DW_AT_name')=='EAttrRegionMode':
    enums=[{'name':val(c,'DW_AT_name'),'value':val(c,'DW_AT_const_value')} for c in d.iter_children()]
p.joinpath('sectree-enum.json').write_text(json.dumps(enums,indent=2))
records=json.loads(p.joinpath('full-records-api.json').read_text())['records']
def tp(t):
 if not t:return 'void'
 tag=t.get('tag');n=t.get('name');inner=t.get('type')
 if tag=='DW_TAG_pointer_type':return tp(inner)+'*'
 if tag=='DW_TAG_reference_type':return tp(inner)+'&'
 if tag=='DW_TAG_const_type':return 'const '+tp(inner)
 if tag=='DW_TAG_volatile_type':return 'volatile '+tp(inner)
 if not n:raise ValueError(t)
 if n=='string':return 'std::string'
 if n=='pixel_position_s':return 'PIXEL_POSITION'
 n=re.sub(r'(?<![:\w])(map|vector|set|pair|basic_string|less|allocator|char_traits)<',r'std::\1<',n)
 return n
decls={}
for name in ('SECTREE_MAP','SECTREE_MANAGER'):
 rec=next(r for r in records if r['cu']=='sectree_manager.cpp' and r['record']['name']==name)
 lines=[];seen=set()
 for m in rec['methods']:
  mn=m['name'];params=[a['type'] for a in m['params']]
  if params and params[0].get('tag')=='DW_TAG_pointer_type' and params[0].get('type',{}).get('name')==name:params=params[1:]
  args=','.join(tp(t) for t in params)
  if mn.startswith('~'):line='virtual '+mn+'();'
  elif mn==name:line=mn+'('+args+');'
  else:line=('virtual ' if m.get('virtuality') else '')+tp(m['type'])+' '+mn+'('+args+');'
  if line not in seen:lines.append(line);seen.add(line)
 decls[name]='\n    '.join(lines)
header=r"""// Full original spatial-manager DATA and API recovered from target DWARF.
// Lifecycle/map loading bodies remain pending; no reduced stand-in class.
#ifndef MCNCC_STAGED_SECTREE_MANAGER_H
#define MCNCC_STAGED_SECTREE_MANAGER_H
#include "sectree.h"
#include "../../common/singleton.h"
#include <map>
#include <vector>
#include <string>
class CHARACTER;
struct SMapRegion {
    int index,sx,sy,ex,ey;
    PIXEL_POSITION posSpawn;
    bool bEmpireSpawnDifferent;
    PIXEL_POSITION posEmpire[3];
    std::string strMapName;
};
typedef SMapRegion TMapRegion;
struct TAreaInfo {
    int sx,sy,ex,ey,dir;
    TAreaInfo(int,int,int,int,int);
};
struct npc_info {
    BYTE bType;
    const char* name;
    long x,y;
    npc_info(BYTE,const char*,long,long);
};
typedef std::map<std::string,TAreaInfo> TAreaMap;
struct SSetting {
    int iIndex,iCellScale,iBaseX,iBaseY,iWidth,iHeight;
    PIXEL_POSITION posSpawn;
};
typedef SSetting TMapSetting;
enum EAttrRegionMode { ENUMS };
class SECTREE_MAP;
typedef SECTREE_MAP* LPSECTREE_MAP;
class SECTREE_MAP:public std::map<DWORD,SECTREE*> {
public:
    MAP_API
    TMapSetting m_setting;
};
class SECTREE_MANAGER:public singleton<SECTREE_MANAGER> {
public:
    MANAGER_API
private:
    std::map<DWORD,SECTREE_MAP*> m_map_pkSectree;
    std::map<int,std::map<std::string,TAreaInfo> > m_map_pkArea;
    std::vector<SMapRegion> m_vec_mapRegion;
    std::map<DWORD,std::vector<npc_info> > m_mapNPCPosition;
};
#endif
"""
header=header.replace('ENUMS',', '.join(x['name']+'='+str(x['value']) for x in enums)).replace('MAP_API',decls['SECTREE_MAP']).replace('MANAGER_API',decls['SECTREE_MANAGER'])
p.joinpath('candidate-sectree_manager.h').write_text(header)
print(header)
