"""Prepare a fresh header-only PointChange syntax overlay. NEVER invokes compiler.
Copies complete existing production headers; adds metadata-proven declarations
only. No app bodies, fake records, casts around private access, or link stubs.
"""
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
import argparse,datetime,hashlib,json,pathlib,re,shutil
BASE=pathlib.Path(__file__).resolve().parent
APIS=[
 ('game/src/char.h','CHARACTER','void','PointChange',['BYTE','int','bool','bool'],'void PointChange(BYTE type,int amount,bool bAmount,bool bBroadcast);','dwarf.json:_ZN9CHARACTER11PointChangeEhibb'),
 ('game/src/char.h','CHARACTER','void','ComputeBattlePoints',[],'void ComputeBattlePoints();','battle_points/dwarf.json:_ZN9CHARACTER19ComputeBattlePointsEv'),
 ('game/src/char.h','CHARACTER','void','ComputePoints',[],'void ComputePoints();','dwarf.json:_ZN9CHARACTER13ComputePointsEv'),
 ('game/src/char.h','CHARACTER','void','Save',[],'void Save();','dwarf.json:_ZN9CHARACTER4SaveEv'),
 ('game/src/char.h','CHARACTER','LPITEM','AutoGiveItem',['DWORD','BYTE','int','bool'],'LPITEM AutoGiveItem(DWORD vnum,BYTE count,int iRarePct,bool bMsg);','dwarf.json:_ZN9CHARACTER12AutoGiveItemEmhib'),
 ('game/src/char.h','CHARACTER','void','SetPolymorph',['DWORD','bool'],'void SetPolymorph(DWORD vnum,bool maintainStat);','dwarf.json:_ZN9CHARACTER12SetPolymorphEmb'),
 ('game/src/char.h','CHARACTER','void','MountVnum',['DWORD'],'void MountVnum(DWORD vnum);','dwarf.json:_ZN9CHARACTER9MountVnumEm'),
 ('game/src/questmanager.h','CQuestManager','void','LevelUp',['unsigned int'],'void LevelUp(unsigned int pc);','logger-quest-dwarf.json:_ZN5quest13CQuestManager7LevelUpEj'),
 ('game/src/guild.h','CGuild','void','LevelChange',['DWORD','BYTE'],'void LevelChange(DWORD pid,BYTE level);','dwarf.json:_ZN6CGuild11LevelChangeEmh'),
 ('game/src/log.h','LogManager','void','Query',['const char*','...'],'void Query(const char* format,...);','logger-quest-dwarf.json:_ZN10LogManager5QueryEPKcz(public)'),
 ('game/src/log.h','LogManager','void','CharLog',['DWORD','DWORD','DWORD','DWORD','const char*','const char*'],'void CharLog(DWORD pid,DWORD x,DWORD y,DWORD value,const char* text,const char* hint);','logger-quest-dwarf.json:_ZN10LogManager7CharLogEmmmmPKcS1_')]
def sha(data):return hashlib.sha256(data).hexdigest()
def clean(text):
 pat=r'//[^\n]*|/\*.*?\*/|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\''
 return re.sub(pat,lambda m:''.join('\n' if c=='\n' else ' ' for c in m[0]),text,flags=re.S)
def class_span(text,name):
 c=clean(text);m=re.search(r'\bclass\s+'+re.escape(name)+r'\b[^;{]*\{',c)
 if not m:raise ValueError('Complete class not found: '+name)
 opening=c.index('{',m.start());depth=1
 for i in range(opening+1,len(c)):
  if c[i]=='{':depth+=1
  elif c[i]=='}':
   depth-=1
   if depth==0:return opening,i
 raise ValueError('Unbalanced original class '+name)
def norm(s):return re.sub(r'\s+','',s)
def param_type(text):
 s=text.split('=',1)[0].strip()
 if s=='...':return s
 words=list(re.finditer(r'\b[A-Za-z_]\w*\b',s))
 known={'void','bool','int','char','long','short','unsigned','signed','const','DWORD','BYTE','LPITEM'}
 if words and words[-1][0] not in known:
  m=words[-1];s=s[:m.start()]+s[m.end():]
 return norm(s)
def patch_api(text,api):
 header,cls,ret,name,params,decl,proof=api;opening,closing=class_span(text,cls);body=clean(text)[opening+1:closing]
 # Existing declarations are parsed only for this exact original signature.
 pattern=r'(?m)^[ \t]*(?P<ret>(?:(?:virtual|inline|static)\s+)*(?:void|LPITEM|CItem\s*\*))\s+'+re.escape(name)+r'\s*\((?P<params>[^;{}()]*)\)\s*(?P<const>const\s*)?;'
 matches=list(re.finditer(pattern,body));exact=None
 for m in matches:
  got=[] if not m['params'].strip() else [param_type(x) for x in m['params'].split(',')]
  if got==[norm(x) for x in params]:
   actual_ret=re.sub(r'\b(?:virtual|inline|static)\s+','',m['ret']).strip()
   if norm(actual_ret) not in ({'LPITEM','CItem*'} if ret=='LPITEM' else {norm(ret)}) or m['const']:
    raise ValueError(header+': existing incompatible return/const for '+name)
   exact=m;break
 if exact and name!='Query':return text,{'header':header,'class':cls,'method':name,'action':'already_exact','proof':proof}
 if exact and name=='Query':
  labels=list(re.finditer(r'\b(public|private|protected)\s*:',body[:exact.start()]))
  access=labels[-1][1] if labels else 'private'
  if access=='public':return text,{'header':header,'class':cls,'method':name,'action':'already_exact_public','proof':proof}
  a=opening+1+exact.start();b=opening+1+exact.end();text=text[:a]+text[b:];opening,closing=class_span(text,cls)
  action='move_existing_Query_to_original_public_access'
 else:action='add_missing_original_declaration'
 insertion='\npublic:\n    // Syntax overlay declaration only; '+proof+'\n    '+decl+'\n'
 text=text[:closing]+insertion+text[closing:]
 return text,{'header':header,'class':cls,'method':name,'action':action,'proof':proof,'declaration':decl}
