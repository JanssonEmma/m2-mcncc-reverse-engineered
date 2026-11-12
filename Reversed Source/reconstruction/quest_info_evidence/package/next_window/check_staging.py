from pathlib import Path
import json,re,hashlib,ast
p=Path('work/pc_quest_info');n=p/'next_window';old=json.loads((p/'MANIFEST.json').read_text(encoding='utf-8-sig'));r=next(r for r in old['files'] if r['path'].endswith('/questpc.cpp.candidate'));assert hashlib.sha256((p/'questpc.cpp.candidate').read_bytes()).hexdigest()==r['sha256'],'Frozen13bodieschanged'
for f in n.glob('*.py'):ast.parse(f.read_text(encoding='utf-8-sig'))
for f in n.glob('*.cpp'):
 s=re.sub(r'//[^\n]*|/\*.*?\*/|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'','',f.read_text(encoding='utf-8-sig'),flags=re.S)
 for a,b in [('(',')'),('{','}'),('[',']')]:
  count=0
  for c in s:
   if c==a:count+=1
   if c==b:count-=1
   assert count>=0,(f,a)
  assert count==0,(f,a,count)
x=json.loads((n/'integration/plan.json').read_text(encoding='utf-8-sig'));assert len(x['files'])==7 and x['runtime_count_after']==103
for row in x['files']:assert hashlib.sha256((n/'integration/payload'/row['path']).read_bytes()).hexdigest()==row['after']
functions=json.loads((p/'evidence/quest-info-addressed-ghidra.json').read_text(encoding='utf-8-sig'));reviews=[]
for r in functions['functions']:reviews.append({'address':r['function']['address'],'source_refs':[{'path':'game/src/questpc.cpp','anchor':r['function']['name']+'('}],'test_refs':[{'path':'reconstruction/tests/pc_quest_info_regression.cpp','anchor':'int main('}],'review_scope':'PENDING realpool/PC/state/CHAR/DESC network regression: all7masks and oldheader6, context/state/mask callbackreloads, fixedwidthsafeinitialized fields, cachedBeginlog vsfreshotherfields. No shorttail/indeterminate/VM/nullunsafe domain claim. No builds run.','notes':'Refresh hashes/anchors only after parent normal/O2/strict/full passes. Contextgetter inline scope not standaloneaddress.'})
n.joinpath('mappings.pending.json').write_text(json.dumps({'target_sha256':functions['target_sha256'],'reviews':reviews},indent=2)+'\n',encoding='utf-8')
print('Staticonly: frozen13bodySHAunchanged; PythonAST/C++delimiters,7payloadhashes,103canonicalregistryorder checked. No compile/apply.')
