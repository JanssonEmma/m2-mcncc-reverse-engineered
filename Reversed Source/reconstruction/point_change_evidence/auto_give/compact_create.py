import pathlib,json
x=json.loads(pathlib.Path('work/point_change/auto_give/immediate-ghidra.json').read_text());p=pathlib.Path('work/point_change/auto_give')
for r in x:
 if r['function']['name'].endswith('CreateItem'):
  p.joinpath('create-item-decompiled.txt').write_text('\n'.join(l for l in r['code'].splitlines() if l.strip())+'\n')
