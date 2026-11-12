"""TEST ONLY unchanged COFF function selection; no compiler or execution."""
import argparse,hashlib,json,struct
from pathlib import Path
def sha(b):return hashlib.sha256(b).hexdigest()
def project(src,dst,proof,source=None,function='thecore_random'):
 b=Path(src).read_bytes();machine,nsec,stamp,so,ns,opt,flags=struct.unpack_from('<HHLLLHH',b)
 assert opt==0 and machine in (0x8664,0x14c)
 strings=b[so+18*ns:];strings=strings[:struct.unpack_from('<L',strings)[0]]
 def name(raw):
  if raw[:4]==b'\0'*4:return strings[struct.unpack_from('<L',raw,4)[0]:].split(b'\0',1)[0].decode()
  return raw.split(b'\0',1)[0].decode()
 secs=[]
 for i in range(nsec):
  h=list(struct.unpack_from('<8sLLLLLLHHL',b,20+40*i));sn=h[0].split(b'\0',1)[0].decode()
  if sn.startswith('/'):sn=strings[int(sn[1:]):].split(b'\0',1)[0].decode()
  assert h[6]==0 and h[8]==0
  secs.append(dict(name=sn,h=h,data=b[h[4]:h[4]+h[3]],rel=[struct.unpack_from('<LLH',b,h[5]+10*j) for j in range(h[7])]))
 syms={};i=0
 while i<ns:
  raw=b[so+18*i:so+18*(i+1)];val,sec,typ,store,aux=struct.unpack_from('<LhHBB',raw,8)
  syms[i]=dict(name=name(raw[:8]),raw=raw,sec=sec,store=store,aux=[b[so+18*(i+j+1):so+18*(i+j+2)] for j in range(aux)])
  i+=aux+1
 if function == 'MAX' and machine != 0x8664:
  raise ValueError('MAX projection requires the validated x64 COFF format')
 wanted=tuple(prefix+function for prefix in ('.text$','.pdata$','.xdata$'))
 kept=[i+1 for i,s in enumerate(secs) if s['name'] in wanted]
 assert kept and secs[kept[0]-1]['name']==wanted[0]
 if function == 'MAX':
  assert {secs[i-1]['name'] for i in kept}==set(wanted) and len(kept)==len(wanted)
 sm={old:i+1 for i,old in enumerate(kept)}
 need={i for i,s in syms.items() if s['sec'] in kept}
 for old in kept:need.update(r[1] for r in secs[old-1]['rel'])
 assert all(i in syms for i in need)
 for i in need:assert syms[i]['sec'] in kept or syms[i]['sec']==0
 ext=[s['name'] for i,s in syms.items() if i in need and s['sec']==0]
 assert ext==(['random'] if function=='thecore_random' else []),ext
 assert [s['name'] for i,s in syms.items() if i in need and s['store']==2 and s['sec']>0]==[function]
 tab=bytearray();im={}
 for i in sorted(need):
  s=syms[i];im[i]=len(tab)//18;raw=bytearray(s['raw']);struct.pack_into('<h',raw,12,sm.get(s['sec'],s['sec']));tab+=raw
  for aux in s['aux']:
   assert s['store']==3 and s['sec']>0
   a=bytearray(aux);assoc=struct.unpack_from('<h',a,12)[0]
   if assoc:assert assoc in sm;struct.pack_into('<h',a,12,sm[assoc])
   tab+=a
 out=bytearray(20+40*len(kept));details=[]
 for ni,old in enumerate(kept):
  s=secs[old-1];h=s['h'][:]
  while len(out)%4:out+=b'\0'
  h[4]=len(out) if s['data'] else 0;out+=s['data'];h[5]=len(out) if s['rel'] else 0;rel=[]
  for off,idx,typ in s['rel']:
   out+=struct.pack('<LLH',off,im[idx],typ);rel.append(dict(offset=off,type=typ,symbol=syms[idx]['name']))
  struct.pack_into('<8sLLLLLLHHL',out,20+40*ni,*h)
  details.append(dict(name=s['name'],bytes_sha256=sha(s['data']),bytes=len(s['data']),relocations=rel))
 oso=len(out);out+=tab;out+=strings
 struct.pack_into('<HHLLLHH',out,0,machine,len(kept),stamp,oso,len(tab)//18,0,flags)
 for ni,old in enumerate(kept):
  h=struct.unpack_from('<8sLLLLLLHHL',out,20+40*ni);assert bytes(out[h[4]:h[4]+h[3]])==secs[old-1]['data'];rel=[]
  for j in range(h[7]):
   off,idx,typ=struct.unpack_from('<LLH',out,h[5]+10*j);raw=out[oso+idx*18:oso+(idx+1)*18]
   rel.append(dict(offset=off,type=typ,symbol=name(raw[:8])))
  assert rel==details[ni]['relocations']
 if function=='thecore_random':assert any(r['symbol']=='random' for r in details[0]['relocations'])
 Path(dst).write_bytes(out)
 report=dict(kind='TEST ONLY unchanged actual utils.c COFF function projection',full_object_sha256=sha(b),projected_object_sha256=sha(out),sections=details,undefined_symbols=ext,code_and_unwind_bytes_equal=True,relocation_names_types_offsets_equal=True,source_sha256=sha(Path(source).read_bytes()) if source else None)
 Path(proof).write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('input');p.add_argument('output');p.add_argument('proof');p.add_argument('--source');p.add_argument('--function',choices=('thecore_random','MAX'),default='thecore_random');a=p.parse_args();print(json.dumps(project(a.input,a.output,a.proof,a.source,a.function),indent=2))
