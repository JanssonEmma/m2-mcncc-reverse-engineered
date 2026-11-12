import pathlib,json,re
p=pathlib.Path('work/point_change');rows=[]
steps=[-2147483648,-2147483647,-2147483646,-2147483645,-2147483644,-2147483643,-10,-1,0,1,2,3,4]
for target in (2,3,4):
 for step in steps:
  raw=(target-step)&0xffffffff;count=raw if raw<0x80000000 else raw-0x100000000
  rows.append({'target':target,'real_step':step,'sub_eax_hex':hex(raw),'signed_count':count,'signed_jle_skips':count<=0,'theoretical_iterations_if_selected':max(0,count)})
p.joinpath('quarter-loop-boundaries.json').write_text(json.dumps(rows,indent=2))
lines=[]
for line in p.joinpath('assembly.txt').read_text().splitlines():
 m=re.match(r'\s*([0-9a-f]+):',line)
 if m and (0x812e776<=int(m[1],16)<=0x812e7cd or 0x812e82f<=int(m[1],16)<=0x812e938):lines.append(line)
p.joinpath('quarter-loop-assembly.txt').write_text('\n'.join(lines)+'\n')
