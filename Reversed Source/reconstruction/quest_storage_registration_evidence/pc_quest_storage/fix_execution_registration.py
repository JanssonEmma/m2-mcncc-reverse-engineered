from pathlib import Path
p=Path('work/pc_quest_storage/prepare.py');s=p.read_text(encoding='utf-8-sig');s=s.replace('.read_text()','.read_text(encoding=\'utf-8-sig\')').replace('.encode()','.encode(\'utf-8\')');old="s=s.replace('RUNTIME_TESTS = ','RUNTIME_TESTS = pc_quest_storage_regression ',1);changes[rel]=(s+'\\n'+(p/'Makefile.addition').read_text(encoding='utf-8-sig')).encode('utf-8')";assert s.count(old)==1
new="""s=s.replace('RUNTIME_TESTS = ','RUNTIME_TESTS = pc_quest_storage_regression ',1)
anchor='\\t./quest_manager_indices_regression$(EXEEXT)';assert s.count(anchor)==1
s=s.replace(anchor,'\\t./pc_quest_storage_regression$(EXEEXT)\\n'+anchor,1)
lines=s.splitlines();start=next(i for i,l in enumerate(lines) if l.startswith('check:'));commands=[]
for line in lines[start+1:]:
 if not line.startswith('\\t'):break
 match=re.search(r'\\./([A-Za-z0-9_]+)\\$\\(EXEEXT\\)',line)
 if match:commands.append(match.group(1))
registry=re.search(r'^RUNTIME_TESTS = (.*)$',s,re.M).group(1).split()
assert commands==registry,('Canonical execution/registry mismatch',commands,registry)
assert commands.count('pc_quest_storage_regression')==1
changes[rel]=(s+'\\n'+(p/'Makefile.addition').read_text(encoding='utf-8-sig')).encode('utf-8')"""
s=s.replace(old,new);s=s.replace("write_text(json.dumps(plan,indent=2)+'\\n')","write_text(json.dumps(plan,indent=2)+'\\n',encoding='utf-8')");p.write_text(s,encoding='utf-8')
p=Path('work/pc_quest_storage/apply.py');s=p.read_text(encoding='utf-8-sig').replace('.read_text()','.read_text(encoding=\'utf-8-sig\')');p.write_text(s,encoding='utf-8')
p=Path('work/pc_quest_storage/validate.py');s=p.read_text(encoding='utf-8-sig');s=s.replace("a.add_argument('--make',default='C:/Strawberry/c/bin/gmake.exe');v=a.parse_args();cwd=Path(v.root)/'reconstruction/tests';out=p/'validation';out.mkdir(exist_ok=True)","a.add_argument('--make',default='C:/Strawberry/c/bin/gmake.exe');a.add_argument('--out',help='Parent-selected output directory; native test directory supported');v=a.parse_args();cwd=Path(v.root)/'reconstruction/tests';out=Path(v.out).resolve() if v.out else p/'validation';out.mkdir(parents=True,exist_ok=True)")
s=s.replace("log=out/(name+'.log')","log=out/('pc-quest-storage-'+name+'.log')").replace("log.open('w')","log.open('w',encoding='utf-8')").replace("(out/(name+'.receipt.json')).write_text(json.dumps(r,indent=2))","(out/('pc-quest-storage-'+name+'.receipt.json')).write_text(json.dumps(r,indent=2),encoding='utf-8')").replace("out/(old[:-2]+'_o2.o')","out/('pc_quest_storage_'+old[:-2]+'_o2.o')")
p.write_text(s,encoding='utf-8')
