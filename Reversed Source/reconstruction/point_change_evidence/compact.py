from pathlib import Path
p=Path('work/point_change/decompiled.txt');s=p.read_text();s='\n'.join(l for l in s.splitlines() if l.strip())+'\n';Path('work/point_change/decompiled-compact.txt').write_text(s)
