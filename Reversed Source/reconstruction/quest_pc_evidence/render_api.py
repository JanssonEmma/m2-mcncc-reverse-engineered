import json
from pathlib import Path
p=Path(__file__).resolve().parent
rows=json.loads((p/'pc-full-record-api-dwarf.json').read_text())
r=next(x for x in rows if x['record']['name']=='PC' and x['cu']=='questpc.cpp')
def typ(t):
 tag=t.get('tag');n=t.get('name')
 if tag=='DW_TAG_reference_type':return typ(t['type'])+'&'
 if tag=='DW_TAG_pointer_type':return typ(t['type'])+'*'
 if tag=='DW_TAG_const_type':return 'const '+typ(t['type'])
 if n=='LPCHARACTER':return 'CHARACTER*'
 if n and ('basic_string<char' in n or n in ('string','std::string')):return 'std::string'
 if n and ('_Rb_tree_iterator<' in n):return 'QuestInfoIterator'
 if n:return n.replace('quest::','')
 if 'type' in t:return typ(t['type'])
 raise ValueError(t)
pub=[];priv=[];manifest=[]
for m in r['children']:
 if m['tag']!='DW_TAG_subprogram' or m['artificial']:continue
 name=m['name']
 if name=='PC':decl='    PC();'
 elif name=='~PC':decl='    ~PC();'
 else:
  params=[typ(q['type']) for q in m['params'] if not q['artificial']]
  cv=' const' if (m.get('linkage') or '').startswith('_ZNK') else ''
  decl='    '+typ(m['type'])+' '+name+'('+','.join(params)+')'+cv+';'
 (priv if m['access']==3 else pub).append(decl)
 manifest.append({'declaration':decl.strip(),'access':'private' if m['access']==3 else 'public','linkage':m['linkage'],'source':m['file'],'line':m['line']})
(p/'pc-public-api.fragment').write_text('    typedef std::map<unsigned int,QuestState> QuestInfo;\n    typedef QuestInfo::iterator QuestInfoIterator;\n'+'\n'.join(pub)+'\n')
(p/'pc-private-api.fragment').write_text('    typedef std::map<std::string,int> TFlagMap;\n    typedef std::map<std::string,LPEVENT> TTimerMap;\n'+'\n'.join(priv)+'\n')
(p/'pc-api-declarations.json').write_text(json.dumps(manifest,indent=2))
print('\n'.join(pub+priv));print('Count',len(manifest))
