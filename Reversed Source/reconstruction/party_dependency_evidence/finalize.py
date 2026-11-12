import pathlib,json,re
p=pathlib.Path('work/party_dependency')
s=(p/'party.h.candidate').read_text().replace('    // Emitted template instantiation: FExitDungeon ForEachNearMember<<unnamed>::FExitDungeon>(FExitDungeon);','    // Template shape proved by emitted return-T/argument-T instantiation.\n    template<class Func> Func ForEachNearMember(Func);').replace('private:\nprotected:\n','protected:\n')
(p/'party.h.candidate').write_text(s)
a=json.loads((p/'method-api.json').read_text());syms={}
for l in (p/'symbols.txt').read_text().splitlines():
 t=l.split()
 if len(t)>=6:syms[t[-1]]={'address':'0x'+t[0],'size':int(t[-2],16)}
for m in a:
 if m['linkage'] in syms:m.update(syms[m['linkage']])
(p/'method-api.json').write_text(json.dumps(a,indent=2))
(p/'method-api.txt').write_text('\n'.join((m.get('address','inline/declaration')+' '+m['signature']+' // '+str(m.get('linkage'))) for m in a)+'\n')