def main():
 ap=argparse.ArgumentParser();ap.add_argument('--production-root',default=_resolve_portable_path('PROJECT_ROOT\\Reversed Source'));ap.add_argument('--out');args=ap.parse_args()
 source=pathlib.Path(args.production_root).resolve()
 stamp=datetime.datetime.now(datetime.timezone.utc).strftime('%Y%m%dT%H%M%S%fZ')
 out=pathlib.Path(args.out).resolve() if args.out else BASE/('syntax_tree_'+stamp)
 # Refuse in-place production edits and stale overlay reuse.
 if out==source or source in out.parents:raise ValueError('Overlay must be outside production tree')
 if out.exists():raise ValueError('Choose a NEW overlay directory; existing path refused: '+str(out))
 out.mkdir(parents=True)
 exts={'.h','.hpp','.hh','.hxx','.inc','.inl'};headers=[]
 for f in source.rglob('*'):
  if not f.is_file() or f.suffix.lower() not in exts or '.git' in f.parts:continue
  rel=f.relative_to(source);data=f.read_bytes();dest=out/rel;dest.parent.mkdir(parents=True,exist_ok=True);dest.write_bytes(data)
  headers.append({'relative':str(rel),'source_sha256':sha(data),'bytes':len(data)})
 changes=[]
 for api in APIS:
  f=out/api[0];text=f.read_text(encoding='utf-8-sig');text,record=patch_api(text,api);f.write_text(text,encoding='utf-8');changes.append(record)
 constants=out/'game/src/constants.h';text=constants.read_text(encoding='utf-8-sig');c=clean(text)
 m=re.search(r'\bextern\s+([^;{}]+?)\bexp_table\s*;',c)
 if m:
  if norm(m[1]) not in {'constDWORD*','DWORDconst*'}:raise ValueError('Existing exp_table type differs from required mutableconstDWORD*; no shadow declaration added')
  changes.append({'header':'game/src/constants.h','global':'exp_table','action':'already_exact','proof':'Original mutablepointer/readonlytable metadata + parent requested constDWORD* declaration'})
 else:
  at=text.rfind('#endif')
  if at<0:raise ValueError('constants.h guard not found')
  text=text[:at]+'\n// Syntax-only ORIGINAL extern: no definition/selection invented.\nextern const DWORD* exp_table;\n'+text[at:];constants.write_text(text,encoding='utf-8')
  changes.append({'header':'game/src/constants.h','global':'exp_table','action':'add_extern_declaration_only','proof':'stat-dependency-old-data-metadata / readonlytable / parent requested type; actual selection remains frontier'})
 candidate=BASE/'PointChange.cpp.candidate';dest=out/'game/src/PointChange.syntax.cpp';dest.write_bytes(candidate.read_bytes())
 sig=out/'reconstruction/tests/point_change_syntax_signatures.cpp';sig.parent.mkdir(parents=True,exist_ok=True);sig.write_bytes((BASE/'signatures.cpp.candidate').read_bytes())
 changed=[]
 for h in headers:
  f=source/h['relative']
  if not f.exists() or sha(f.read_bytes())!=h['source_sha256']:changed.append(h['relative'])
 if changed:raise RuntimeError('Production headers changed during snapshot; prepare fresh overlay: '+str(changed))
 for h in headers:h['overlay_sha256']=sha((out/h['relative']).read_bytes())
 flags=['-std=c++03','-m32','-fsyntax-only','-D_GLIBCXX_USE_CXX11_ABI=0','-Wno-long-long','-Ifdwatch_shim','-Isocket_shim','-Iposix_shim','-Isql_shim','-I../dependencies/mysql-4.1.3/include','-include','sql_socket_platform.h','../../game/src/PointChange.syntax.cpp','point_change_syntax_signatures.cpp']
 manifest={'kind':'header-only syntax overlay, NOT functional recovery','production_root':str(source),'overlay':str(out),'created_utc':stamp,'candidate_sha256':sha(candidate.read_bytes()),'copied_complete_headers':headers,'header_mutations':changes,'new_app_bodies':[],'compiler_invoked':False,'compiler_cwd':str(out/'reconstruction/tests'),'compiler_args':flags,'functional_frontier_file':str(BASE/'dependency-frontier.json')}
 (out/'SYNTAX_MANIFEST.json').write_text(json.dumps(manifest,indent=2),encoding='utf-8')
 print(json.dumps({'overlay':str(out),'headers':len(headers),'declaration_changes':sum(x['action'].startswith(('add_','move_')) for x in changes),'manifest':str(out/'SYNTAX_MANIFEST.json'),'compiler_invoked':False},indent=2))
if __name__=='__main__':main()
